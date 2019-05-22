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
    struct node **p;
    p = malloc(sizeof(struct node *));
    while((*head)->next != NULL){
        *p = *head;
        while((*p)->next != NULL)  *p = (*p) -> next;
        run((*p)->task, (*p)->task->burst);
        delete(head, (*p)->task);
    }
    run((*head)->task, (*head)->task->burst);

    free(p);
}
