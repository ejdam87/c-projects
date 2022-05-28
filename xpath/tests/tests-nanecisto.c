// #desc: Testy nanečisto
// #name: nanecisto
#include "cut.h"

#include "mainwrap.h"

#include <stdlib.h>
#include "utils_for_tests.h"

#define ARGS(...) \
    char *argv[] = { __VA_ARGS__, NULL }; \
    int argc = (sizeof(argv) / sizeof(*argv)) - 1

/* Provided tests are testing your outputs in two ways:
 *
 * 1) Text format is tested on exact match on stdout via macro ASSERT_FILE().
 *
 * 2) XML format is stored in file 'out.xml' (which is later removed)
 *    and tested with special function match().
 *    As there is no strict formating rule on your XML in assignment,
 *    mentioned function compares your xml with special pattern.
 *
 *    Pattern:
 *    - where can be 0..N whitespaces, whitespaces in patern ends with '*'
 *    - where can be 1..N whitespaces, whitespaces in patern ends with '+'
 *    - where can be 0..1 whitespaces, whitespaces in patern ends with '?' (not used)
 *
 * You can write your own tests in similar manner.
 **/

// #desc: nanecisto_linear: Xpath s -t argumentem na jednoduchém xml
TEST(nanecisto_linear)
{
    INPUT_STRING(
        "<bookstore>\n"
        "  <book category=\"cooking\">\n"
        "    <title lang=\"en\">Everyday Italian</title>\n"
        "  </book>\n"
        "</bookstore>"
    );

    ARGS("xpath", "-t", "/bookstore");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);

    ASSERT_FILE(stdout,
        "Everyday Italian\n"
    );
}

// #desc: nanecisto_zadani: Xpath s -x argumentem na xml ze zadání
TEST(nanecisto_zadani)
{
    INPUT_STRING(
        "<bookstore>\n"
        "  <book category=\"cooking\">\n"
        "    <title lang=\"en\">Everyday Italian</title>\n"
        "    <author>Giada De Laurentiis</author>\n"
        "    <year>2005</year>\n"
        "    <price>30.00</price>\n"
        "  </book>\n"
        "</bookstore>\n"
    );

    const char *test_model =
        "  *< *book +category *= *\"cooking\" *>"
        "    *< *title +lang *= *\"en\" *> *Everyday +Italian *</ *title *>"
        "    *< *author *> *Giada +De +Laurentiis *</ *author *>"
        "    *< *year *> *2005 *</ *year *>"
        "    *< *price *> *30.00 *</ *price *>"
        "  *</ *book *>"
    ;

    char *student_file = "out.xml";
    ARGS("xpath", "-x", "-o", student_file, "/bookstore/book");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);

    char *student_str = NULL;
    ASSERT(!file_to_string(student_file, &student_str));
    int test_retval = match(student_str, test_model);
    free(student_str);
    remove(student_file);
    ASSERT(test_retval == 1);
}

// #desc: nanecisto_zadani_vic_attrs: Xpath s -x argumentem na xml ze zadání s více atributy
TEST(nanecisto_zadani_vic_attrs)
{
    INPUT_STRING(
        "  <bookstore >\n"
        "  <book category = \"cooking\" colored = \"true\" >\n"
        "    <title lang = \"en\" > Everyday Italian </title >\n"
        "    <author > Giada De Laurentiis </author >\n"
        "    <year > 2005 </year >\n"
        "    <price > 30.00 </price >\n"
        "  </book >\n"
        "  </bookstore          >\n"
    );

    const char *test_model =
        "  *< *bookstore *>"
        "  *< *book +category *= *\"cooking\" +colored *= *\"true\" *>"
        "    *< *title +lang *= *\"en\" *> *Everyday +Italian *</ *title *>"
        "    *< *author *> *Giada +De +Laurentiis *</ *author *>"
        "    *< *year *> *2005 *</ *year *>"
        "    *< *price *> *30.00 *</ *price *>"
        "  *</ *book *>"
        "  *</ *bookstore *>"
    ;

    char *student_file = "out.xml";
    ARGS("xpath", "-x", "-o", student_file, "/bookstore");
    int main_retval = student_main(argc, argv);
    ASSERT(main_retval == 0);

    char *student_str = NULL;
    ASSERT(!file_to_string(student_file, &student_str));
    int test_retval = match(student_str, test_model);
    free(student_str);
    remove(student_file);
    ASSERT(test_retval == 1);
}

