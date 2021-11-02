/* util.c
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Utility functions that can be linked with both kernel and user code
 */

#include "util.h"

static short *screen = (short *) 0xb8000;

// Write an integer to the screen
void print_int(int line, int col, int num) {
    int i, n, neg_flag;
    char buf[12];

    neg_flag = num < 0;
    if (neg_flag)
        num = ~num + 1;

    itoa(num, buf);

    n = strlen(buf);
    if (neg_flag)
        print_char(line, col++, '-');
    for (i = 0; i < n; i++)
        print_char(line, col++, buf[i]);
}

// Write an integer to the screen, base 16
void print_hex(int line, int col, uint32_t num) {
    int i, n;
    char buf[12];

    itohex(num, buf);

    n = strlen(buf);
    for (i = 0; i < n; i++)
        print_char(line, col + i, buf[i]);
}

// Write a string to the screen
void print_str(int line, int col, char *str) {
    int i, n;

    n = strlen(str);
    for (i = 0; i < n; i++)
        print_char(line, col + i, str[i]);
}

// Write a character on the screen
void print_char(int line, int col, char c) {
    // Text screen is 80x25 characters
    if ((line < 0) || (line > 24))
        return;
    if ((col < 0) || (col > 79))
        return;

    screen[line * 80 + col] = 0x07 << 8 | c;
}

// Read the character stored at location (x,y) on the screen
int peek_screen(int x, int y) {
    return screen[y * 80 + x] & 0xff;
}

// Clear the screen between (minx, miny) and (maxx, maxy)
void clear_screen(int minx, int miny, int maxx, int maxy) {
    int i, j, k;

    for (j = miny; j < maxy; j++) {
        for (i = minx; i < maxx; i++) {
            k = j * 80 + i;
            screen[k] = 0x0700;
        }
    }
}

// Scroll the screen
void scroll(int minx, int miny, int maxx, int maxy) {
    int i, j, k;

    for (j = miny; j < maxy; j++) {
        for (i = minx; i < maxx; i++) {
            k = j * 80 + i;
            if (j < maxy - 1)                // If not in first line
                screen[k] = screen[k + 80];  // Move character one line up
            else
                screen[k] = 0x0700;
        }
    }
}

// Read the time stamp counter
uint64_t get_timer(void) {
    uint64_t x = 0LL;

    // Load the time stamp counter into edx:eax (x)
    asm volatile ("rdtsc":"=A" (x));

    return x;
}

// Convert an ASCII string (like "234") to an integer
uint32_t atoi(char *s) {
    int n;
    for (n = 0; *s >= '0' && *s <= '9'; n = n * 10 + *s++ - '0');
    return n;
}

/* Functions from K&R - The C Programming Language */

// Convert an integer to an ASCII string (page 64)
void itoa(uint32_t n, char *s) {
    int i;

    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    s[i++] = 0;
    reverse(s);
}

// Convert an integer to an ASCII string, base 16
void itohex(uint32_t n, char *s) {
    int i, d;

    i = 0;
    do {
        d = n % 16;
        if (d < 10)
            s[i++] = d + '0';
        else
            s[i++] = d - 10 + 'a';
    } while ((n /= 16) > 0);
    s[i++] = 0;
    reverse(s);
}

// Reverse a string (page 62)
void reverse(char *s) {
    int c, i, j;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

// Get the length of a null-terminated string (page 99)
int strlen(char *s) {
    int n;
    for (n = 0; *s != '\0'; s++)
        n++;
    return n;
}

// Return TRUE if string s1 and string s2 are equal
int same_string(const char *s1, const char *s2) {
    while ((*s1 != 0) && (*s2 != 0)) {
        if (*s1 != *s2)
            return FALSE;
        s1++;
        s2++;
    }
    return (*s1 == *s2);
}

// Copy size bytes from source to destination
// The arrays can overlap
void bcopy(char *source, char *destin, int size) {
    int i;

    if (size == 0)
        return;

    if (source < destin) {
        for (i = size - 1; i >= 0; i--)
            destin[i] = source[i];
    } else {
        for (i = 0; i < size; i++)
            destin[i] = source[i];
    }
}

// Zero out size bytes starting at area
void bzero(char *area, int size) {
    int i;

    for (i = 0; i < size; i++)
        area[i] = 0;
}

// Read byte from I/O address space
uint8_t inb(int port) {
    int ret;

    asm volatile ("xorl %eax,%eax");
    asm volatile ("inb %%dx,%%al":"=a" (ret):"d"(port));

    return ret;
}

// Write byte to I/O address space
void outb(int port, uint8_t data) {
    asm volatile ("outb %%al,%%dx"::"a" (data), "d"(port));
}

// Write word to I/O address space
void outw(int port, uint16_t data) {
    asm volatile ("outw %%ax,%%dx"::"a" (data), "d"(port));
}


// Write long to I/O address space
void outl(int port, uint32_t data) {
    asm volatile ("outl %%eax,%%dx"::"a" (data), "d"(port));
}

static uint32_t rand_val;

void srand(uint32_t seed) {
    rand_val = seed;
}

// Return a pseudo random number
uint32_t rand(void) {
  rand_val = rand_step(rand_val);
  return rand_val;
}

// Perform a single step of random number generation
// Does not rely on global state
uint32_t rand_step(uint32_t seed) {
  return (1103515245 * seed + 12345) >> 16;
}
