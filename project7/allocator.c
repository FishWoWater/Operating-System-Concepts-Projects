/*************************************************************************
	> File Name: allocator.c
	> Author: Junjie Wang
	> Mail: dreamboy.gns@sjtu.edu.cn
  > Website:http://www.dbgns.com
  > Blog:http://www.dbgns.com/blog
	> Created Time: 2019年05月19日 星期日 09时39分01秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "allocator.h"

int max_size;
struct node *head, *tail, *newNode;
Block *block, *tmp_block;
/* the number of blocks */
int num_blocks = 0;

int main(int argc, char *argv[]){ 
    if(argc != 2){
        fprintf(stderr, "Error! Insufficient arguments!\n");
        exit(-1);
    }
    /* initialize */
    head = malloc(sizeof(struct node *));
    tail = malloc(sizeof(struct node *));
    head->next = tail;  tail->prev = head;
    max_size = atoi(argv[1]);
    block = malloc(sizeof(Block));
    newNode = malloc(sizeof(struct node));
    block->start = 0; 
    block->end = max_size;
    block->type = 0;     
    newNode->block = block;
    insertNode(head, tail, newNode);

    char *buffer = malloc(sizeof(char) * MAX_LENGTH);
    while(1){
        printf("allocator>");
        fgets(buffer, MAX_LENGTH, stdin);
        buffer = preprocess(buffer);
        if(buffer[0] == 'X'){
            printf("Bye.\n");
            break;
        }else if(buffer[0] == 'C'){
            /* compact */
            //handleCompactSimple();
            handleCompactShuffle();
        }else if(strncmp(buffer, "RQ", 2) == 0){ 
            /* request the memory, use buffer+3 to get rid of the "RQ" at the beginning */
            handleRequest(buffer+3);
        }else if(strncmp(buffer, "RL", 2) == 0){
            /* release the memory to a specific process */
            handleRelease(buffer+3);
        }else if(strncmp(buffer, "STAT", 2) == 0){
            /* print out the memory allocation status */
            printStatus();
        }else{
            fprintf(stderr, "Unrecognized input!\n");
            usage();
        }
    }
    return 0;
}

void insertNode(struct node *head_p, struct node *tail_p, struct node *newNode){
    if(head_p -> next == tail_p){
        head_p -> next = newNode;
        newNode -> prev = head_p;
        newNode -> next = tail_p;
        tail_p -> prev = newNode;
    }else{
        /* insert the node at the end */
        tail_p -> prev -> next = newNode;
        newNode -> prev = tail_p -> prev;
        tail_p -> prev = newNode;
        newNode -> next = tail_p;
    }
}

void splitNode(int size, char *name, struct node *oldNode){
    block = oldNode -> block;
    if(block -> end - block -> start == size){
        /* if we have size == end - start, that's perfect */
        strcpy(block->name, name);
        block->type = 1;
    }else{
        /* else we have to split the node */
        tmp_block = malloc(sizeof(Block));
        newNode = malloc(sizeof(struct node));
        tmp_block -> start = block -> start;
        tmp_block -> end = block -> start + size;
        block -> start = block -> start + size;
        /* since the first one is the allocation 
        * its type = 1 */
        tmp_block -> type = 1;
        /* if use strcpy() function, 
         * don't forget tmp_block -> name = malloc(...) */
        tmp_block -> name = name;
        newNode -> block = tmp_block;
        oldNode -> prev -> next = newNode;
        newNode -> prev = oldNode -> prev;
        newNode -> next = oldNode;
        oldNode -> prev = newNode;
    }   
}

void printStatus(){
    struct node *p = head->next;
    while(p != tail){
        block = p->block;
        printf("Addresses\t[%d: %d]\t", block->start , block->end);
        if(block->type)  printf("Process\t%s\n", block->name);
        else    printf("Unused\n");
        p = p->next;
    }
}

void handleRequest(char *buffer){
    int size;
    char fit_type;
    char *name = malloc(sizeof(char) * MAX_LENGTH);
    struct node *p = head->next;
    sscanf(buffer, "%s %d %c", name, &size, &fit_type);
    switch(fit_type){
        case 'F':
        {
            /* using the first hit strategy */
            while(p!=tail){
                block = p->block;
                if(block->type == 0 && block->end - block->start >= size){
                    splitNode(size, name, p);
                    break;
                }
                p = p->next;
            }
            if(p == tail)   fprintf(stderr, "Error! No space to allocate. You may try compaction fisrt!\n");
            break;
        }
        case 'B':
        {
            /* using the best fit strategy 
             * maintain the min size which 
             * satisfies our condition */
            int min_hole = max_size;
            struct node *best_node;
            while(p!=tail){
               block = p->block;
                if(block->type == 0 && block->end - block->start >= size){
                    if(block->end - block->start <= min_hole){
                        min_hole = block->end - block->start;
                        best_node = p; 
                    }
                }
                p = p->next;
            }
            if(min_hole == max_size) fprintf(stderr, "Error! No space to allocate. You may try compaction first!\n");
            else splitNode(size, name, best_node);
            break;
        }
        case 'W':
        {
            int max_hole = 0;
            struct node *largest_node;
            while(p!=tail){
                block = p->block;
                if(block->type == 0 && block->end - block->start >= size){
                    if(block->end - block->start > max_hole){
                        max_hole = block->end - block->start;
                        largest_node = p;
                    }
                }
                p = p->next;
            }
        if(max_hole == 0)    fprintf(stderr, "Error! No space to allocate. You may try compaction first!\n");
        else splitNode(size, name, largest_node);
        }   
    }   
}

void handleRelease(char *buffer){
    int idx = 0;
    char *name = malloc(sizeof(char) * MAX_LENGTH);
    for(;idx<MAX_LENGTH && buffer[idx]!=' ' && buffer[idx] != '\n';idx++) ;
    /* get the name of the process */
    strncpy(name, buffer, idx);
    /* release all the memory allocated to that process */
    struct node *p = head->next;
    while(p != tail){
        block = p->block;
        if(block->type == 1 && strcmp(block->name, name) == 0){
            /* simply set this type to 0 */
            block->type = 0;
            /* then check whether we can merge holes */
            mergeHole(p);
        }
        p = p->next;
    }
}

/* check whether we can merge the node with 
 * its previous one and its following one */
void mergeHole(struct node *target){
    newNode = malloc(sizeof(struct node));
    block = malloc(sizeof(Block));
    if(target->prev != head && target->prev->block->type == 0){
        /* merge current one with previous one */
        block -> start = target -> prev -> block -> start;
        block -> end = target -> block -> end;
        block -> type = 0;
        newNode -> block = block;
        target -> prev -> prev -> next = newNode;
        newNode -> prev = target -> prev -> prev;
        newNode -> next = target -> next;
        target -> next -> prev = newNode;
        free(target);
        free(target->prev);
    } 
    if(target->next != tail && target->next->block->type == 0){
        /* merge current one with following one */
        block -> start = target -> block -> start;
        block -> end = target -> next -> block -> end;
        block -> type = 0;
        newNode -> block = block;
        target -> next -> next -> prev = newNode;
        newNode -> next = target -> next -> next;
        target -> prev -> next = newNode;
        newNode -> prev = target -> prev;
        free(target);
        free(target->next);
    }
}

void handleCompactSimple(){
    struct node *p = head->next;
    struct node *q = malloc(sizeof(struct node));
    struct node *new_head = malloc(sizeof(struct node));
    struct node *new_tail = malloc(sizeof(struct node));
    new_head -> next = new_tail;    new_tail -> prev = new_head;

    /* total size of the holes */
    int sum_holes = 0, idx;
    /* first traverse -> get the sum_holes */
    while(p != tail){
        if(p->block->type == 0) sum_holes += p->block->end - p->block->start;
        p = p->next;
    }
    /* allocate a large block at the beginning of q */
    block = malloc(sizeof(Block));
    block->start = 0;
    block->end = sum_holes;
    block->type = 0;
    q -> block = block;
    insertNode(new_head, new_tail, q);
    /* second traverse, handle the allocation blocks*/
    p = head->next;
    idx = sum_holes;
    while(p != tail){
        if(p->block->type == 1){
            q = malloc(sizeof(struct node));
            block = malloc(sizeof(block));
            block -> start = idx;
            block -> end = idx + (p->block->end - p->block->start);
            block -> type = 1;
            block -> name = malloc(sizeof(char) * MAX_LENGTH);
            strcpy(block->name, p->block->name);
            q -> block = block;
            insertNode(new_head, new_tail, q);
            idx += (p->block->end - p->block->start);
        } 
        p = p->next;
    }
    destroyNodes(head, tail);
    head = new_head;
    tail = new_tail;
}

/* As we traverse the link list, whenever we meet 
 * type 0 block, we shuffle that forward. Finally we 
 * will have all the holes on one side of memory. */
void handleCompactShuffle(){
    struct node *p = head->next, *prev;
    while(p != tail){
        /* only when we meet unused */
        if(p->block->type == 0 && p->prev != head){
            /* since we merge the holes when 
             * allocation, the previous one must
             * be allocated and we can just exchange */
            prev = p->prev;
            /* actually this operation may be implemented as memory copy */
            prev->block->start += (p->block->end - p->block->start);
            prev->block->end += (p->block->end - p->block->start);
            p->block->start -= (prev->block->end - prev->block->start);
            p->block->end -= (prev->block->end - prev->block->start);
            /* now exchange these two */
            prev->prev->next = p;
            p->prev = prev->prev;
            p->next->prev = prev;
            prev->next = p->next;
            p -> next = prev;
            /* merge the the holes */
            mergeHole(p);
            /* as we shift p ahead , add one more p -> next step */
            p = p->next ;
        }
        p = p->next;
    }
}

/* destory a link list */
void destroyNodes(struct node *head_p, struct node *tail_p){
    struct node *tmp;
    while(head_p != tail_p){
        tmp = head_p;
        head_p = head_p -> next;
        free(tmp);
    }
    free(head_p);
}

/* preprocess the string,
 * stripping unnecessary space */
char *preprocess(char *buffer){
    char *tmp = malloc(sizeof(char) * MAX_LENGTH);
    int i, j=0, last=0;
    for(i=0;i<MAX_LENGTH;i++){
        if(buffer[i] == '\n')   break;
        if(buffer[i] != ' '){
            last = i;
            tmp[j++] = buffer[i];
        }else if(i-last == 1 && j!=0){
            tmp[j++] = buffer[i];
        }   
    }
    if(tmp[j-1] == ' ') tmp[j-1] = '\n';
    else    tmp[j] = '\n';
    free(buffer);
    buffer = tmp;
    return buffer;
}

void usage(){
    printf("Allocator usage:\n");
    printf("        RQ      Request the memory      example: RQ P0 40000 W\n");
    printf("        RL      Release the memory      example: RL P0\n");
    printf("        C       Compact the memory\n");
    printf("        STATUS  Print out the memory status\n");
    printf("        X       Exit\n");
}
