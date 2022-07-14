
#include "structures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void print_int(void *elem){
    printf("%d", *(int *) elem);
}


linked_list *init_list(size_t elem_size){

    linked_list * list = malloc(sizeof(linked_list));
    list -> elem_size = elem_size;
    return list;

}


struct node *create_node(void){

    struct node *new_node = malloc(sizeof(struct node));

    if (new_node == NULL){
        return NULL;
    }

    new_node -> data = NULL;
    new_node -> next = NULL;

    return new_node;
}

int append_to_list(linked_list *list, int*elem){

    struct node *node = create_node();
    if (node == NULL){
        return 1;
    }

    node -> data = malloc(list -> elem_size);
    memcpy(node -> data, elem, list -> elem_size);

    if (list -> last == NULL){

        list -> first = node;
        list -> last = node;
        return 0;

    }

    list -> last -> next = node;
    list -> last = node;

    return 0;

}

void print_list(linked_list *list, void (*elem_printer) (void*)){

    for (struct node *node = list -> first; node != NULL; node = node -> next){
        elem_printer(node -> data);
        printf(" -> ");
    }

    putchar('\n');

}