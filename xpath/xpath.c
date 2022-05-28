#include "xpath.h"
#include "xparser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "vector.h"
#include "managed_string.h"

#define NAME_QUERY 1
#define INDEX_QUERY 2
#define ATTR_QUERY 3
#define ATTR_VAL_QUERY 4

void print_node(struct node*, FILE*, size_t);
void print_tag(struct node*, bool, FILE*, size_t);
void print_content(struct node*, FILE*, size_t);
void print_node_text(FILE*, struct node*);
struct node *node_copy(struct node*);

struct vector *copy_children(struct vector *children){

    struct vector *new = vec_create(children -> elem_size);

    for (size_t i = 0; i < vec_size(children); i++){
        struct node **node = vec_get(children, i);
        struct node *new_node = node_copy(*node);
        vec_push_back(new, &new_node);
    }

    return new;

}

struct vector *copy_args(struct vector *args){

    struct vector *new = vec_create(args -> elem_size);

    for (size_t i = 0; i < vec_size(args); i++){
        char **arg = vec_get(args, i);
        char *new_arg = str_create(*arg);
        vec_push_back(new, &new_arg);
    }

    return new;

}

struct node *node_copy(struct node *node){

    char *name = str_create(node -> name);
    char *text = NULL;
    bool empty = node -> empty;
    struct vector *children = NULL;
    struct vector *keys = NULL;
    struct vector *values = NULL;

    if (node -> text != NULL && strcmp(node -> text, "") != 0){
        text = str_create(node -> text);
    }
    if (node -> children != NULL){
        children = copy_children(node -> children);
    }
    if (node -> keys != NULL && node -> keys -> data != NULL){
        keys = copy_args(node -> keys);
        values = copy_args(node -> values);
    }

    return node_create(name, keys, values, text, children, empty);

}
// --- Deciding which query type it is (all of these functions assume correct query)
bool is_index_query(char *query){
    return strchr(query, '[') != NULL && strchr(query, '@') == NULL;
}

bool is_attr_val_query(char *query){
    return strchr(query, '[') != NULL && strchr(query, '@') != NULL && strchr(query, '=') != NULL;
}

bool is_attr_query(char *query){
    return strchr(query, '[') != NULL && strchr(query, '@') != NULL;
}
// ---

char *get_attr(char *query){

    size_t i = 0;

    while (i < strlen(query) && query[i] != '['){
        i++;
    }

    // Did not find any attrs
    if (i == strlen(query)){
        return NULL;
    }

    i++;
    char *attrs = str_create("");
    while (i < strlen(query) && query[i] != ']'){
        str_add_char(&attrs, query[i]);
        i++;
    }

    if (i == strlen(query) && query[i - 1] != ']'){
        return NULL;
    }

    return attrs;

}

char *get_name(char *query){

    char *output = str_create("");

    if (output == NULL){
        return NULL;
    }

    while (*query != 0 && *query != '['){

        str_add_char(&output, *query);
        query++;
    }

    return output;

}

char *get_value(char *attr){

    char *val = str_create("");

    while (*attr != '='){
        attr++;
    }

    attr++; // =
    attr++;
    while (*attr != '"'){
        str_add_char(&val, *attr);
        attr++;
    }

    return val;
}

bool is_correct_name(char *name){

    if (str_len(name) < 1){
        return false;
    }

    if (name[0] != '_' && !isalpha(name[0])){
        return false;
    }

    char valid_punct[] = {'_', '-', '.', ':'};

    for (size_t i = 0; i < str_len(name); i++){

        // only left is valid punct
        if (!isdigit(name[i]) && !isalpha(name[i])){

            bool found = false;
            for (size_t j = 0; j < 4; j++){
                if (name[i] == valid_punct[j]){
                    found = true;
                }
            }

            if (!found){
                return false;
            }
        }
    }

    return true;

}

char *get_attr_name(char *attr){

    char *attr_name = str_create("");
    size_t i = 1;
    while (i < str_len(attr) && attr[i] != '='){
        str_add_char(&attr_name, attr[i]);
        i++;
    }

    return attr_name;

}

bool is_correct_attr(char *attr){

    if (strcmp(attr, "") == 0){
        return false;
    }

    // must be index query --> attr has to be number
    if (strchr(attr, '@') == NULL) {

        for (size_t i = 0; i < str_len(attr); i++) {
            if (!isdigit(attr[i])) {
                return false;
            }
        }
        return true;
    }
    // ---

    if (attr[0] != '@'){
        return false;
    }

    char *attr_name = get_attr_name(attr);

    size_t i = 1;
    while (i < str_len(attr) && attr[i] != '='){
        i++;
    }

    if (!is_correct_name(attr_name)){
        str_destroy(attr_name);
        return false;
    }

    // Only attr query
    if (strchr(attr, '=') == NULL){
        str_destroy(attr_name);
        return true;
    }

    // --- From here --> we check validity of value
    i++;
    bool found_start = false;
    bool found_end = false;

    for (size_t index = i; index < str_len(attr); index++){

        if (attr[i] == '"' && !found_start){
            found_start = true;
            continue;
        }
        else if (attr[i] == '"' && found_start){
            found_end = true;
            continue;
        }

        str_destroy(attr_name);
        // non-white char after closing mark
        if (found_end && !isspace(attr[i])){
            str_destroy(attr_name);
            return false;
        }
        // non-white char before closing mark
        else if (!found_start && !isspace(attr[i])){
            str_destroy(attr_name);
            return false;
        }
    }

    str_destroy(attr_name);
    return true;

}

bool is_correct_query(char *query){

    if (query == NULL){
        return false;
    }
    if (strcmp(query, "") == 0){
        return false;
    }
    // it should have a name --> [..] is forbidden
    if (query[0] == '['){
        return false;
    }

    if (query[str_len(query) - 1] == '/'){
        return false;
    }

    char *name = get_name(query);
    if (!is_correct_name(name)){
        str_destroy(name);
        return false;
    }

    char *attrs = get_attr(query);
    if (attrs == NULL && strchr(query, '[') != NULL){
        str_destroy(name);
        return false;
    }

    bool status = true;
    if (attrs != NULL){
        status = is_correct_attr(attrs);
    }

    str_destroy(attrs);
    str_destroy(name);
    return status;

}
// ---

bool is_pair_present(struct vector *a, struct vector *b,
                     const char *key,  const char *val){

    for (size_t i = 0; i < vec_size(a); i++){

        char **pkey = vec_get(a, i);
        char **pval = vec_get(b, i);

        if (*pkey == NULL){
            continue;
        }

        if (strcmp(*pkey, key) == 0 && strcmp(*pval, val) == 0){
            return true;
        }
    }

    return false;

}

bool is_in_vector(struct vector *vect, const char *needle){

  if (vect == NULL){
    return false;
  }
    for (size_t i = 0; i < vec_size(vect); i++){

        char **present = vec_get(vect, i);
        if (strcmp(*present, needle) == 0){
            return true;
        }
    }

    return false;

}

struct node *traverse_children(struct node *input_root,
                      const char *name,
                      const char *attr,
                      const char *val,
                      const size_t *index,
                      int query_type){

    if (input_root -> children == NULL){
        return NULL;
    }

    size_t already_gathered = 0;
    struct node **first;
    struct vector *children = vec_create(sizeof(struct node *));
    size_t i = 0;
    struct node **current;
    int condition;
    size_t n = vec_size(input_root -> children);

    while (i < n){

        current = vec_get(input_root -> children, i);

        switch (query_type){
            case NAME_QUERY:
                condition = strcmp((*current) -> name, name) == 0;
                break;
            case INDEX_QUERY:
                condition = i + 1 == *index;
                break;
            case ATTR_QUERY:
                if ((*current) -> keys == NULL){
                  condition = false;
                }
                else {
                  condition = is_in_vector((*current) -> keys, attr);
                }
                break;
            case ATTR_VAL_QUERY:
                if ((*current) -> keys == NULL){
                  condition = false;
                }
                else {
                  condition = is_pair_present((*current) -> keys, (*current) -> values, attr, val);
                }
                break;
            default:
                // This should not be reached
                assert(false);
        }

        if (condition){
            already_gathered++;
            if (already_gathered == 1){
                first = current;
            }
            else if (already_gathered == 2){
                struct node *new = node_copy(*current);
                struct node *new_first = node_copy(*first);

                vec_push_back(children, &new_first);
                vec_push_back(children, &new);
            }
            else {
                struct node *new = node_copy(*current);
                vec_push_back(children, &new);
            }
        }
        i++;
    }

    // Only one result
    if (already_gathered == 1){
        vec_destroy(children, DESTRUCTOR(node_ptr_destroy));
        return *first;
    }

    if (vec_size(children) == 0){
        vec_destroy(children, DESTRUCTOR(node_ptr_destroy));
        return NULL;
    }

    str_destroy(input_root -> name);
    str_destroy(input_root -> text);
    vec_destroy(input_root -> keys, DESTRUCTOR(keyval_ptr_destroy));
    vec_destroy(input_root -> values, DESTRUCTOR(keyval_ptr_destroy));
    vec_destroy(input_root -> children, DESTRUCTOR(node_ptr_destroy));

    input_root -> name = str_create("result");
    input_root -> text = NULL;
    input_root -> keys = NULL;
    input_root -> values = NULL;
    input_root -> children = children;
    input_root -> empty = false;

    return input_root;

}

int parse_number(char *open_bracket){

    open_bracket++;
    char *str_num = str_create("");

    while (*open_bracket != ']'){
        str_add_char(&str_num, *open_bracket);
        open_bracket++;
    }

    int res = atoi(str_num);
    str_destroy(str_num);
    return res;

}

void insert_node(struct vector *vec, struct node **node){

    if (strcmp((*node) -> name, "result") != 0){
        vec_push_back(vec, node);
    }
    else {
        for (size_t i = 0; i < vec_size((*node) -> children); i++){
            struct node **child = vec_get((*node) -> children, i);
            struct node *child_copy = node_copy(*child);
            vec_push_back(vec, &child_copy);
        }
        node_destroy(*node);
    }
}

struct node *eval_query(struct node *input_root, char *query){

    if (!is_correct_query(query)){
        return NULL;
    }

    char *name = NULL;
    char *opening_bracket = NULL;



    // If we have our result nested in 'result' tag
    if (strcmp(input_root -> name, "result") == 0){

        struct vector *result = vec_create(sizeof(struct node*));

        for (size_t i = 0; i < vec_size(input_root -> children); i++){

            struct node **child = vec_get(input_root -> children, i);
            struct node *res = eval_query(*child, query);
            if (res != NULL){
              struct node *new_node = node_copy(res);
              insert_node(result, &new_node);
            }
        }

        vec_destroy(input_root -> children, DESTRUCTOR(node_ptr_destroy));
        input_root -> children = result;
        if (vec_size(input_root -> children) == 0){
          return NULL;
        }
        return input_root;
    }

    // --- Only name query
    if (strchr(query, '[') == NULL){
        name = query;

        // if it is us
        if (strcmp(name, input_root -> name) == 0){
            return input_root;
        }

        return traverse_children(input_root, name, NULL, NULL, NULL, NAME_QUERY);

    }

    // --- Index query (contains '[' but does not contain '@')
    else if (is_index_query(query)){

        // get opening bracket
        opening_bracket = query;
        while (*opening_bracket != '['){
            opening_bracket++;
        }

        int index = parse_number(opening_bracket);

        if (index < 1){
            fprintf(stderr, "Invalid index --> ");
            return NULL;
        }

        size_t valid_index = (size_t) index;
        char *qname = get_name(query);

        struct node *wanted_names = traverse_children(input_root, qname, NULL,
                                                      NULL, NULL, NAME_QUERY);

        struct node *res = NULL;
        if (wanted_names == NULL){
            str_destroy(qname);
            return NULL;
        }

        if (strcmp(wanted_names -> name, qname) == 0){
          if (valid_index == 1){
            return wanted_names;
          }
        }
        else{
          res = traverse_children(wanted_names, NULL, NULL,
                                  NULL, &valid_index, INDEX_QUERY);
        }

        str_destroy(qname);
        return res;

    }

    // --- name[@attr="val"]
    else if (is_attr_val_query(query)){

        char *attr = get_attr(query);
        char *attr_name = get_attr_name(attr);
        char *val_name = get_value(attr);
        char *qname = get_name(query);

        struct node *wanted_names = traverse_children(input_root, qname, NULL,
                                                      NULL, NULL, NAME_QUERY);

        if (wanted_names == NULL){
            str_destroy(qname);
            str_destroy(attr);
            str_destroy(attr_name);
            str_destroy(val_name);
            return NULL;
        }

        struct node *res = NULL;
        if (strcmp(wanted_names -> name, qname) == 0){
          if (is_pair_present(wanted_names -> keys, wanted_names -> values, attr_name, val_name)){
            res = wanted_names;
          }
        }
        else{
          res = traverse_children(wanted_names, NULL, attr_name,
                                  val_name, NULL, ATTR_VAL_QUERY);
        }


        str_destroy(qname);
        str_destroy(attr);
        str_destroy(attr_name);
        str_destroy(val_name);
        return res;
    }

    // --- name[@attr]
    else if (is_attr_query(query)){

        char *attr = get_attr(query);
        char *attr_name = get_attr_name(attr);
        char *qname = get_name(query);

        struct node *wanted_names = traverse_children(input_root, qname, NULL,
                                                      NULL, NULL, NAME_QUERY);
        if (wanted_names == NULL){
            str_destroy(attr);
            str_destroy(attr_name);
            str_destroy(qname);
            return NULL;
        }

        struct node *res = NULL;

        if (strcmp(wanted_names -> name, qname) == 0){
            if (is_in_vector(wanted_names -> keys, attr_name)){
              res = wanted_names;
            }
        }
        else {
          res = traverse_children(wanted_names, NULL, attr_name,
                                  NULL, NULL, ATTR_QUERY);
        }

        str_destroy(attr);
        str_destroy(attr_name);
        str_destroy(qname);
        return res;

    }

    return NULL;

}

FILE* get_fd(char *file_path, bool input){
    // --- Open file
    FILE *out = NULL;

    char *base;

    if (input){
        base = "stdin";
    }
    else {
        base = "stdout";
    }

    if (strcmp(file_path, base) != 0){

        if (input){
            out = fopen(file_path, "r");
        }
        else {
            out = fopen(file_path, "w");
        }

    }

    if (out == NULL && strcmp(file_path, base) != 0){
        fprintf(stderr, "Cannot open output file --> ");
        return NULL;
    }

    if (strcmp(file_path, base) == 0){

        if (input){
            out = stdin;
        }
        else {
            out = stdout;
        }

    }

    return out;

}

// --- XML output
void print_spaces(FILE *out, size_t count){

    for (size_t i = 0; i < count; i++){
        fputc(' ', out);
    }

}

void print_node(struct node *node, FILE *out, size_t indent){

    if (node -> empty){
        print_tag(node, false, out, indent);
        fputc('\n', out);
        return;
    }

    print_tag(node, false, out, indent);

    if (node -> children != NULL){
        fputc('\n', out);
        print_content(node, out, indent + 1);
    }
    else if (node -> text != NULL){
        print_content(node, out, indent + 1);
    }
    else{
        fputc('\n', out);
    }

    print_tag(node, true, out, indent);
    fputc('\n', out);

}

void print_content(struct node *node, FILE *out, size_t indent){

    if (node -> text != NULL){
        print_node_text(out, node);
    }
    else if (node -> children != NULL){

        for (size_t i = 0; i < vec_size(node -> children); i++){
            struct node **child = vec_get(node -> children, i);
            print_node(*child, out, indent);
        }
    }
}

void print_tag(struct node *node, bool closing, FILE *out, size_t indent){

    if (!closing || node -> text == NULL){
        print_spaces(out, indent * 4);
    }

    if (closing){
        fprintf(out, "%s", "</ ");
    }
    else{
        fprintf(out, "%s", "< ");
    }

    // Add name
    fprintf(out, "%s", node -> name);

    // Add attrs
    if (!closing && node -> keys != NULL){

        for (size_t i = 0; i < vec_size(node -> keys); i++){

            mchar **key = vec_get(node -> keys, i);
            mchar **value = vec_get(node -> values, i);

            if (*key == NULL){
                break;
            }

            fputc(' ', out);
            fprintf(out, "%s", *key);
            fputc('=', out);
            fputc('"', out);
            fprintf(out, "%s", *value);
            fputc('"', out);
        }
    }

    if (node -> empty){
        fprintf(out, "%s", " />");
    }
    else{
        fprintf(out, "%s", " >");
    }

}

int print_xml_to_file(char *file_path, struct node *root){

    FILE *out = get_fd(file_path, false);

    if (out == NULL){
        return EXIT_FAILURE;
    }

    size_t indent = 0;
    print_node(root, out, indent);

    if (strcmp(file_path, "stdout") != 0){
        fclose(out);
    }

    return EXIT_SUCCESS;

}
// ---

// --- Text output
void print_node_text(FILE *out, struct node* node){

    char *text = node -> text;
    bool have_space = false;
    bool in_mid = false;

    for (size_t i = 0; i < str_len(text); i++){

        if (!isspace(text[i])){
            in_mid = true;
            if (have_space){
                fputc(' ', out);
                have_space = false;
            }
            fputc(text[i], out);
        }
        else if (in_mid){
            have_space = true;
        }
    }

}

void print_text(FILE *out, struct node* node){

    if (node == NULL){
        return;
    }

    if (node -> text  != NULL){
        print_node_text(out, node);
        fputc('\n', out);
    }

    if (node -> children == NULL){
        return;
    }

    size_t i = 0;
    struct node **current;

    while (i < vec_size(node -> children)){
        current = vec_get(node -> children, i);
        print_text(out, *current);
        i++;
    }

}

int print_text_to_file(char *file_path, struct node *root){

    FILE *out = get_fd(file_path, false);

    if (out == NULL){
        return EXIT_FAILURE;
    }

    print_text(out, root);
    if (strcmp(file_path, "stdout") != 0){
        fclose(out);
    }

    return EXIT_SUCCESS;

}
// ---


int eval_xpath(char *in_path, char *query, char *format, char *out_path){

    if (query == NULL){
        return EXIT_FAILURE;
    }

    if (query[0] != '/'){
        return EXIT_FAILURE;
    }

    if (query[strlen(query) -1] == '/'){
      return EXIT_FAILURE;
    }

    FILE *in = get_fd(in_path, true);

    if (in == NULL){
        return EXIT_FAILURE;
    }

    struct node *root = parse_xml(in);

    if (root == NULL){
        fprintf(stderr, "Could not parse XML! --> ");
        return EXIT_FAILURE;
    }

    fclose(in);

    // --- traverse query
    size_t i = 1;
    struct node *current = root;

    char *single_query = str_create("");

    if (single_query == NULL){
        return EXIT_FAILURE;
    }

    while (i < strlen(query)){

        // --- get subquery
        while (i < strlen(query) && query[i] != '/'){

            str_add_char(&single_query, query[i]);
            i++;

        }
        // ---

        // --- Eval subquery
        current = eval_query(current, single_query);

        if (current == NULL){
            node_destroy(root);

            if (is_correct_query(single_query)){
                str_destroy(single_query);
                return EXIT_SUCCESS;
            }
            str_destroy(single_query);
            return EXIT_FAILURE;

        }

        str_clear(single_query);
        i++;

    }

    if (strcmp(format, "--text") == 0 || strcmp(format, "-t") == 0){
        if (print_text_to_file(out_path, current) != EXIT_SUCCESS){
            return EXIT_FAILURE;
        }
    }
    else{
        if (print_xml_to_file(out_path, current) != EXIT_SUCCESS){
            return EXIT_FAILURE;
        }
    }

    str_destroy(single_query);
    node_destroy(root);

    return EXIT_SUCCESS;

}
