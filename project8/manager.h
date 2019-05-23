/*************************************************************************
	> File Name: manager.h
	> Author: Junjie Wang
	> Mail: dreamboy.gns@sjtu.edu.cn
  > Website:http://www.dbgns.com
  > Blog:http://www.dbgns.com/blog
	> Created Time: 2019年05月19日 星期日 16时20分35秒
 ************************************************************************/

#ifndef _MANAGER_H
#define _MANAGER_H

/* maximum length of each line in the file */
#define MAX_LENGTH 20

/* file */
#define BACKING_STORE "./BACKING_STORE.bin"
#define INPUT "./addresses.txt"
#define OUTPUT "./output.txt"
#define CORRECT "./correct.txt"

/* page and frames */
#define PAGE_OFFSET_BITS 8
#define PAGE_NUMBER_BITS 8
#define FRAME_OFFSET_BITS 8
#define FRAME_NUMBER_BITS 8

/* the number of entries in TLB */
#define TLB_ENTRIES 16
/* define the maximum value of LRU */
#define MAX_LRU 1000

/* TLB entry struct */
typedef struct _entry{
    int page_idx;
    int frame_idx;
    int lru;
} entry;

/* initialization */
void init();
/* output the results */
void testInput();
/* find a free frame */
int findFreeFrame();
int lookUpTLB(int page_idx);
void updateLRUTLB(int page_idx);
/* update the LRUs maintained for the physical memory */
void updateLRUMemory(int frame_idx);
#endif
