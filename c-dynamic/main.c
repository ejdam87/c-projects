#include "linked_list.h"

#include <stdio.h>

int main() {

    linked_list *list = init_list(sizeof(int));

    for (int i = 0; i < 10; i++){

        prepend_to_list(list, &i);
        append_to_list( list, &i );

    }

    int elem;
    int want = 5;
    print_list( list, print_int );

    pop_nth_list( list, 1, &elem );

    print_list( list, print_int );

    printf( "%i\n", elem );

    destroy_list( list );

    return 0;
}
