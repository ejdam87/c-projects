#ifndef XPARSER_H
#define XPARSER_H

#include "managed_string.h"
#include "parser.h"

#include <stdio.h>


/*****************************************************************************
 *  DATA STRUCTURES
 *****************************************************************************/

struct node {
    mchar *name;

    struct vector *keys;
    struct vector *values;
    bool empty;

    mchar *text;
    struct vector *children;
};

struct node *node_create(mchar *name, struct vector *keys, struct vector *values,
                         mchar *text, struct vector *children, bool);

void node_destroy(struct node *node);

void node_ptr_destroy(struct node **node);

void keyval_ptr_destroy(mchar **);

/*****************************************************************************
 *  MISCELLANEOUS
 *****************************************************************************/

typedef int (*predicate) (int);

mchar *parse_string(struct parsing_state *state, predicate valid_chars);

mchar *parse_word(struct parsing_state *state, predicate start,
                  predicate rest, const char *error_message);


/*****************************************************************************
 *  PARSING
 *****************************************************************************/

mchar *parse_name(struct parsing_state *state);

mchar *parse_text(struct parsing_state *state, bool normalize);

bool parse_equals(struct parsing_state *state);

mchar *parse_value(struct parsing_state *state);

bool parse_attributes(struct parsing_state *state, struct vector **keys, struct vector **values);

struct node *parse_xnode(struct parsing_state *state);

struct vector *parse_xnodes(struct parsing_state *state);

struct node *parse_root(struct parsing_state *state);

struct node *parse_xml(FILE *file);


#endif /* XPARSER_H */
