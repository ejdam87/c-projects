#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xpath.h"

int parse_args(int argc, char**argv,
               char **input, char **output, char **format,
               char **xpath){

    if (argc < 2 || argc > 7){
        fprintf(stderr, "Invalid argument count --> ");
        return EXIT_FAILURE;
    }

    int i = 1;

    while (i < argc){

        // --- Input
        if ((strcmp("-i", argv[i]) == 0) || (strcmp("--input", argv[i]) == 0)){

            if (*input != NULL){
                fprintf(stderr, "Input already set --> ");
                return EXIT_FAILURE;

            }

            i++;

            if (i == argc){
                fprintf(stderr, "Need to specify input --> ");
                return EXIT_FAILURE;
            }

            *input = argv[i];
            i++;
            continue;
        }
        // ---

        // --- Output
        if ((strcmp("-o", argv[i]) == 0) || (strcmp("--output", argv[i]) == 0)){

            if (*output != NULL){
                fprintf(stderr, "Output already set --> ");
                return EXIT_FAILURE;

            }

            i++;
            if (i == argc){
                fprintf(stderr, "Need to specify output --> ");
                return EXIT_FAILURE;
            }

            *output = argv[i];
            i++;
            continue;
        }
        // ---

        // --- Format
        if ((strcmp("-x", argv[i]) == 0) || (strcmp("--xml", argv[i]) == 0)){

            if (*format != NULL){
                fprintf(stderr, "Format already set --> ");
                return EXIT_FAILURE;

            }

            *format = argv[i];
            i++;
            continue;
        }

        else if ((strcmp("-t", argv[i]) == 0) || (strcmp("--text", argv[i]) == 0)){

            if (*format != NULL){
                fprintf(stderr, "Format already set --> ");
                return EXIT_FAILURE;

            }

            *format = argv[i];
            i++;
            continue;
        }
        else{

            if (*xpath != NULL){
                fprintf(stderr, "Invalid argument --> ");
                return EXIT_FAILURE;
            }

            *xpath = argv[i];
            i++;
            continue;

        }
        // ---

    }

    if (*input == NULL){
        *input = "stdin";
    }

    if (*output == NULL){
        *output = "stdout";
    }

    if (*format == NULL){
        *format = "-t";
    }

    return EXIT_SUCCESS;

}

int main(int argc, char **argv)
{

    char *input = NULL;
    char *output = NULL;
    char *format = NULL;
    char *xpath = NULL;

    if (parse_args(argc, argv, &input, &output, &format, &xpath) == EXIT_FAILURE){
        fprintf(stderr, "Could not parse arguments!\n");
        return EXIT_FAILURE;
    }

    if (eval_xpath(input, xpath, format, output) != EXIT_SUCCESS){
        fprintf(stderr, "Xpath evaluation failed!\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}
