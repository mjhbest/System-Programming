/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

static size_t* heap_p;

#define B_SIZE      1
#define D_SIZE      2
#define HD_FT       8

#define read(p)     (*(size_t *)(p))
#define write(p, val) (*(size_t *)(p) = (val))
#define get_size(p)     (read(p) & ~0x7)
#define get_alloc(p)    (read(p) & 0x1)

#define header(p)   (p-B_SIZE)
#define footer(p)   (p + get_size(header(p))/4-D_SIZE)

#define next(p)     ((size_t *)read(p) )
#define prev(p)     ((size_t *)read(p+B_SIZE))
#define PACK(size, alloc)   ((size) | (alloc))

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
int mm_init(void);
void *mm_malloc(size_t size);
void mm_free(void *ptr);
void *find_fit(size_t size);
void *mm_realloc(void *ptr, size_t size);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    if((heap_p = mem_sbrk(12))==NULL){
        return -1;
    }
    write(heap_p,0);
    write(heap_p+B_SIZE,PACK(0,1));
    write(heap_p+2*B_SIZE,PACK(0,1));
    return 0;
}   
/* 
 * mm_ma    lloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t *p = find_fit(ALIGN(size)+HD_FT);
    size_t psize;
    if(p){
        size_t fsize = get_size(header(p));
        size_t *ne = next(p);
        size_t *pr = prev(p);
        if((fsize-(ALIGN(size)+HD_FT))<=ALIGNMENT){
            write(header(p),PACK(get_size(header(p)),1));
            write(footer(p),PACK(get_size(header(p)),1));
            write(pr,(size_t)ne);
            if(ne)  write(ne+B_SIZE,(size_t)pr);
        }
        else{
            psize = get_size(header(p)-ALIGN(size)-HD_FT);
            write(header(p),PACK(ALIGN(size)+HD_FT,1));
            write(footer(p),PACK(ALIGN(size)+HD_FT,1));
            p +=(ALIGN(size)+HD_FT)/4;
            write(header(p),PACK(psize,1));
            write(footer(p),PACK(psize,1));
            write(pr,(size_t)p);
            write(p,(size_t)ne);
            write(p+B_SIZE,(size_t)pr);
            if(ne)  write(ne+B_SIZE,(size_t)p);
            p -= (ALIGN(size)+HD_FT)/4;
        }
    }
    else{
        p = mem_sbrk(ALIGN(size)+HD_FT);
        p = p+ B_SIZE;
        write(header(p),PCK(ALIGN(size)+HD_FT,1));
        write(footer(p),PACK(ALIGN(size)+HD_FT,1));
    }
    return p;
    /*int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
    return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }*/

}


/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr){
    
    size_t *re_ptr = (size_t*)ptr;
    size_t * start = mem_heap_lo();
    size_t  prev_alloc = get_alloc(re_ptr-D_SIZE);
    size_t  next_alloc = get_alloc(footer(re_ptr)+B_SIZE);
    size_t newsize;
    if(!ptr)    return;
    if(get_alloc(footer(re_ptr)+B_SIZE) >= (size_t)mem_heap_hi())   next_alloc = 1;

    if(prev_alloc && next_alloc){
        write(header(re_ptr),get_size(header(re_ptr)));
        write(footer(re_ptr),get_size(footer(re_ptr)));
        size_t *ne = (size_t*)*start;
        write(re_ptr,(size_t)ne);
        write(start,(size_t)re_ptr);
        write(re_ptr+B_SIZE,(size_t)start);
        if(ne)   write(ne+B_SIZE,(size_t)re_ptr);
    }
    else if(!prev_alloc && next_alloc){
        newsize = get_size(header(re_ptr))+(*(re_ptr-D_SIZE));
        re_ptr = re_ptr - (*(re_ptr-D_SIZE))/4;
        write(header(re_ptr),newsize);
        write(footer(re_ptr),newsize);
    }
    else if(prev_alloc && ! next_alloc){
        size_t old = get_size(header(re_ptr));
        re_ptr += old/4;

        size_t *ne=next(re_ptr);
        size_t *pr=next(re_ptr);
        newsize = get_size(header(re_ptr))+old;
        re_ptr -= old/4;

        write(header(re_ptr),newsize);
        write(footer(re_ptr),newsize);
        write(re_ptr,(size_t)ne);
        write(pr,(size_t)re_ptr);
        write(re_ptr+B_SIZE,(size_t)pr);
        if(ne)  write(ne+B_SIZE,(size_t)re_ptr);
    }

    else{
        size_t old = get_size(header(re_ptr));
        newsize =(*(re_ptr-D_SIZE))+get_size(header(re_ptr))+(get_size(header(re_ptr+get_size(header(re_ptr))/4))); 
        re_ptr += old/4;
        size_t *ne = next(re_ptr);
        size_t *pr = prev(re_ptr);

        write(pr,(size_t)ne);
        if(ne)  write(ne+B_SIZE,(size_t)pr);

        re_ptr -= old/4;
        re_ptr -= (*(re_ptr -D_SIZE))/4;
        write(header(re_ptr),newsize);
        re_ptr += (*(re_ptr -D_SIZE))/4;

    }
}



void *find_fit(size_t size){
    size_t * fit = NULL;
    size_t *p = mem_heap_lo();
    int cnt=0;
    for(p=(size_t*)*p;p;p = (size_t*)*p){
        if(get_size(header(p))>size){
            if(!cnt){
                fit = p;
                cnt++;
            }
            else if(get_size(header(p))>get_size(header(p))){
                fit = p;
            }
        }
    }
    return fit;
}
/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    size_t old;
    void * newptr;
    if(size ==0){
        free(ptr);
        return 0;
    }

    if(ptr == NULL) return malloc(size);
    newptr = malloc(size);
    if(!newptr) return 0;

    old = get_size(header(ptr-4));
    if(size<old)    old=size;
    memcpy(newptr,ptr,old);
    free(ptr);
    return newptr;

    /*void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;*/
}














