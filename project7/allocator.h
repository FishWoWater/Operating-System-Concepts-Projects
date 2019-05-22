/*************************************************************************
	> File Name: allocator.h
	> Author: Junjie Wang
	> Mail: dreamboy.gns@sjtu.edu.cn
  > Website:http://www.dbgns.com
  > Blog:http://www.dbgns.com/blog
	> Created Time: 2019年05月19日 星期日 09时51分04秒
 ************************************************************************/

#ifndef _ALLOCATOR_H
#define _ALLOCATOR_H

/* max length of the input */
#define MAX_LENGTH 50

/* define the struct to represent the allocation and hole */
typedef struct{
    /* type 1: allocation 
     * type 0: hole(namely unused) */
    int type;
    int start;
    int end;
    /* the process id */
    char *name;
} Block;

/* use double link list */
struct node{
    Block *block;
    struct node *prev;
    struct node *next;
};


/* print help about the input arguments */
void usage();
/* destroy the nodes in some link list(specified by the head and tail pointers) */
void destroyNodes(struct node *head_p, struct node *tail_p);
/* preprocess the buffer string to get rid of unnecessary space characters */
char *preprocess(char *buffer);
/* handle the "RQ" input */
void handleRequest(char *buffer);
/* handle the "RL" input */
void handleRelease(char *buffer);
/* insert a node to some link list(specified by the head_p and tail_p pointers) */
void insertNode(struct node *head_p, struct node *tail_p, struct node *newNode);
/* split a unused node(as we have to allocate the space) */
void splitNode(int size, char *name, struct node *oldNode);
/* merge adjacent holes */
void mergeHole(struct node *target);
/* simple/false way to handle compact the memory */
void handleCompactSimple();
/* shuffle unused ahead to one side of the memory space */
void handleCompactShuffle();
/* print out the memory allocation status */
void printStatus();

#endif
