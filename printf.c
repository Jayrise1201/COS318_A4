/* printf.c
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Printing functions for debugging
 */

#include "printf.h"

static unsigned short (*const screen)[80] = (unsigned short (*)[80]) 0xb8000;

static void printf_c(int *line, int *col, char c) {
    if (*col < 0)
        *col = 0;
    while (*col >= 80) {
        ++*line;
        *col -= 80;
    }
    if (*line < 0)
        *line = 0;
    *line %= 25;
    switch (c) {
    case '\t':
        do {
            printf_c(line, col, ' ');
        } while (*col % 8 != 0);
        break;
    case '\n':
        ++*line;
        // Fall through
    case '\r':
        *col = 0;
        break;
    default:
        screen[*line][*col] = c | (unsigned short) 0xf00;
        ++*col;
    }
}

static void printf_u(int *line, int *col, unsigned u) {
    char buffer[16];
    char *p;

    p = buffer;
    *p = '\0';
    do {
        *++p = '0' + u % 10;
        u /= 10;
    } while (0 != u);
    for (; '\0' != *p; --p) {
        printf_c(line, col, *p);
    }
}

static void printf_d(int *line, int *col, int d) {
    if (d < 0) {
        printf_c(line, col, '-');
        printf_u(line, col, -d);
    } else {
        printf_u(line, col, d);
    }
}

static void printf_s(int *line, int *col, char *s) {
    for (; '\0' != *s; ++s) {
        printf_c(line, col, *s);
    }
}

static void printf_x(int *line, int *col, unsigned x) {
    int i;
    char c;

    for (i = 7; i >= 0; --i) {
        c = (x >> (i << 2)) & 0xf;
        if (c >= 0xa)
            c += 'a' - 0xa;
        else
            c += '0';
        printf_c(line, col, c);
    }
}

// To bypass the aliasing rules in the newer version of gcc
// http://blog.worldofcoding.com/2010/02/solving-gcc-44-strict-aliasing-problems.html
typedef unsigned __attribute__((__may_alias__)) alias_unsigned;

void printf(int line, int col, char *format, ...) {
    alias_unsigned *argument = 1 + (alias_unsigned *)(void *) &format;
    char *p;

    for (p = format; '\0' != *p; ++p) {
        if ('%' == *p) {
            ++p;
            switch (*p) {
            case '\0':
                --p;
                break;
            case 'c':
                printf_c(&line, &col, *argument++);
                break;
            case 'd':
                printf_d(&line, &col, *argument++);
                break;
            case 's':
                printf_s(&line, &col, (char *) *argument++);
                break;
            case 'u':
                printf_u(&line, &col, *argument++);
                break;
            case 'x':
                printf_x(&line, &col, *argument++);
                break;
            default:
                printf_c(&line, &col, *p);
                break;
            }
        } else {
            printf_c(&line, &col, *p);
        }
    }
}
