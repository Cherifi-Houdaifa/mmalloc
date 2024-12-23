// memory allocator with fixed chunks sizes

#ifndef MMALLOC_H
#define MMALLOC_H 1

#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <stdio.h>

#define CHUNK_SIZE 0x40

// start of memory for the allocator
void* memstart = NULL;
size_t memsize = 0x0;

// end of used region in the mmaped memory
void* memused = NULL;


struct freelist_t {
    struct freelist_t* next;
};
struct freelist_t* freelist = NULL;
size_t freed_chunks = 0x0;


void *mmalloc(size_t size) {
    // return value;
    void* p;

    if (size != CHUNK_SIZE) {
        return (void*)-1;
    }


    if (memstart == NULL) {
        memstart = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0x0);
        if (memstart == (void*)-1) {
            fputs("mmap: an error happened", stderr);
            return (void*)-1;
        }
        memsize = 0x1000;
        memused = memstart;
    }

    if (freed_chunks == 0) {
        // make a new chunk
        if (memused + CHUNK_SIZE > memstart + memsize) {
            // we need to mmap more memory
            void* mem = mmap(memstart + memsize, 0x1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0x0);
            if (mem == (void*)-1) {
               fputs("mmap: an error happened", stderr);
                return (void*)-1;
            }
            memsize = memsize + 0x1000;
        }
        p = memused;
        memused = memused + CHUNK_SIZE;

    } else if (freed_chunks > 0) {
        p = freelist;
        freelist = freelist->next;
        freed_chunks--;
    }
    return p;
}

void mfree(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    struct freelist_t* head = (struct freelist_t*)ptr;
    head->next = freelist;
    freelist = head;
    freed_chunks++;
}


#endif