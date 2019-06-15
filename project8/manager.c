/*************************************************************************
	> File Name: manager.c
	> Author: Junjie Wang
	> Mail: dreamboy.gns@sjtu.edu.cn
  > Website:http://www.dbgns.com
  > Blog:http://www.dbgns.com/blog
	> Created Time: 2019年05月19日 星期日 16时20分24秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "manager.h"

/* the number of pages */
int num_pages = 1;
/* page size */
int page_size = 1;
/* the number of frames */
int num_frames = 1;
/* frame size */
int frame_size = 1;

/* page table */
int *page_table;
/* the physical memory */
char **memory;
/* the lru for the memory */
int *memory_lru;
/* keep track of the free frames */
int *free_frames;
/* the TLB table to make things faster */
entry *tlb_table;

/* the number of hits and visits */
int num_addresses = 0;
int num_faults = 0;
int tlb_hits = 0;

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr, "Invalid arguments!\n");
        exit(1);
    }
    /* since we only have 16 bits address at most,
     * here int type is sufficient */
    int address;
    int page_idx,frame_idx, offset;
    int i, min_lru, min_lru_idx;
    signed char data;
    signed char *buffer;
    /* buffer will be used to store the data from backing store */
    FILE *fp = fopen(argv[1], "r");
    //FILE *fp_out = fopen(OUTPUT, "w");
    FILE *fp_back = fopen(BACKING_STORE, "rb");
    assert(fp && fp_back);
    init();
    while(!feof(fp)){
        fscanf(fp, "%d", &address);
        /* first get the page offset and page number */
        offset = address % page_size;
        page_idx = (address / page_size) % num_pages;
        /* first look up the TLB table */
        if((frame_idx = lookUpTLB(page_idx)) != -1){
            data = memory[frame_idx][offset];
            updateLRUTLB(page_idx);
            tlb_hits++;
        }else if(page_table[page_idx] == -1){
            /* loop up the page table */
            
            /* -1 means page fault(we don't have a frame number 
             * at page_idx in page table */
            
            /* 1. read data from BACKING_STORE 
             * find the position */
            buffer = malloc(sizeof(char) * page_size);
            fseek(fp_back, (page_idx) * page_size, SEEK_SET);
            fread(buffer, sizeof(char), page_size, fp_back);
            /* 2. find a free frame and store the data */
            frame_idx = findFreeFrame();
            if(frame_idx == -1){
                /* can not find a free frame, we must use page replacement */
                /* choose a victim and replace the frames */
                min_lru = MAX_LRU;
                min_lru_idx = 0;
                for(i=0;i<num_frames;i++){
                    if(memory_lru[i] < min_lru){
                        min_lru = memory_lru[i];
                        min_lru_idx = i;
                    }
                }
                frame_idx = min_lru_idx;
                updateLRUMemory(min_lru_idx);
            }
            memcpy(memory[frame_idx], buffer, page_size);
            free_frames[frame_idx] = 0;
            /* 3. update the page table and TLB */
            page_table[page_idx] = frame_idx;
            /* choose a victim and update TLB */
            min_lru = MAX_LRU;
            min_lru_idx = 0;
            for(i=0;i<TLB_ENTRIES;i++){
                if(tlb_table[i].lru < min_lru){
                    min_lru = tlb_table[i].lru;
                    min_lru_idx = i;
                }
            }
            tlb_table[min_lru_idx].page_idx = page_idx;
            tlb_table[min_lru_idx].frame_idx = frame_idx;
            updateLRUTLB(page_idx);
            
            num_faults++;
        }else{
            /* hit in the page table */
            frame_idx = page_table[page_idx];
            data = memory[frame_idx][offset];
            
            min_lru = MAX_LRU;
            min_lru_idx = 0;
            for(i=0;i<TLB_ENTRIES;i++){
                if(tlb_table[i].lru < min_lru){
                    min_lru = tlb_table[i].lru;
                    min_lru_idx = i;
                }
            }
            tlb_table[min_lru_idx].page_idx = page_idx;
            tlb_table[min_lru_idx].frame_idx = frame_idx;
            updateLRUTLB(page_idx);
            
        }
        num_addresses++;
    }
    /* test the input */
    testInput();

    printf("Page fault: %.3f%%\n", num_faults * 100.0 /num_addresses);
    printf("TLB hit: %.3f%%\n", tlb_hits * 100.0 / num_addresses);
    return 0;
}

/* find a free frame to store the data */
int findFreeFrame(){
    for(int i=0;i<num_frames;i++){
        if(free_frames[i] == 1) return i;
    }
    return -1;
}

/* test the input(addresses.txt) and compare with the correct.txt */
void testInput(){
    int address, offset;
    int page_idx, frame_idx;
    signed char data;
    char *ref, *out;
    FILE *fp = fopen(INPUT, "r");
    FILE *fp_out = fopen(OUTPUT, "w");
    assert(fp && fp_out);
    out = malloc(sizeof(char) * MAX_LENGTH);
    while(!feof(fp)){
        fscanf(fp, "%d", &address);
        /* first get the page offset and page number */
        offset = address % page_size;
        page_idx = (address / page_size) % num_pages;
        frame_idx = page_table[page_idx];
        data = memory[frame_idx][offset];
        fprintf(fp_out, "Virtual address: %d, Physical address: %d, Value: %d\n", address, (frame_idx) * frame_size + offset, data);
    }
    printf("Process finished! Output saved to => %s\n", OUTPUT);
}

int lookUpTLB(int page_idx){
    for(int i=0;i<TLB_ENTRIES;i++){
        if(tlb_table[i].page_idx == page_idx){
            updateLRUTLB(page_idx);
            return tlb_table[i].frame_idx;
        }
    }
    /* can not find in the TLB */
    return -1;
}

/* update the lru for the tlb */
void updateLRUTLB(int page_idx){
    for(int i=0;i<TLB_ENTRIES;i++){
        if(tlb_table[i].page_idx == page_idx)   tlb_table[i].lru = MAX_LRU;
        else    tlb_table[i].lru--;
    
    }
}

/* update the LRU for the memory */
void updateLRUMemory(int frame_idx){
    for(int i=0;i<num_frames;i++){
        if(i == frame_idx)  memory_lru[i] = MAX_LRU;
        else    memory_lru[i]--;
    }
}

/* do some preparation work */
void init(){
    int i;
    for(i=0;i<PAGE_OFFSET_BITS;i++)     page_size = page_size << 1;
    for(i=0;i<PAGE_NUMBER_BITS;i++)     num_pages = num_pages << 1;
    for(i=0;i<FRAME_OFFSET_BITS;i++)    frame_size = frame_size << 1;
    for(i=0;i<FRAME_NUMBER_BITS;i++)    num_frames = num_frames << 1;
    printf("Page numbers: %d, Page size: %d\n", num_pages, page_size);
    printf("Frame numbers: %d, Frame size: %d\n", num_frames, frame_size);

    page_table = malloc(sizeof(int) * num_pages);
    /* initially, set each element to -1 */
    for(i=0;i<num_pages;i++)    page_table[i] = -1;
    memory = malloc(sizeof(char *) * num_frames);
    for(i=0;i<num_frames;i++)   memory[i] = malloc(sizeof(char) * frame_size);
    
    free_frames = malloc(sizeof(int) * num_frames);
    /* set each frame to be free initially */
    for(i=0;i<num_frames;i++)   free_frames[i] = 1;
    /* initilize the TLB table, note for odd subscripts, we store
     * the page index, and for even subscripts, store the frame index */
    tlb_table = malloc(sizeof(entry) * TLB_ENTRIES);
    for(i=0;i<TLB_ENTRIES;i++)    { tlb_table[i].page_idx = -1; tlb_table[i].lru = MAX_LRU; }
    
    memory_lru = malloc(sizeof(int) * num_frames);
    for(i=0;i<num_frames;i++)  memory_lru[i] = MAX_LRU; 
}
