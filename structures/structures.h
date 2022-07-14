
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdlib.h>

struct node {

    int *data;
    struct node *next;

};

typedef struct linked_list {

    struct node *first;
    struct node *last;

    size_t elem_size;

} linked_list;


void print_int(void*);
struct node *create_node(void);
int append_to_list(linked_list*, int*);
void print_list(linked_list*, void (*elem_printer) (void*));
linked_list *init_list(size_t);

#endif

