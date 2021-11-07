/* processes.c
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Programs included in the ramdisk filesystem
 */

#include "common.h"
#include "syslib.h"
#include "util.h"
#include "printf.h"

// The 'init' process is a shell that lets you spawn other programs
static void get_line(char *buffer, int maxlen);
void init_process(void) {
    for (;;) {
        printf(20, 1, "Type help for help...");
        printf(21, 1, "$                           ");

        char buffer[100];
        get_line(buffer, 100);

        pid_t result = spawn(buffer);
        if (result == -1)
            printf(19, 1, "*** Error: %s not found ***      ", buffer);
        else if (result == -2)
            printf(19, 1, "*** Error: process table full ***");
        else
            printf(19, 1, "Started process PID=%d", result);
    }
}

// The 'help' process prints help, then exits
void help_process(void) {
    printf(10, 1, "KludgeShell v 1.0");
    sleep(1000);
    printf(11, 1, "Type the name of a program to run");
    sleep(1000);
    printf(12, 1, "Available programs are:");
    printf(13, 1, "  help, count, producer, consumer, plane and shutdown");
    sleep(1000);
    printf(14, 1, "You can start more than one instance of each.");
    sleep(5000);
    exit();
}

// The 'count' process just counts seconds since it started
void count_process(void) {
    pid_t myPid = getpid();

    int myRow = myPid / 2;
    int myCol = 40* (myPid % 2);

    int i;
    for (i=0; ; ++i) {
        printf(myRow, myCol, "Count(%d): %d", myPid, i);
        sleep(1000);
    }
}

// The 'producer' process will put numbers into a message box
void producer_process(void) {
    pid_t myPid = getpid();

    int myRow = myPid / 2;
    int myCol = 40* (myPid % 2);

    mbox_t mbox = mbox_open("theMbox");

    int i;
    for (i=0; ; ++i) {
        printf(myRow, myCol, "Producer(%d): Sending %d   ", myPid, i);
        mbox_send(mbox, (void*)&i, sizeof(int));
        printf(myRow, myCol, "Producer(%d): Sent %d     ", myPid, i);
        sleep(1000);
    }
}

// The 'consumer' process will pull numbers from a message box
void consumer_process(void) {
    pid_t myPid = getpid();

    int myRow = myPid / 2;
    int myCol = 40* (myPid % 2);

    mbox_t mbox = mbox_open("theMbox");

    int i;
    for (i=0; ; ++i) {
        int number;
        mbox_recv(mbox, (void*)&number, sizeof(int));
        printf(myRow, myCol, "Consumer(%d): Recv %d   ", myPid, number);
        sleep(1000);
    }
}

// This process will do nothing but shutdown
void shutdown_process(void) {
  shutdown();
}

#define ROWS 4
#define COLUMNS 18
static void draw(int loc_x, int loc_y, int plane);
void airplane_process(void) {
    int loc_x = 80, loc_y = 10;

    while (1) {
        // Erase plane
        draw(loc_x, loc_y, FALSE);
        loc_x -= 1;
        if (loc_x < -20) {
            loc_x = 80;
        }
        // Draw plane
        draw(loc_x, loc_y, TRUE);
        sleep(100);
    }
}

static void get_line(char *buffer, int maxlen) {
    int offset;
    for (offset=0; offset<maxlen; ) {
        char c = get_char();
        if (c == '\n' || c == '\r')
            break;
        else if (c == '\b') {
            if( offset > 0 ) {
                offset --;
                printf(21, 1 + 2 + offset, " ");
            }
            continue;
        } else {
            printf(21, 1 + 2 + offset, "%c", c);
            buffer[offset++] = c;
        }
    }
    if (offset >= maxlen)
        offset = maxlen - 1;
    buffer[offset] = '\0';
}

static char picture[ROWS][COLUMNS + 1] = {
    "     ___       _  ",
    " | __\\_\\_o____/_| ",
    " <[___\\_\\_-----<  ",
    " |  o'            "
};

static void draw(int loc_x, int loc_y, int plane) {
    int i, j;

    for (i = 0; i < COLUMNS; i++) {
        for (j = 0; j < ROWS; j++) {
            if (plane == TRUE) {
                print_char(loc_y + j, loc_x + i, picture[j][i]);
            } else {
                print_char(loc_y + j, loc_x + i, ' ');
            }
        }
    }
}
