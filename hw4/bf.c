#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>


typedef struct block{//sizeof struct is 32
    size_t size;
    int free;
    struct block *prev;
    struct block *next;
}Block;

void *first=0;
struct block *head=0;


void *malloc(size_t size){
    if(size==0){
        //output max size
        struct block *tmp;
        tmp=head;
        size_t maxsize=0;
        while(tmp!=NULL){
            if(tmp->size > maxsize && tmp->free==1){
                maxsize=tmp->size;
            }
            tmp=tmp->next;
        }
        char output[50];
        sprintf(output,"Max Free Chunk Size = %ld\n",maxsize);
        write(1/*stdout*/,output,strlen(output));

        munmap(first,20000);
        return 0;
    }else{ 
        size_t m=size/32;
        size_t n=size%32;
        size_t allosize;
        if(n==0){
            allosize=size;
        }else{
            allosize=32*(m+1);
        }
        if(first==0){//first allocate memory pool
            first=mmap( NULL, 20000 , PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE , -1 , 0 );
            head=first;
            head->size=20000-32;//header:32
            head->free=1;//1:free 0:allocate
            head->prev=NULL;
            head->next=NULL;
        }
        
        struct block *find;
        struct block *best;

        find=head;
        //best=head;

        size_t threshold=20001;//must be update after first compare

        while(find!=NULL){//find best
            if(find->free==1&&find->size>=allosize){
                if(find->size<threshold){//the smallest plot can place
                    threshold=find->size;
                    best=find;//ans
                }
            }
            find=find->next;
        }

        struct block *rhs;

        best->free=0;
        if(best->size==allosize){
            return best+1;
        }else{
            rhs=best+1+allosize/32;
            rhs->size=(best->size)-(32+allosize);
            rhs->free=1;
            rhs->prev=best;
            rhs->next=best->next;

            if(best->next!=NULL){
                best->next->prev=rhs;
            }
            best->next=rhs;
            best->size=allosize;
            return best+1;
        }
    }
}



void free(void *ptr){//ptr = data 
    struct block *find;
    find=ptr;
    find=find-1;//to header place
    if(find->next!=NULL){//right
        if(find->next->free==1){//base: left side
            size_t newsize=find->size+find->next->size+32;//32=header
            find->size= newsize;//after combine
            if(find->next->next==NULL){
                find->next=NULL;
            }else{
                find->next->next->prev=find;
                find->next=find->next->next;
            }
        }
    }
    //base: right side => error! 
    if(find->prev!=NULL){//left
        if(find->prev->free==1){//base: left side
            size_t newsize=find->size+find->prev->size+32;
            find->prev->size=newsize;
            if(find->next!=NULL){
                find->prev->next=find->next;
                find->next->prev=find->prev;
            }else{//empty no need to connect back
                find->prev->next=find->next;
            }
        }
    }
    find->free=1;
}