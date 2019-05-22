/*************************************************************************
	> File Name: threadpool.c
	> Author: Junjie Wang
	> Mail: dreamboy.gns@sjtu.edu.cn
  > Website:http://www.dbgns.com
  > Blog:http://www.dbgns.com/blog
	> Created Time: 2019年05月18日 星期六 14时14分38秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "threadpool.h"

Task task;

struct node *head, *tmp, *newNode;
sem_t sem;
pthread_mutex_t mutex;
pthread_t pid[MAX_THREADS];
int res, is_shutdown, num_tasks;

void enqueue(Task task){
    newNode = malloc(sizeof(struct node));
    newNode -> task = task;
    newNode -> next = NULL;
    if(head == NULL) {
        head = newNode; 
    }else{
        tmp = head;
        while(tmp->next != NULL)    tmp = tmp->next;
        tmp -> next = newNode;
    }
}

Task dequeue(){
    task = head -> task;
    tmp = head;
    head = head -> next;
    free(tmp);
    return task;
}

int pool_submit(void (*somefunction)(void *p), void *p){
    /* submit the task to the pool */
    task.function = somefunction;
    task.data = p;
    /* lock the mutex, avoid multi access to the queue */
    pthread_mutex_lock(&mutex);
    enqueue(task);
    num_tasks++;
    printf("sucessfully submit the task!\n");
    sem_post(&sem);
    pthread_mutex_unlock(&mutex);

    return 0;
}

void *worker(void *param){
    while(1){
        while(num_tasks == 0 && !is_shutdown){
            printf("waiting for the task...\n");
            sem_wait(&sem);
        }
        if(is_shutdown){
            printf("The pool has been shut down => threads all exiting...\n");
            pthread_exit(0);
        }
        /* also lock the mutex */
        pthread_mutex_lock(&mutex);
        task = dequeue();
        pthread_mutex_unlock(&mutex);
        num_tasks--;
        execute(task.function, task.data);
    }
}

void execute(void (*somefunction)(void *p), void *p){
    printf("executing...\n");
    (*somefunction)(p);
    printf("finish execution...\n");
} 

void pool_init(void){
    num_tasks = 0;
    is_shutdown = 0;
    head = NULL;
    pthread_mutex_init(&mutex, NULL);
    /* initialize it to 0 */
    sem_init(&sem, 0, 0);
    for(int i=0;i<MAX_THREADS;i++)
        pthread_create(&pid[i], NULL, worker, NULL);
}

void pool_shutdown(void){
    /* destroy the link list */
    while(head !=  NULL){
        tmp = head;
        head = head->next;
        free(tmp);
    }
    printf("shuting down...\n");
    is_shutdown = 1;
    /* destroy all the threads */
    for(int i=0;i<MAX_THREADS;i++)
        sem_post(&sem);
    for(int i=0;i<MAX_THREADS;i++)
        pthread_join(pid[i], NULL);
    pthread_mutex_destroy(&mutex);
    /* destroy the semaphore */
    sem_destroy(&sem);
}
