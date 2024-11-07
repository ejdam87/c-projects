#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0
#define FAILURE 1

// --- Example helpers

void print_int(void *elem){
    printf("%d", *(int *) elem);
}

bool compare_int(void *a, void *b){
    return *(int*) a == *(int*) b;
}

// ---


linked_list *init_list(size_t elem_size){

    linked_list * list = malloc(sizeof(linked_list));
    list -> elem_size = elem_size;
    list -> length = 0;
    list -> first = NULL;
    list -> last = NULL;
    return list;

}

void destroy_list(linked_list *list){

    struct node *node = list -> first;
    if (node == NULL){
        return;
    }

    struct node *next;

    while (node != NULL){

        next = node -> next;

        delete_node( node );

        node = next;
    }

    free(list);

}

struct node *create_node(void){

    struct node *new_node = malloc(sizeof(struct node));

    if (new_node == NULL){
        return NULL;
    }

    new_node -> data = NULL;
    new_node -> next = NULL;
    new_node -> prev = NULL;

    return new_node;
}

int fill_node(linked_list *list, struct node *node, void *elem){

    if (node == NULL){
        return FAILURE;
    }

    node -> data = malloc(list -> elem_size);
    memcpy(node -> data, elem, list -> elem_size);
    return SUCCESS;

}

void delete_node(struct node *node){

    destroy_node(node);
    free(node);

}

void destroy_node(struct node *node){

    free(node -> data);

}

void detach_node(linked_list *list, struct node *node){

    if (node -> prev != NULL){
        node -> prev -> next = node -> next;
    }
    else {
        list -> first = node -> next;
    }

    if (node -> next != NULL){
        node -> next -> prev = node -> prev;
    }
    else {
        list -> last = node -> prev;
    }


    if (list -> length == 1){
        list -> first = NULL;
        list -> last = NULL;
    }

}

int append_to_list(linked_list *list, void *elem){

    struct node *node = create_node();
    if (fill_node(list, node, elem) == FAILURE){
        delete_node( node );
        return FAILURE;
    }

    if (list -> last == NULL){

        list -> first = node;
        list -> last = node;
        list -> length++;
        return SUCCESS;

    }

    list -> last -> next = node;
    node -> prev = list -> last;
    list -> last = node;
    list -> length++;
    return SUCCESS;

}

int prepend_to_list(linked_list *list, void *elem){

    struct node *node = create_node();
    if (fill_node(list, node, elem) == FAILURE){
        delete_node( node );
        return FAILURE;
    }

    if (list -> first == NULL){
        list -> first = node;
        list -> last = node;
        list -> length++;
        return SUCCESS;
    }

    node -> next = list -> first;
    list -> first -> prev = node;
    list -> first = node;
    list -> length++;

    return SUCCESS;

}

int pop_from_list( linked_list *list, void* place ){

    if (list -> length == 0){
        return FAILURE;
    }

    struct node *lst = list -> last;
    list -> last = list -> last -> prev;

    if (list -> last == NULL) {
        list -> first = NULL;
    }
    else if (list -> last == list -> first) {
        list -> last -> next = NULL;
        list -> last -> prev = NULL;
        list -> first -> next = NULL;
        list -> first -> prev = NULL;
    }
    else {
        list -> last -> next = NULL;
    }

    list -> length--;

    memcpy( place, lst -> data, list -> elem_size );
    delete_node( lst );

    return SUCCESS;

}

int popleft_from_list(linked_list *list, void* place){

    if (list -> first == NULL){
        return FAILURE;
    }

    struct node *fst = list -> first;
    list -> first = list -> first -> next;

    if (list -> first == NULL) {
        list -> last = NULL;
    }
    else if (list -> last == list -> first) {
        list -> last -> next = NULL;
        list -> last -> prev = NULL;
        list -> first -> next = NULL;
        list -> first -> prev = NULL;
    }
    else {
        list -> first -> prev = NULL;
    }

    list -> length--;

    memcpy( place, fst -> data, list -> elem_size );
    delete_node( fst );

    return SUCCESS;

}

void* search_list(linked_list *list, void* val, bool (*comparator) (void*, void*)){

    for (struct node *current = list -> first; current != NULL; current = current -> next){

        if (comparator(current -> data, val)){
            return current -> data;
        }
    }

    return NULL;

}

void* get_nth_list(linked_list *list, size_t n){

    if (n >= list -> length){
        return NULL;
    }

    struct node *nth_node = list -> first;

    for (int i = 0; i < n; i++){

        if (nth_node == NULL){
            return NULL;
        }

        nth_node = nth_node -> next;

    }

    return nth_node -> data;

}

void remove_nth_list(linked_list *list, size_t n){

    if (n >= list -> length){
        return;
    }

    struct node *nth_node = list -> first;

    for (int i = 0; i < n; i++){

        if (nth_node == NULL){
            return;
        }

        nth_node = nth_node -> next;

    }

    if (nth_node == NULL){
        return;
    }

    detach_node(list, nth_node);
    list -> length--;
    delete_node(nth_node);

}

int pop_nth_list(linked_list *list, size_t n, void* place){

    if (n >= list -> length){
        return FAILURE;
    }

    struct node *nth_node = list -> first;

    for (int i = 0; i < n; i++){

        if (nth_node == NULL){
            return FAILURE;
        }

        nth_node = nth_node -> next;

    }

    if (nth_node == NULL){
        return FAILURE;
    }

    detach_node(list, nth_node);
    list -> length--;

    memcpy( place, nth_node -> data, list -> elem_size );
    delete_node( nth_node );

    return SUCCESS;

}

void print_list(linked_list *list, void (*elem_printer) (void*)){

    for (struct node *node = list -> first; node != NULL; node = node -> next){
        elem_printer(node -> data);
        printf(" -> ");
    }

    putchar('\n');

}