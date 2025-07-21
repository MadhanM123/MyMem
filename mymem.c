#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

typedef char ALIGN_16[16];


union header{
    struct {
        size_t sz;
        unsigned free;
        union header* next;
    } s;
    ALIGN_16 mem_align;
};

typedef union header header_t;

header_t *head, *tail;

pthread_mutex_t global_malloc_lock;




void* malloc(size_t size){
    size_t tot_sz;
    void* block;
    header_t* header;

    if(!size){
        return NULL;
    }

    pthread_mutex_lock(&global_malloc_lock);
    header = get_free_block(size);

    if(header){
        header->s.free = 0;
        pthread_mutex_unlock(&global_malloc_lock);
        return (void*)(header + 1);
    }

    tot_sz = sizeof(header_t) + size;
    block = sbrk(tot_sz);

    if(block == (void*) - 1){
        pthread_mutex_unlock(&global_malloc_lock);
        return NULL;
    }

    header = block;
    header->s.sz = size;
    header->s.next = NULL;
    header->s.free = 0;

    if(!head){
        head = header;
    }

    if(tail){
        tail->s.next = header;
    }

    tail = header;
    pthread_mutex_unlock(&global_malloc_lock);

    return (void*)(header + 1);
}

header_t* get_free_block(size_t req_sz){
    header_t* curr = head;

    while(curr){
        if(curr->s.free && curr->s.sz >= req_sz){
            return curr;
        }
        curr = curr->s.next;
    }

    return NULL;
}

