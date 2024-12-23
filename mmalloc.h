// memory allocator with dynamic sizes (from 0x20 to 0x100)

#ifndef MMALLOC_H
#define MMALLOC_H 1

#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>

#define HEAP_START (void*)0x100000


void* memstart = NULL;
size_t memsize = 0x0;
void* memused = NULL;



struct freelist_t {
    struct freelist_t* next;
};

struct bin_t {
    size_t freed;
    struct freelist_t* freelist;
};


// from 0x20 to 0x100
struct bin_t bins[15];



void* mmalloc(size_t size) {
    void* p;

    if (size > 0x100) {
        return (void*)-1;
    }
    size = (size + 0xf) & (~0xfUL);
    if (size < 0x20)
        size = 0x20;

    if (memstart == NULL) {
        memstart = mmap(HEAP_START, 0x1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0x0);
        if (memstart == (void*)-1) {
            fputs("mmap: an error happened", stderr);
            return (void*)-1;
        }
        memsize = 0x1000;
        memused = memstart;
    }
    size_t idx = (size / 0x10) - 2;
    if (bins[idx].freed > 0) {
        // get from freelist
        p = bins[idx].freelist;
        bins[idx].freelist = bins[idx].freelist->next;
        bins[idx].freed--;
    } else {
        // make new chunk
        if (memused + size + 0x8 > memstart + size) {
            void* mem = mmap(memstart + memsize, 0x1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0x0);
            if (mem == (void*)-1) {
                fputs("mmap: an error happened", stderr);
                return (void*)-1;
            }
            memsize = memsize + 0x1000;
        }
        p = memused;
        *(size_t*)p = size;
        p = memused + 8;
        memused = memused + size + 8;
    }
    return p;
}

void mfree(void* ptr) {
    if (ptr == NULL)
        return;
    size_t size = *(size_t*)(ptr - 8);
    size_t idx = (size / 0x10) - 2; 

    struct freelist_t* head = ptr;
    head->next = bins[idx].freelist;
    bins[idx].freelist = head;
    bins[idx].freed++;
}


#endif