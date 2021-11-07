/* processes.c
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Processes included in the ramdisk filesystem
 */

#include "common.h"
#include "syslib.h"
#include "util.h"
#include "printf.h"

void init(void) {
    ASSERT( spawn("RobinHood") >= 0 );
    ASSERT( spawn("LittleJohn") >= 0 );
    ASSERT( spawn("Sheriff") >= 0 );
    exit();
}

void RobinHood(void) {
    mbox_t pub = mbox_open("Robin-Hood-Publish-PID");
    pid_t myPid = getpid();

    // Send PID twice, once for LittleJohn, and once for the Sheriff
    mbox_send(pub, &myPid, sizeof(pid_t));
    mbox_send(pub, &myPid, sizeof(pid_t));

    // Find Little John's PID
    mbox_t sub = mbox_open("Little-John-Publish-PID");

    for (;;) {
        pid_t john;
        mbox_recv(sub, &john, sizeof(pid_t));
        printf(1,1, "Robin Hood(%d): Rob from the rich                   ", myPid);
        wait(john);
        printf(1,1, "Robin Hood(%d): I'm coming to save you, Little John!", myPid);
        sleep(1000);
        spawn("LittleJohn");
        mbox_send(pub, &myPid, sizeof(pid_t));
    }
}

void LittleJohn(void) {
    mbox_t pub = mbox_open("Little-John-Publish-PID");
    pid_t myPid = getpid();

    // Send PID twice, once for Robin Hood, and once for the Sheriff
    mbox_send(pub, &myPid, sizeof(pid_t));
    mbox_send(pub, &myPid, sizeof(pid_t));

    // Find Robin's PID
    mbox_t sub = mbox_open("Robin-Hood-Publish-PID");

    for (;;) {
        pid_t aramis;
        mbox_recv(sub, &aramis, sizeof(pid_t));
        printf(2,1, "Little John(%d): and give to the poor!         ", myPid);
        wait(aramis);
        printf(2,1, "Little John(%d): I'm coming to save you, Robin!", myPid);
        sleep(1000);
        spawn("RobinHood");
        mbox_send(pub, &myPid, sizeof(pid_t));
    }
}

void Sheriff(void) {
    uint32_t myRand = get_timer();
    pid_t myPid = getpid();

    mbox_t subRobin = mbox_open("Robin-Hood-Publish-PID");
    mbox_t subJohn = mbox_open("Little-John-Publish-PID");

    pid_t robin, john;

    mbox_recv(subRobin, &robin, sizeof(pid_t));
    mbox_recv(subJohn, &john, sizeof(pid_t));

    for (;;) {
        printf(10,1, "Sheriff of Knottingham(%d): I am plotting... muahaha ", myPid);
        sleep(5000);
        printf(10,1, "Sheriff of Knottingham(%d): I have a dastardly plan! ", myPid);
        myRand = rand_step(myRand);
        switch( myRand % 2 ) {
            case 0:
                printf(11, 1, 
                       "Sheriff of Knottingham(%d): I will kill Robin Hood(%d)!  ",
                       myPid, robin);
                sleep(1000);
                printf(1,1, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ");
                kill(robin);
                mbox_recv(subRobin, &robin, sizeof(pid_t));
                printf(12, 1,
                        "Sheriff of Knottingham(%d): Egads! Robin(%d) lives!                 ",
                        myPid, robin);
                break;
            case 1:
                printf(11, 1,
                        "Sheriff of Knottingham(%d): I will kill Little John(%d)! ", 
                        myPid, john);
                sleep(1000);
                printf(2,1, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ");
                kill(john);
                mbox_recv(subJohn, &john, sizeof(pid_t));
                printf(12, 1,
                        "Sheriff of Knottingham(%d): Blimey! Little John(%d) is alive again! ",
                        myPid, john);
                break;
        }
        sleep(2000);
        printf(11, 1, "                                                           ");
        printf(12, 1, "                                                                      ");
    }
}
