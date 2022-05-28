#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <pwd.h>
#include <grp.h>
#include <libgen.h>

#define STICKY_BIT 01000
#define FAIL       1      // Return value if function fails
#define SUCCESS    0      // Return value if function succeed


char *get_content(FILE *file){
    /*
     Function returns str-content of file
     (using malloc --> need to free after)
    */

    size_t size = 1024;
    char *output = malloc(sizeof(char) * size);

    if (output == NULL){
        return NULL;
    }

    size_t current = 0;

    int ch = fgetc(file);
    while (ch != EOF){

        if (current == size){

            char *new = realloc(output, sizeof(char) * size * 2);

            if (new == NULL){
                free(output);
                return NULL;
            }

            output = new;
            size *= 2;
        }

        output[current] = (char) ch;
        ch = fgetc(file);
        current++;

    }

    return output;
}

bool empty_flags(const char flags[4]){
    /*
     Function checks if given flags are all empty
    */

    for (size_t i = 0; i < 4; i++){

        if (flags[i] == 0){
            break;
        }
        if (flags[i] != '-'){
            return false;
        }
    }

    return true;
}

void shift_string(char **str_ptr, size_t n){
    /*
     Function shift str_ptr by n places
    */

    for (size_t i = 0; i < n; i++){
        (*str_ptr)++;
    }
}

int change_permissions(struct stat stats, char *file_path, char *new_perms){
    /*
     Function changes permissions of file_path with new_perms
    */

    char *name = strstr(new_perms, "file: ");
    char *flags = strstr(new_perms, "flags: ");
    char *user_perms = strstr(new_perms, "user::");
    char *group_perms = strstr(new_perms, "group::");
    char *other_perms = strstr(new_perms, "other::");

    if (name == NULL || flags == NULL || user_perms == NULL ||
        group_perms == NULL || other_perms == NULL) {
        return FAIL;
    }

    shift_string(&user_perms, strlen("user::"));
    shift_string(&group_perms, strlen("group::"));
    shift_string(&other_perms, strlen("other::"));
    shift_string(&flags, strlen("flags::"));
    shift_string(&name, strlen("file: "));

    mode_t mode = stats.st_mode;

    if (strcmp(basename(name), basename(file_path)) != 0){
        fprintf(stderr, "Names does not match! \n");
    }

    // --- Changing perms
    // flags
    if (flags[0] == 's') {
        mode |= S_ISUID;
    }
    else {
        mode &= ~(S_ISUID);
    }
    if (flags[1] == 's') {
        mode |= S_ISGID;
    }
    else {
        mode &= ~(S_ISGID);
    }
    if (flags[2] == 't') {
        mode |= STICKY_BIT;
    }
    else {
        mode &= ~(STICKY_BIT);
    }

    // User
    if (user_perms[0] == 'r') {
        mode |= S_IRUSR;
    }
    else {
        mode &= ~(S_IRUSR);
    }
    if (user_perms[1] == 'w') {
        mode |= S_IWUSR;
    }
    else {
        mode &= ~(S_IWUSR);
    }
    if (user_perms[2] == 'x') {
        mode |= S_IXUSR;
    }
    else {
        mode &= ~(S_IXUSR);
    }

    // Group
    if (group_perms[0] == 'r') {
        mode |= S_IRGRP;
    }
    else {
        mode &= ~(S_IRGRP);
    }
    if (group_perms[1] == 'w') {
        mode |= S_IWGRP;
    }
    else {
        mode &= ~(S_IWGRP);
    }
    if (group_perms[2] == 'x') {
        mode |= S_IXGRP;
    }
    else {
        mode &= ~(S_IXGRP);
    }

    // Other
    if (other_perms[0] == 'r') {
        mode |= S_IROTH;
    }
    else {
       mode &= ~(S_IROTH);
    }
    if (other_perms[1] == 'w') {
        mode |= S_IWOTH;
    }
    else {
        mode &= ~(S_IWOTH);
    }
    if (other_perms[2] == 'x') {
        mode |= S_IXOTH;
    }
    else {
        mode &= ~(S_IXOTH);
    }

    chmod(file_path, mode);

    return SUCCESS;

}

int resolve_print_stats(struct stat stats, FILE *out, char *relative_path){
    /*
     Function resolves file permissions from stats and then prints them to out
    */

    struct passwd *pw = getpwuid(stats.st_uid);
    struct group *gr = getgrgid(stats.st_gid);

    char flags[4] = {0};
    char user[4] = {0};
    char group[4] = {0};
    char other[4] = {0};

    if (stats.st_mode & S_ISUID) {flags[0] = 's';} else {flags[0] = '-';}
    if (stats.st_mode & S_ISGID) {flags[1] = 's';} else {flags[1] = '-';}
    if (stats.st_mode & STICKY_BIT) {flags[2] = 't';} else {flags[2] = '-';}

    if (stats.st_mode & S_IRUSR) {user[0] = 'r';} else {user[0] = '-';}
    if (stats.st_mode & S_IRUSR) {user[1] = 'w';} else {user[1] = '-';}
    if (stats.st_mode & S_IRUSR) {user[2] = 'x';} else {user[2] = '-';}

    if (stats.st_mode & S_IRGRP) {group[0] = 'r';} else {group[0] = '-';}
    if (stats.st_mode & S_IWGRP) {group[1] = 'w';} else {group[1] = '-';}
    if (stats.st_mode & S_IXGRP) {group[2] = 'x';} else {group[2] = '-';}

    if (stats.st_mode & S_IROTH) {other[0] = 'r';} else {other[0] = '-';}
    if (stats.st_mode & S_IWOTH) {other[1] = 'w';} else {other[1] = '-';}
    if (stats.st_mode & S_IXOTH) {other[2] = 'x';} else {other[2] = '-';}


    fprintf(out, "# file: %s\n", relative_path);
    fprintf(out, "# owner: %s\n", pw -> pw_name);
    fprintf(out, "# group: %s\n", gr -> gr_name);

    if (!empty_flags(flags)){
        fprintf(out, "# flags: %s\n", flags);
    }

    fprintf(out, "# user::%s\n", user);
    fprintf(out, "# group::%s\n", group);
    fprintf(out, "# other::%s\n", other);

    fputc('\n', out);

    return SUCCESS;
}

int add_slash(char *str){

    strcat(str, "/");
    return SUCCESS;

}

int get_path(char *prefix, const char *name){
    /*
     Function creates path from prefix (ending with directory) and name
    */

    if (prefix[strlen(prefix) - 1] != '/'){
        add_slash(prefix);
    }

    strcat(prefix, name);
    return SUCCESS;

}

int get_stats(char    file_name[],
              struct  dirent *dir_entry,
              char    file_path[],
              char    **relative_path,
              struct  stat *sb){
    /*
     Function resolves i-node statistics of file_name
    */

    strcpy(file_name, dir_entry->d_name);
    get_path(file_path, file_name);
    if (*relative_path == NULL){
        *relative_path = file_name;
    }
    else {
        get_path(*relative_path, file_name);
    }

    if (lstat(file_path, sb) == -1) {
        fprintf(stderr, "Could not get stats of %s\n", file_name);
        return FAIL;
    }

    return SUCCESS;

}

void clear_names(struct dirent **names, int length){
    /*
     Function clears output of scandir
    */

    for (int i = 0; i < length; i++){
        free(names[i]);
    }
    free(names);
}

bool is_spec_dir(char *name){
    return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
}

bool is_special(mode_t file_mode){
    return !(S_ISREG(file_mode) || S_ISDIR(file_mode));
}

int traverse_dir(char path[PATH_MAX], bool initial, char *mode,
                 char *perm_content, FILE *f, char *relative_path) {
    /*
     Function traverses given directory while executing in given mode (i/e)
    */

    struct dirent **names;
    int length;
    char file_name[PATH_MAX] = {0};
    char file_path[PATH_MAX] = {0};
    struct stat sb;

    char original_relative[PATH_MAX] = {0};
    if (relative_path != NULL){
        strcpy(original_relative, relative_path);
    }

    length = scandir(path, &names, NULL, alphasort);
    int i = 0;

    // --- traverse directories first
    while (i < length) {

        strcpy(file_path, path);
        if (strcmp(original_relative, "") != 0) {
            strcpy(relative_path, original_relative);
        }
        else {
            relative_path = NULL;
        }

        if (get_stats(file_name, names[i], file_path, &relative_path, &sb) == FAIL){
            fprintf(stderr, "Failed to read file: %s", file_name);
            clear_names(names, length);
            return FAIL;
        }

        i++;
        // --- We do not want reg files
        if (S_ISREG(sb.st_mode)){
            continue;
        }

        if (is_special(sb.st_mode)){
            fprintf(stderr, "Found special file!\n");
            clear_names(names, length);
            return FAIL;
        }

        if (strcmp(file_name, "..") == 0) {
            continue;
        }

        if (strcmp(file_name, ".") != 0) {
            if (strcmp(mode, "-e") == 0) {
                resolve_print_stats(sb, f, relative_path);
            } else {

                if (change_permissions(sb, file_path, perm_content) == FAIL){
                    fprintf(stderr, "Invalid input file provided!\n");
                    return FAIL;
                }

            }
        } else if (initial) {

            if (strcmp(mode, "-e") == 0) {
                resolve_print_stats(sb, f, relative_path);
            } else {

                if (change_permissions(sb, file_path, perm_content) == FAIL){
                    fprintf(stderr, "Invalid input file provided!\n");
                    return FAIL;
                }

            }

            initial = false;
        }

        if (!is_spec_dir(file_name)) {
            add_slash(file_path);

            if (perm_content != NULL) {perm_content++;}
            traverse_dir(file_path, false, mode, perm_content, f, relative_path);
        }
        // ---
    }

    clear_names(names, length);
    length = scandir(path, &names, NULL, alphasort);
    i = 0;

    // --- traverse files
    while (i < length) {

        strcpy(file_path, path);
        if (strcmp(original_relative, "") != 0) {
            strcpy(relative_path, original_relative);
        }
        else {
            relative_path = NULL;
        }

        get_stats(file_name, names[i], file_path, &relative_path, &sb);
        i++;

        // --- We do not want dirs
        if (S_ISDIR(sb.st_mode)){
            continue;
        }

        if (strcmp(mode, "-e") == 0){
            resolve_print_stats(sb, f, relative_path);
        }
        else {

            if (change_permissions(sb, file_path, perm_content) == FAIL){
                fprintf(stderr, "Invalid input file provided!\n");
                return FAIL;
            }

        }
    }

    clear_names(names, length);
    return SUCCESS;
}

int main(int argc, char** argv) {

    // -- Arg parse
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Invalid argument count provided!\n");
        puts("Syntax: './checkperms <e/i> <perm_file> <dir_to_traverse>'");
        return EXIT_FAILURE;
    }
    if (strcmp(argv[1], "-e") == 0 && strcmp(argv[1], "-i") == 0){
        fprintf(stderr, "Invalid option provided\n");
        return EXIT_FAILURE;
    }

    char *mode = NULL;
    char *perm_file = NULL;

    int c = getopt(argc, argv, "e:i:");
    switch (c){
        case 'e':
            mode = "-e";
            perm_file = optarg;
            break;
        case 'i':
            mode = "-i";
            perm_file = optarg;
            break;
        case '?':
            fprintf(stderr, "Invalid option provided!\n");
            return EXIT_FAILURE;
        default:
            abort();
    }

    char dir[PATH_MAX] = {0};

    if (argc != 4){
        getcwd(dir, sizeof(dir));
    }
    else {
        strcpy(dir, argv[3]);
    }

    if (dir[strlen(dir) -1] != '/'){
        add_slash(dir);
    }
    // ---

    // --- Driven code
    char *perm_content = NULL;
    FILE *f = NULL;

    if (strcmp(mode, "-e") == 0){
        f = fopen(perm_file, "w");
    }
    else {
        f = fopen(perm_file, "r");
    }

    if (f == NULL){
      fprintf(stderr, "Could not open perm file!\n");
      return EXIT_FAILURE;
    }

    if (strcmp(mode, "-i") == 0){

        perm_content = get_content(f);
        if (perm_content == NULL){
            fprintf(stderr, "Reading input file failed!\n");
            fclose(f);
            free(perm_content);
            return EXIT_FAILURE;
        }
    }

    char *relative_path = NULL;
    if (traverse_dir(dir, true, mode, perm_content, f, relative_path) == FAIL){
        fclose(f);
        free(perm_content);
        return EXIT_FAILURE;
    }

    // --- Cleaning
    if (perm_content != NULL){
        free(perm_content);
    }

    fclose(f);
    // ---

    return EXIT_SUCCESS;
    // ---

}
