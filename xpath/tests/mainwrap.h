#ifndef MAINWRAP_H
#define MAINWRAP_H

#ifndef WRAP_SYMBOL
#   define WRAP_SYMBOL student_main
#endif

#ifndef WRAP_FILE
#   define WRAP_FILE "../main.c"
#endif

#ifndef WRAP_INDIRECT
#   define main __PB071_wrapped_main
#   include WRAP_FILE
#   undef main
#else
int __PB071_wrapped_main(int argc, char **argv);
#endif

#include "exitus.h"

static inline
int WRAP_SYMBOL(int argc, char **argv)
{
    return exitus(__PB071_wrapped_main, argc, argv);
}

#endif // MAINWRAP_H
