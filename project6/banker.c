/*************************************************************************
	> File Name: banker.c
	> Author: Junjie Wang
	> Mail: dreamboy.gns@sjtu.edu.cn
  > Website:http://www.dbgns.com
  > Blog:http://www.dbgns.com/blog
	> Created Time: 2019年05月18日 星期六 18时39分57秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

/* the file which contains the
 * maximum need of each customer */
#define INPUT_FILEPATH "./input.txt"
#define MAX_LINE_LENGTH 20

/* we limit the maximum size 
 * of the customers and resources.
 * Acutually in the implementation 
 * by pointers, they are not needed */

#define MAX_CUSTOMERS 10
#define MAX_RESOURCES 5

/* martices will be used */
int **maximum;
int **need;
int *available;
int **allocation;

/* temporary pointer which will be 
 * used when checing the safe state */
int **_maximum;
int **_need;
int *_available;
int **_allocation;

/* the number of customer and resources */
int num_customers = 0;
int num_resources = 0;

void initArrays();
char *preprocess(char *buffer);
void getResources(int argc, char *argv[]);
void printArrays();
void handleReleases(char *buffer);
int handleRequest(char *buffer);
int releaseResources(int customerIndex, int *strategy);
void allocateResources(int customerIndex, int *strategy);
int checkSafeWrapper();
int checkSafe(int *finished);
int findSomeCustomer(int *finished);
int *parseLine(char *buffer);
void destroyArrays();

/* implement the banker's algorithm */
int main(int argc, char *argv[]){
    char *buffer, *find;
    /* we will use the buffer to store the input from stdin */
    buffer = malloc(sizeof(char) * MAX_LINE_LENGTH);
    initArrays();
    /* get the resources of each type */
    getResources(argc, argv);
    if(!checkSafeWrapper()) printf("Warning: current state is unsafe.\n");
    while(1){
        printf("banker>");
        fgets(buffer, MAX_LINE_LENGTH, stdin);
        buffer = preprocess(buffer);
        if(strcmp(buffer, "exit\n") == 0){
            printf("Bye.\n");
            break;
        }else if(buffer[0] == '*'){
            /* output all the arrays */
            printArrays();
        }else if(strncmp(buffer, "RL", 2) == 0){
            /* release the resources */
            handleReleases(buffer);
        }else if(strncmp(buffer, "RQ", 2) == 0){
            /* request the resources */
            handleRequest(buffer);
        }else{
            /* unrecognized input */
            fprintf(stderr, "unrecognized input!\n");
        }
    }
    destroyArrays();
    return 0;
}

/* get the resources of each type */
void getResources(int argc, char *argv[]){
    if(argc != num_resources + 1){
        fprintf(stderr, "Error! Your input does not match the number of resources!\n");
        exit(-1);
    }
    for(int i=0;i<num_resources;i++)    available[i] = atoi(argv[i+1]);
    printf("Get resources successfully!\n");
}

/* release the resources */
void handleReleases(char *buffer){
    /* buffer + 3 means striping the "RL " */ 
    int *arr = parseLine(buffer+3); 
    int customerIndex = arr[0];
    if(releaseResources(customerIndex, arr+1)) printf("Successfully released!\n");
    else printf("Sorry... But you don't have that much resources...\n");
}

/* handle the request from the banker 
 * call the checkSafe to judge whether 
 * this request can be satisfied */
int handleRequest(char *buffer){
    int flag = 1; 
    int *arr = parseLine(buffer+3);
    int customerIndex = arr[0];

    allocateResources(customerIndex, arr+1);

    if(!checkSafeWrapper()){
        /* if after the allocation, the state 
         * is not safe, we roll back */
        flag = 0;
        releaseResources(customerIndex, arr+1);
    }
    if(flag)    printf("Congratulations! You request has been satisfied.\n");
    else    printf("Sorry, banker... But your request may lead to unsafe state, so we deny your request\n");
    return flag;
}

/* allocate the resources to a specific customer */
void allocateResources(int customerIndex, int *strategy){
    for(int i=0;i<num_resources;i++){
        if(maximum[customerIndex][i]<strategy[i]){
            printf("Warning: Your allocation exceeds tha maximum value you need!\n");
            printf("The allocation is automatically set to the maximum matrix. \n");
            strategy[i] = maximum[customerIndex][i];
        }
    }
    for(int i=0;i<num_resources;i++){
        allocation[customerIndex][i] += strategy[i];
        need[customerIndex][i] -= strategy[i];
        available[i] -= strategy[i];
    }
}

/* release the resources for a specific customer */
int releaseResources(int customerIndex, int *strategy){
    int flag = 1;
    for(int i=0;i<num_resources;i++){
        if(allocation[customerIndex][i]<strategy[i]){
            flag = 0;
        }
    }
    if(flag){
        for(int i=0;i<num_resources;i++){
            allocation[customerIndex][i] -= strategy[i];
            need[customerIndex][i] += strategy[i];
            available[i] += strategy[i];
        }
    }
    return flag;
}

/* core of the banker's alg
 * use recursion to check if 
 * we satisfy the request, 
 * whether deadlock is possible
 * to happen(banker's algorithm) */
int checkSafeWrapper(){
    int flag;
    int *finished = malloc(sizeof(int) * num_customers);
    _maximum = malloc(sizeof(int *) * num_customers);
    _need = malloc(sizeof(int *) * num_customers);
    _allocation = malloc(sizeof(int *) * num_customers);
    _available = malloc(sizeof(int) * num_customers);
    for(int i=0;i<num_customers;i++){
        _maximum[i] = malloc(sizeof(int) * num_resources);
        _need[i] = malloc(sizeof(int) * num_resources);
        _allocation[i] = malloc(sizeof(int) * num_resources);
        memcpy(_maximum[i], maximum[i], num_resources * sizeof(int));
        memcpy(_need[i], need[i], num_resources * sizeof(int));
        memcpy(_allocation[i], allocation[i], num_resources * sizeof(int));
    }
    memcpy(_available, available, sizeof(int) * num_customers);

    for(int i=0;i<num_customers;i++)    finished[i] = 0;

    flag = checkSafe(finished);
    free(maximum);      maximum = _maximum;
    free(need);         need = _need;
    free(allocation);   allocation = _allocation;
    free(available);    available = _available;
    
    return flag;
}

int checkSafe(int *finished){
    int i;
    for(i=0;i<num_customers && finished[i];i++) ;
    if(i == num_customers)  return 1;

    int customerIndex = findSomeCustomer(finished);
    if(customerIndex == -1)   return 0;
    else{
        releaseResources(customerIndex, allocation[customerIndex]);
        finished[customerIndex] = 1;
        checkSafe(finished);
    }
}

/* whether current available 
 * array can satisfies the need of any
 * customer. NULL means none */
int findSomeCustomer(int *finished){
    int i, j;
    for(i=0;i<num_customers;i++){
        for(j=0;j<num_resources;j++){
            if(need[i][j] > available[j])   break;
        }
        if(j == num_resources && !finished[i])  return i;
    }
    return -1;
}

/* parse the input(e.g. 7 3 3 4) */
int* parseLine(char *buffer){
    int i, j=0;
    int *arr = malloc(sizeof(int) * (num_resources+1));
    /* get the customer index */
    arr[j++] = (int)(buffer[0]) - (int)('0');

    for(i=2;i<MAX_LINE_LENGTH;i++){
        if(buffer[i] == ' ' || buffer[i] == '\n')   arr[j++] = (int)(buffer[i-1]) - (int)('0');
        if(buffer[i] == '\n')   break;
    }
    if(j != num_resources+1){
        fprintf(stderr, "Invalid input, it should match the number of resources.\n");
    }
    return arr;
}

/* initialize the martices */
void initArrays(){
    char *buffer;
    int i,j,idx;
    FILE *fp = fopen(INPUT_FILEPATH, "r");
    assert(fp);
    
    /* do not use the fscanf, which will assume
     * that we know the number of resources beforehand*/
    buffer = malloc(sizeof(char) * MAX_LINE_LENGTH);
    while(fgets(buffer, MAX_LINE_LENGTH, fp) != NULL){
        /* while we have some input */
        if(num_resources == 0){
            /* for the first time 
             * we compute the number of resources */
            for(i=0;i<MAX_LINE_LENGTH;i++){
                if(buffer[i] == ',' || buffer[i] == '\n')    num_resources++;
                if(buffer[i] == '\n')   break;
            }
        }
        num_customers++;
    }
    printf("There are %d customers and %d resources types\n", num_customers, num_resources);
    maximum = malloc(sizeof(int *) * num_customers);
    allocation = malloc(sizeof(int *) * num_customers);
    need = malloc(sizeof(int *) * num_customers);
    available = malloc(sizeof(int) * num_resources);

    for(i=0;i<num_customers;i++){
        maximum[i] = malloc(sizeof(int) * num_resources);
        allocation[i] = malloc(sizeof(int) * num_resources);
        need[i] = malloc(sizeof(int) * num_resources);
        memset(allocation[i], 0, num_resources);
    }
    memset(available, 0, num_customers);

    /* move back to the beginning */
    fseek(fp, 0, SEEK_SET);
    i = j = 0;
    while(fgets(buffer, MAX_LINE_LENGTH, fp) != NULL){
        for(idx=0;idx<MAX_LINE_LENGTH;idx++){
            if(buffer[idx] == ',' || buffer[idx] == '\n'){
                need[i][j] = atoi((char *)&buffer[idx-1]);
                maximum[i][j] = need[i][j];
                j++;
            }
            if(buffer[idx] == '\n') break;
        }
        i++; j=0;
    }
    fclose(fp);
    printf("Initialization finished!\n");
}

/* output all the arrays */
void printArrays(){
    int i,j;
    printf("The maximum array is as follows:\n");
    for(i=0;i<num_customers;i++){
        for(j=0;j<num_resources;j++)
            printf("%d\t", maximum[i][j]);
        printf("\n");
    }
    printf("The allocation array is as follows:\n");
    for(i=0;i<num_customers;i++){
        for(j=0;j<num_resources;j++)
            printf("%d\t", allocation[i][j]);
        printf("\n");
    }
    printf("The need array is as follows:\n");
    for(i=0;i<num_customers;i++){
        for(j=0;j<num_resources;j++)
            printf("%d\t", need[i][j]);
        printf("\n");
    }
    printf("The available array is as follows:\n");
    for(i=0;i<num_resources;i++)
        printf("%d\t", available[i]);
    printf("\n");
}

char *preprocess(char *buffer){
    int i,j=0;
    int find, last;
    char *tmp = malloc(sizeof(char) * MAX_LINE_LENGTH);
    last = 0;
    for(i=0;i<MAX_LINE_LENGTH;i++){
        if(buffer[i] == '\n')   break;
        if(buffer[i] != ' '){
            last = i;
            tmp[j++] = buffer[i];
        }else if(i-last == 1){
            tmp[j++] = buffer[i];
        }
    }
    if(tmp[j-1] == ' ') tmp[j-1] = '\n';
    else tmp[j] = '\n';
    free(buffer);
    return tmp;
}

/* destroy the arrays */
void destroyArrays(){
    for(int i=0;i<num_resources;i++){
        free(maximum[i]);
        free(allocation[i]);
        free(need[i]);
    }    
    free(maximum);
    free(allocation);
    free(need);
    free(available);
    printf("destruction finished!\n");
}
