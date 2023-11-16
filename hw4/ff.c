#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>


struct block{//sizeof struct is 32
    size_t size;
    int free;
    struct block *prev;
    struct block *next;
};

void *first=0;
struct block *head=0;

void *malloc(size_t size){
    //printf("malloc\n");
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
        //align to 32
        size_t m=size/32;
        size_t n=size%32;
        size_t allosize;
        if(n==0){
            allosize=size;
        }else{
            allosize=32*(m+1);
        }
        ///////////////

        if(first==0){//first allocate memory pool
            first=mmap( NULL, 20000 , PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE , -1 , 0 );
            head=first;
            head->size=20000-32;//header:32
            head->free=1;//1:free 0:allocate
            head->prev=NULL;
            head->next=NULL;
        }

        //find free spot
        struct block *find;
        struct block *rhs;

        find=head;

        while(find!=NULL){
            if( find->free==1 && find->size >= allosize ){
                find->free=0;
                if((find->size) == allosize){//no need to handle rhs
                    return find+1;//return next struct
                }else{
                    //rhs
                    rhs=find+1+allosize/32;
                    rhs->size=(find->size)-(32+allosize);//header+data
                    rhs->free=1;
                    rhs->prev=find;
                    rhs->next=find->next;

                    //find->next to connect rhs
                    if(find->next!=NULL){//if not last
                        find->next->prev=rhs;
                    }

                    find->next=rhs; 
                    find->size=allosize;

                    return find+1; 
                }
            }
            find=find->next;
        }
    }
}


void free(void *ptr){//ptr=data
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