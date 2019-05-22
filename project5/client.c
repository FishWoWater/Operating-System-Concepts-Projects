/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "threadpool.h"

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    // create some work to do
    struct data work[4];

    // initialize the thread pool
    pool_init();

    /* ensure that all the threads are successfully created */
    sleep(0.5);
    // submit the work to the queue

    /* note that for mulitiple test cases
     * do not use the same variable work */
    for(int i=0;i<NUM_WORKS;i++){
        work[i].a = rand()%100;
        work[i].b = rand()%200;
        pool_submit(&add, &work[i]);
    }

    /* for a better-organized output */
    sleep(3);

    pool_shutdown();

    return 0;
}
