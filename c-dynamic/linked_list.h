
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdlib.h>
#include <stdbool.h>

struct node {

    void *data;
    struct node *next;
    struct node *prev;

};

typedef struct linked_list {

    struct node *first;
    struct node *last;

    size_t elem_size;
    size_t length;

} linked_list;


// --- Example helpers
void print_int(void*);
bool compare_int(void*, void*);
// ---

// --- memory-handling functions
struct node *create_node(void);
void destroy_node(struct node*);
void delete_node(struct node*);
void detach_node(linked_list*, struct node*);

linked_list *init_list(size_t);
void destroy_list(linked_list*);

// ---

// --- List operations + worst case complexity ( n - length of list )

int append_to_list(linked_list*, void*);                                            // O(1)
int prepend_to_list(linked_list*, void*);                                           // O(1)
int pop_from_list(linked_list*, void*);                                             // O(1)
int popleft_from_list(linked_list*, void*);                                         // O(1)
void* search_list(linked_list*, void*, bool (*comparator) (void*, void*));          // O(n)
void print_list(linked_list*, void (*elem_printer) (void*));                        // O(n)

void* get_nth_list(linked_list*, size_t);                                           // O(n)
void remove_nth_list(linked_list*, size_t);                                         // O(n)
int pop_nth_list(linked_list*, size_t, void*);                                      // O(n)

// ---

#endif

