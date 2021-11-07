/* readme.txt
 * COS 318, Fall 2021: Project 4 IPC and Process Management
 * Authors: Jayson Badal and Austin Li
 */ 


In this project we did the following: implemented a system call, inter-process communication 
using message boxes, a handler for the keyboard interrupt, a kill system call, and a wait 
system call. 

The spawn() system call created a new running process. It looks up a process by name 
and returns the pid on success, -1 if unable to find a process, and -2 if there 
are too many processes. With respect to inter-process communication, we implemented 
an efficient implementation of the bounded buffer problem. Our message boxes supported
four operations, each of which has a corresponding system call.

Firstly, we have do_mbox_open(name). This creates an IPC queue named name. If a queue 
already existed under that name, then instead we returned the existing queue and 
incremented its usage count. Secondly, we have do_mbox_close(mbox). This function 
decreases the usage count of the specified message box. If no one was using that message 
box, it deallocates this message box. Thirdly, we have do_mbox_send(mbox,buf,size). This will add 
the message buf to the specified message box; size is the length of the message in bytes. If the 
message box is full, then the process blocks until it can enqueue the message. Fourthly, we have 
do_mbox_recv(mbox,buf,size). This will receive the next message from the message box. It copies the 
contents of the message into buf. It does not copy more than size bytes to avoid buffer overflows. 
Fifthly, we have do_mbox_is_full(mbox). This returns 1 if the message box is full, and 0 otherwise. 
For handling the keyboard, we wrote a handler for irq1.  This handler receives bytes from the keyboard,
and buffers them using a message box. If the keyboard buffer is full, the handler discards the 
character.

For killing processes, the kill() system call changes the state of a process to EXITED. In our pcb 
struct, we add a pointer that holds the current queue the respective pcb is on. We then modified our 
queue implementation to update this pointer whenever a node is enqueued onto a respective queue. 
In addition, we modified our queue implementation to add a queue remove function to remove a respective 
node from a queue. In addition, we added a queue called waiting_on_queue. This queue helps release all
processes that were waiting on this killed process to die. 

The kill system immediately kills a process, even if it is sleeping or blocked (even on await() call).
If a process is killed while sleeping, other sleeping processes still wake on schedule. If a process is
killed while blocked on a lock, semaphore, condition variable or barrier, the other processes which 
interact with that synchronization primitive are unaffected. If a process is killed while it is in the 
ready queue, lottery scheduling still gives proportional scheduling to the surviving processes.
If a process has opened message boxes, their usage counts are decremented when a process is killed. 
With respect to the wait function, we block the caller until a given process has terminated.
