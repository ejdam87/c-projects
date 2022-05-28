#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

uint64_t rotate_left(uint64_t n, uint64_t shift) {

    // From definition
    uint64_t length = sizeof(n) * 8; // count of bits
    shift %= length;
    uint64_t l_shifted = n << shift;
    uint64_t r_shifted = n >> (length - shift);
    return l_shifted | r_shifted;

}

uint64_t rotate_right(uint64_t n, uint64_t shift) {

    // from definition
    uint64_t length = sizeof(n) * 8; // count of bits
    shift %= length;
    uint64_t r_shifted = n >> shift;
    uint64_t l_shifted = n << (length - shift);

    return r_shifted | l_shifted;

}

// Printing functions
void print_error_message(char * message) {
    fprintf(stderr, "%s\n", message);
}

void print_binary(uint64_t n) {

    if (n == 0) {
        printf("# 0\n");
        return;
    }

    printf("# ");
    uint64_t mask = 1;
    bool found_one = false;
    char digit;

    // bit is the highest bit of our number
    for (uint64_t bit = mask << (sizeof(uint64_t) * 8 - 1); bit > 0; bit /= 2) {

        if (n & bit) {
            digit = '1';
        } else {
            digit = '0';
        }

        if (digit == '1') {
            found_one = true;
        }

        if (!found_one) {
            continue;
        }

        printf("%c", digit);
    }

    putchar('\n');

}

void raise_error(char * message) {
    print_error_message(message);
    exit(EXIT_FAILURE);
}

// Number loading functions
uint64_t load_binary(void) {

    int ch = getchar();
    int digit;
    uint64_t res = 0;
    uint64_t prev_res = 0;

    while ((ch == '0' || ch == '1' || isspace(ch) || ch == '\n')) {

        if (isspace(ch) || ch == '\n') {
            ch = getchar();
            continue;
        }

        digit = ch - '0';
        res = res << 1;

        if (res < prev_res) {
            raise_error("Out of range");
        }

        res = res | digit;

        prev_res = res;
        ch = getchar();
    }

    if (ungetc(ch, stdin) != ch){
        exit(1);
    }

    return res;

}

uint64_t load_hexadecimal(void) {

    int ch = getchar();
    uint64_t res = 0;
    uint64_t prev_res = 0;
    int digit;
    while (((ch >= '0' && ch <= '9') ||
            (toupper(ch) >= 'A' && toupper(ch) <= 'F') ||
            isspace(ch) || ch == '\n')) {

        if (isspace(ch) || ch == '\n') {
            ch = getchar();
            continue;
        }

        if (isdigit(ch)) {
            digit = (ch - '0');
        } else {
            digit = (toupper(ch) - 'A') + 10;
        }

        // adding new digit
        res = res << 4;

        if (res < prev_res) {
            raise_error("Out of range");
        }

        res = res | digit;

        prev_res = res;
        ch = getchar();
    }

    if (ungetc(ch, stdin) != ch){
        exit(1);
    }
    return res;
}

uint64_t load_octal(void) {

    int ch = getchar();
    uint64_t res = 0;
    uint64_t prev_res = 0;
    int digit;

    while (((ch >= '0' && ch <= '7') || isspace(ch) || ch == '\n')) {

        if (isspace(ch) || ch == '\n') {
            ch = getchar();
            continue;
        }

        digit = ch - '0';
        res = res << 3;

        if (res < prev_res) {
            raise_error("Out of range");
        }

        res = res | digit;

        prev_res = res;
        ch = getchar();
    }

    if (ungetc(ch, stdin) != ch){
        exit(1);
    }
    return res;
}

uint64_t load_decimal(int ch) {

    uint64_t res = 0;
    uint64_t prev_res = 0;

    while (((ch >= '0' && ch <= '9') || isspace(ch) || ch == '\n')) {

        if (isspace(ch) || ch == '\n') {
            ch = getchar();
            continue;
        }

        res *= 10;
        res += (ch - '0');

        if (res < prev_res) {
            raise_error("Out of range");
        }

        prev_res = res;
        ch = getchar();
    }

    if (ungetc(ch, stdin) != ch){
        exit(1);
    }
    return res;
}

uint64_t load_number(uint64_t memory,
                               uint64_t accum) {

    uint64_t res = 0;
    int ch = getchar();

    // ignoring whitespace
    while (isspace(ch) || ch == '\n') {
        ch = getchar();
    }

    // memory value
    if (ch == 'm') {
        return memory;
    }

    // convert system
    bool converted = false;
    switch (ch) {
        case 'T':
            print_binary(accum);
            res = load_binary();
            converted = true;
            break;
        case 'O':
            printf("# %lo\n", accum);
            res = load_octal();
            converted = true;
            break;
        case 'X':
            printf("# %lX\n", accum);
            res = load_hexadecimal();
            converted = true;
            break;
        default:
            break;
    }

    if (isdigit(ch) && !converted) {
        res = load_decimal(ch);
        converted = true;
    }

    if (!converted) {
        raise_error("Syntax error");
    }

    return res;
}

uint64_t execute_command(int command, uint64_t accum, uint64_t memory) {

    uint64_t new;
    switch (command) {
        case 'P':
            new = load_number(memory, accum);
            accum = new;
            break;
        case '=':
            break;
        case 'N':
            accum = 0;
            break;
        case '+':
            new = load_number(memory, accum);

            if (accum + new < accum) { // overflow
                raise_error("Out of range");
            }

            accum += new;
            break;
        case '-':
            new = load_number(memory, accum);

            if (accum - new > accum) { // overflow
                raise_error("Out of range");
            }
            accum -= new;
            break;

        case '/':
            new = load_number(memory, accum);

            if (new == 0) {
                raise_error("Division by zero");
            }

            accum /= new;
            break;

        case '*':
            new = load_number(memory, accum);

            if (accum * new < accum) { // overflow
                raise_error("Out of range");
            }
            accum *= new;
            break;
        case '%':
            new = load_number(memory, accum);

            if (new == 0) {
                raise_error("Division by zero");
            }

            accum %= new;
            break;
        case '<':
            new = load_number(memory, accum);
            accum = accum << new;
            break;
        case '>':
            new = load_number(memory, accum);

            accum = accum >> new;
            break;
        case 'l':
            new = load_number(memory, accum);

            accum = rotate_left(accum, new);
            break;
        case 'r':
            new = load_number(memory, accum);

            accum = rotate_right(accum, new);
            break;
        default:
            break;
    }

    return accum;
}

bool calculate(void) {

    // Accum is 64bit non-negative integer
    uint64_t memory = 0;
    uint64_t accum = 0;
    int ch = getchar();

    while (ch != EOF) {

        if (isspace(ch) || ch == '\n') {
            ch = getchar();
            continue;
        }
        switch (ch) {
            case 'M':

                if (memory + accum < memory) {
                    raise_error("Out of range");
                }

                memory += accum;
                break;
            case 'R':
                memory = 0;
                break;
            case 'T':
                print_binary(accum);
                break;
            case 'O':
                printf("# %lo\n", accum);
                break;
            case 'X':
                printf("# %lX\n", accum);
                break;
            case ';':

                // Print comment
                while (ch != '\n' && ch != EOF) {
                    ch = getchar();
                }
                break;

                // Valid operations to be executed
            case 'P':
            case '+':
            case '-':
            case '*':
            case '/':
            case '%':
            case '<':
            case '>':
            case 'N':
            case '=':
            case 'l':
            case 'r':
                accum = execute_command(ch, accum, memory);
                printf("# %lu\n", accum);
                break;
            default:
                raise_error("Syntax error");
        }

        ch = getchar();
    }

    return true;
}

int main(void) {
    // If program fails --> failure exit inside function
    calculate();
    return EXIT_SUCCESS;
}
