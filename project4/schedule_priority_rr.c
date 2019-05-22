/*************************************************************************
	> File Name: schedule_fcfs.c
	> Author: Junjie Wang
	> Mail: dreamboy.gns@sjtu.edu.cn
  > Website:http://www.dbgns.com
  > Blog:http://www.dbgns.com/blog
	> Created Time: 2019年05月16日 星期四 21时52分30秒
 ************************************************************************/

/* implement the first in first out algorithm */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

#define RR_TIME 10
extern struct node **head;

void add(char *name, int priority, int burst){
    Task *task; 
    task = malloc(sizeof(Task));
    task->name = malloc(sizeof(char)*20);
    strcpy(task->name, name);
    task->priority = priority;
    task->burst = burst;
    insert(head, task);
}

void schedule(){
    /* implement the priority round robin schedule algorithm 
     * first reverse the list */
    struct node **headNew = malloc(sizeof(struct node *));
    /* must maually set it to NULL */
    *headNew = NULL;

    /* a very strange thing
     * when using the headNew which was malloc in main, *headNew = NULL
     * but when using the headNew here, *headNew != NULL */
    Task *task = malloc(sizeof(Task));
    struct node *newNode;
    struct node **p = malloc(sizeof(struct node *));
    while((*head)->next!=NULL){
        task = (*head) -> task;
        newNode = malloc(sizeof(struct node));
        newNode -> task = task;
        newNode -> next = *headNew;
        *headNew = newNode;
        *head = (*head)-> next;
    }
    task = (*head) -> task;
    newNode = malloc(sizeof(struct node));
    newNode -> task = task;
    newNode -> next = *headNew;
    *headNew = newNode;
    
    /* implement the priority RR based on the headNew 
     * first use an array to store the priority counts */
    *p = *headNew;
    int counts[MAX_PRIORITY+1], i;
    for(i=0;i<=MAX_PRIORITY;i++) counts[i] = 0;
    while((*p)->next != NULL){
        task = (*p) -> task;
        counts[task->priority]++;
        *p = (*p) -> next;
    }
        
    i = MAX_PRIORITY;
    while(1){
        for(;counts[i] == 0 && i>0; i--)   ;
        if(i == 0)  break;

        if(counts[i] == 1){
            /* with only one task, we just execute it */
            *p = *headNew;
            while((*p)->next != NULL){
                if((*p) -> task -> priority == i){
                    run((*p)->task, (*p)->task->burst);
                    delete(headNew, (*p)->task);
                    break;
                }
                *p = (*p) -> next;
            }
            counts[i] = 0;
        }else{
            /* with more than one task, we use the round robin */
            *p = *headNew;
            while(counts[i]>0){
                if((*p)->task->priority == i){
                    task = (*p) -> task;
                    if(task->burst <= RR_TIME){
                        /* with time less than the limit */
                        run(task, task->burst);
                        /* move on */
                        delete(headNew, task);
                        counts[i]--;
                    }else{
                        /* with time more than the Q */
                        run(task, RR_TIME);
                        task->burst -= RR_TIME;
                    }
                }
                if((*p)->next == NULL)  *p = *headNew;
                else *p = (*p) -> next;
            }
        }
    }
    free(newNode);
    free(headNew);
    free(p);
    free(task);
}
