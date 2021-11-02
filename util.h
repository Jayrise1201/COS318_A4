/* util.h
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Definitions and types for utility functions
 */

#ifndef UTIL_H
#define UTIL_H

#include "common.h"

void clear_screen(int minx, int miny, int maxx, int maxy);
void scroll(int minx, int miny, int maxx, int maxy);
int peek_screen(int x, int y);

uint64_t get_timer(void);

uint32_t atoi(char *s);
void itoa(uint32_t n, char *s);
void itohex(uint32_t n, char *s);

void print_char(int line, int col, char c);
void print_int(int line, int col, int num);
void print_hex(int line, int col, uint32_t num);
void print_str(int line, int col, char *str);

void reverse(char *s);
int strlen(char *s);
int same_string(const char *s1, const char *s2);

void bcopy(char *source, char *destin, int size);
void bzero(char *a, int size);

uint8_t inb(int port);
void outb(int port, uint8_t data);
void outw(int port, uint16_t data);
void outl(int port, uint32_t data);

void srand(uint32_t seed);
uint32_t rand_step(uint32_t seed);
uint32_t rand(void);

#endif
