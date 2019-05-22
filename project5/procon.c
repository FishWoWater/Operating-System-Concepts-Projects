/*************************************************************************
	> File Name: producer_consumer.c
	> Author: Junjie Wang
	> Mail: dreamboy.gns@sjtu.edu.cn
  > Website:http://www.dbgns.com
  > Blog:http://www.dbgns.com/blog
	> Created Time: 2019年05月18日 星期六 16时20分04秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 10
/* do not create too much producers or consumers */
#define LIMIT 5
/* type for the items in buffer */
typedef int bufferItem;
/* pointer/index for the position of the consumer */
int con_index = 0;
int pro_index = 0;
/* buffer */
bufferItem buffer[BUFFER_SIZE];
/* semaphore and mutex */
sem_t empty, full;
pthread_mutex_t mutex;

void usage();

void insertItem(bufferItem item){
    /* empty == 0 => the buffer is full */
    sem_wait(&empty); 
    /* use mutex, since multiple producers may 
     * want to access at the same time */
    pthread_mutex_lock(&mutex);
    buffer[pro_index] = item;
    pro_index = (pro_index + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
}

bufferItem removeItem(){
    /* full == 0 => the buffer is empty */
    bufferItem item;
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    item = buffer[con_index];
    con_index = (con_index + 1) % BUFFER_SIZE; 
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    return item;
}

void *consumer(void *param){
    /* sleep for random time
     * remove an item from the buffer */
    int sleep_time;
    int id = *(int *)param;
    bufferItem item;
    while(1){
    /* avoid endless sleeping */
        sleep_time = rand() % 10;
        sleep(sleep_time);
        item = removeItem();
        printf("Consumer %d removes item %d.\n", id, item);
    }
}

void *producer(void *param){
    /* sleep for random time
     * insert an item into the buffer */
    int sleep_time;
    int id = *(int *)param;
    bufferItem item;
    while(1){
        /* avoid endless sleeping */
        sleep_time = rand() % 10;
        sleep(sleep_time);
        /* please do not be too large */
        item = rand() % 1000;
        insertItem(item);
        printf("Producer %d inserts item %d.\n", id, item);
    }
}

int main(int argc, char *argv[]){
    char c;
    int num_cons = 0, num_pros = 0;
    int sleep_time = 0;
    int i;
    /* necessary initialization 
     * note the definition of the empty and full semaphore */
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    /* pid and attributes */
    pthread_t con[LIMIT];   pthread_attr_t con_attr[LIMIT];
    pthread_t pro[LIMIT];   pthread_attr_t pro_attr[LIMIT];

    /* parse the inputs */
    while((c=getopt(argc, argv, "p:c:t:")) != -1){
        switch(c){
            case 'p':
                num_pros = atoi(optarg);
                break;
            case 'c':
                num_cons = atoi(optarg);
                break;
            case 't':
                sleep_time = atoi(optarg);
                break;
            default:
                fprintf(stderr, "unrecognized options!\n");
                usage();
                exit(-1);
        }
    }

    if(num_pros == 0 || num_pros == 0){
        fprintf(stderr, "Invalid arguments!\n");
        usage();
        exit(-1);
    }

    if(num_pros > LIMIT || num_cons > LIMIT){
        fprintf(stderr, "Too much consumers or producers!\n");
        usage();
        exit(-1);
    }
    /* create the consumers */
    for(i=0;i<num_cons;i++){
        pthread_attr_init(&con_attr[i]);
        pthread_create(&con[i], &con_attr[i], consumer, (void *)&i);
    }
    /* create the producers */
    for(i=0;i<num_pros;i++){
        pthread_attr_init(&pro_attr[i]);
        pthread_create(&pro[i], &pro_attr[i], producer, (void *)&i);
    }
    /* sleep before termination */
    sleep(sleep_time);
    /* do some necessary cleaning up */
    sem_destroy(&full);
    sem_destroy(&empty);
    pthread_mutex_destroy(&mutex);

    return 0;
}

void usage(){
    printf("Options:\n");
    printf("    -p          the number of the producer threads(don't make p larger than 5)\n");
    printf("    -c          the number of the consumer threads(don't make c larger than 5)\n");
    printf("    -t          the time(secs) to sleep before termination\n");
}
