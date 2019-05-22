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
    /* implement the round robin schedule algorithm 
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
    
    traverse(*headNew);
    *p = *headNew;
    /* now based on the headNew we implement the RR algorithm */
    while(*headNew != NULL){
        task = (*p)->task;
        if(task->burst <= RR_TIME){
            /* for this job we can finish */
            run(task, task->burst);
            if((*p)->next == NULL)  *p = *headNew;
            else *p = (*p) -> next;
            delete(headNew, task);
        }else{
            /* can not finish this task*/
            run(task, RR_TIME);
            (*p)->task->burst -= RR_TIME;
            if((*p)->next == NULL)  *p = *headNew;
            else *p = (*p) -> next;
        }
    }
}
