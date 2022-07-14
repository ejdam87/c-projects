#include "structures.h"
#include <stdlib.h>

int main() {

    size_t s = sizeof(int);
    linked_list *list = init_list(s);

    int a = 5;
    append_to_list(list, &a);

    a = 4;
    append_to_list(list, &a);
    print_list(list, print_int);

    return 0;
}
