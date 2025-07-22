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

void free(void* block){
    header_t *header, *tmp;
    void* prgmbrk;

    if(!block){
        return;
    }

    pthread_mutex_lock(&global_malloc_lock);
    header = (header_t*)block - 1;

    prgmbrk = sbrk(0);
    if((char*)block + header->s.sz == prgmbrk){
        if(head == tail){
            head = tail = NULL;
        }
        else{
            tmp = head;
            while(tmp){
                if(tmp->s.next == tail){
                    tmp->s.next = NULL;
                    tail = tmp;
                }
                tmp = tmp->s.next;
            }
        }

        sbrk(0 - sizeof(header_t) - header->s.sz);
        pthread_mutex_unlock(&global_malloc_lock);
        return;
    }

    header->s.free = 1;
    pthread_mutex_unlock(&global_malloc_lock);
}

void* calloc(size_t num, size_t nsz){
    size_t sz;
    void* blk;

    if(!num || !nsz){
        return NULL;
    }

    sz = num * nsz;

    //Mult overflow check
    if(nsz != sz / num){
        return NULL;
    }

    blk = malloc(sz);
    if(!blk){
        return NULL;
    }

    memset(blk, 0, sz);
    return blk;
}

void* realloc(void* blk, size_t sz){
    header_t* header;
    void* ret;

    if(!blk || !sz){
        return malloc(sz);
    }

    header = (header_t*)blk - 1;
    if(header->s.sz >= sz){
        return blk;
    }

    ret = malloc(sz);
    if(ret){
        memcpy(ret, blk, header->s.sz);
        free(blk);
    }

    return ret;
}

