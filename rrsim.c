#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

#define MAX_BUFFER_LEN 80

typedef enum {IDLE, DISPATCHING, BUSY} cpu_status;

taskval_t *event_list = NULL;

void print_task(taskval_t *t, void *arg) {
    printf("task %03d: %5d %3.2f %3.2f\n",
        t->id,
        t->arrival_time,
        t->cpu_request,
        t->cpu_used
    );  
}


void increment_count(taskval_t *t, void *arg) {
    int *ip;
    ip = (int *)arg;
    (*ip)++;
}

/*
 * Copy information from src into dst
 * used in swap_data
 */
void copy_task(taskval_t *dst, taskval_t *src)
{
    dst->id             = src->id;
    dst->arrival_time   = src->arrival_time;
    dst->finish_time    = src->finish_time;
    dst->cpu_request    = src->cpu_request;
    dst->cpu_used       = src->cpu_used;
}

/*
 * Swap the data in a with that in b
 * used in sort_list
 */
void swap_data(taskval_t *a, taskval_t *b)
{
    taskval_t * tmp = new_task();
    copy_task(tmp, a);
    copy_task(a, b);
    copy_task(b, tmp);
}

/*
 * Sorts list by arrival time
 */
void sort_list(taskval_t *event_list)
{
    if(event_list == NULL || event_list->next == NULL)
        return;

    taskval_t *node1 = event_list;
    taskval_t *node2;

    while(node1 != NULL)
    {
        node2 = node1->next;
        while(node2 != NULL)
        {
            if(node1->arrival_time > node2->arrival_time)
            {
                swap_data(node1, node2);
            }
            node2 = node2->next;
        }
        node1 = node1->next;
    }
}

/*
 * Runs a round robin scheduling simulation on tasks found in event_list
 * Begins by sorting event_list to minimize the amount of times we need to look at it
 * CPU has 3 states: IDLE, DISPATCHING, BUSY
 * If CPU is IDLE and tasks on ready_q, dispatch task at front
 * If quantum is up and running task not complete, put task at back of ready_q and dispatch next task
 */
void run_simulation(int qlen, int dlen) {
    taskval_t *ready_q      = NULL;

    int tick    = 0;
    int dtick   = 0;
    int qtick   = 0;

    int cpu_state = IDLE;

    /*
     * Sort event list by arrival time
     */
    sort_list(event_list);

    taskval_t *running = NULL;

    for(;;tick++)
    {
        /* 
         * Pull arrived tasks onto ready q
         */
        while(event_list != NULL && event_list->arrival_time <= tick)
        {
            taskval_t * tmp = event_list;
            event_list = remove_front(event_list);
            tmp->next = NULL;
            ready_q = add_end(ready_q, tmp);
        }

        /*
         * Running task has finished within last tick
         */
        if(running && running->cpu_used >= running->cpu_request)
        {
            float waitt = tick - running->arrival_time - running->cpu_request;
            int tat = tick - running->arrival_time;

            printf("[%05d] id=%05d EXIT w=%0.2f ta=%d.00\n", tick, running->id, waitt, tat);

            end_task(running);
            running = NULL;
        }

        /*
         * No more tasks
         */
        if(!running && !ready_q && !event_list)
        {
            exit(0);
        }
        
        if(ready_q != NULL && cpu_state == IDLE)
            cpu_state = DISPATCHING;

        printf("[%05d] ", tick);

        switch(cpu_state)
        {
            case IDLE:
                printf("IDLE\n");
                if (ready_q != NULL)
                    cpu_state = DISPATCHING;
                break;
            case DISPATCHING:
                printf("DISPATCHING\n");
                if(dlen == 0)
                    tick--;
                dtick++;
                if(dtick == 1)
                {
                    running = ready_q;
                    ready_q = remove_front(ready_q);
                }
                if(dtick >= dlen)
                {
                    cpu_state = BUSY;
                    dtick = 0;
                }
                break;
            case BUSY:
                printf("id=%05d req=%.2f used=%.2f\n", running->id, running->cpu_request, running->cpu_used);
                running->cpu_used += 1.0;
                qtick++;
                if(qtick == qlen || running->cpu_used >= running->cpu_request)
                {
                    if(running->cpu_used < running->cpu_request)
                        ready_q = add_end(ready_q, running);

                    if(ready_q == NULL)
                        cpu_state = IDLE;
                    else
                        cpu_state = DISPATCHING;
                    
                    qtick = 0;
                }
                break;
        }
    }
}


int main(int argc, char *argv[]) {
    char   input_line[MAX_BUFFER_LEN];
    int    i;
    int    task_num;
    int    task_arrival;
    float  task_cpu;
    int    quantum_length = -1;
    int    dispatch_length = -1;

    taskval_t *temp_task;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--quantum") == 0 && i+1 < argc) {
            quantum_length = atoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "--dispatch") == 0 && i+1 < argc) {
            dispatch_length = atoi(argv[i+1]);
        }
    }

    if (quantum_length == -1 || dispatch_length == -1) {
        fprintf(stderr, 
            "usage: %s --quantum <num> --dispatch <num>\n",
            argv[0]);
        exit(1);
    }


    while(fgets(input_line, MAX_BUFFER_LEN, stdin)) {
        sscanf(input_line, "%d %d %f", &task_num, &task_arrival,
            &task_cpu);
        temp_task = new_task();
        temp_task->id = task_num;
        temp_task->arrival_time = task_arrival;
        temp_task->cpu_request = task_cpu;
        temp_task->cpu_used = 0.0;
        event_list = add_end(event_list, temp_task);
    }

#ifdef DEBUG
    int num_events = 0;
    apply(event_list, increment_count, &num_events);
    printf("DEBUG: # of events read into list -- %d\n", num_events);
    printf("DEBUG: value of quantum length -- %d\n", quantum_length);
    printf("DEBUG: value of dispatch length -- %d\n", dispatch_length);
#endif

    run_simulation(quantum_length, dispatch_length);

    return (0);
}
