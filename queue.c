/* queue.c
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Queue implementation
 */

#include "common.h"
#include "queue.h"

void queue_init(node_t * queue) {
    queue->prev = queue->next = queue;
}

node_t *queue_get(node_t * queue) {
    node_t *item;

    item = queue->next;
    if (item == queue) {
        // The queue is empty
        item = NULL;
    } else {
        // Remove item from the queue
        item->prev->next = item->next;
        item->next->prev = item->prev;
    }
    return item;
}

void queue_put(node_t * queue, node_t * item) {
    item->prev = queue->prev;
    item->next = queue;
    item->prev->next = item;
    item->next->prev = item;
}

int queue_empty(node_t *queue) {
    if( queue->next == queue )
        return 1;
    else
        return 0;
}

node_t *queue_first(node_t *queue) {
    if (queue->next == queue)
        return NULL;
    else
        return queue->next;
}


void queue_remove(node_t * q, pid_t pid) {
        // check if queue is empty
    

    if (!queue_empty(&sleep_queue)) {
        // only 1 item in queue

        // check first item 
        if ( (pcb_t*) q->pid == pid) {
            // only 1 item in queue
            if (q->next == q) {
                q = NULL;
            }
            else {
                
            }
        }


    if ( (pcb_t*) sleep_queue->pid == pid) {
        sleep_queue = NUL
    }
        

    }

    // only 1 item in queue
    if ( (pcb_t*) sleep_queue->pid == pid) {
        sleep_queue = NULL;
    }

    for (iter = q->next; iter && iter != q; iter=iter->next) {
        pcb_t* temp_pcb = (pcb_t*) iter;
        if (pid == temp_pcb->pid) {
            // remove item  
            iter->prev->next = iter->next;
            iter->next->prev = iter->prev;
        }
    }

}



void queue_put_sort(node_t *q, node_t *elt, node_lte lte) {
    node_t *iter;

    for (iter = q->next; iter && iter != q; iter=iter->next) {
        if (lte(elt, iter)) {
            // Put elt before iter
            queue_put(iter, elt);
            return;
        }
    }
    
    // Either the queue is empty, or elt is larger than all other elements
    // Put it at the end of the queue
    queue_put(q, elt);
}
