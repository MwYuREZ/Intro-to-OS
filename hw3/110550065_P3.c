/*
Student No.: 110550065
Student Name: Mao Wei Yu
Email: zxcv9907800@gmail.com
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <string.h>

sem_t jobSemaphore;  
pthread_mutex_t jobMutex;
//pthread_mutex_t rdMutex;  
//sem_t mergeSemaphore[16];
//pthread_mutex_t arMutex;

int jobQueue[20][3];
int jobCount = 0;
int n;
int *array,*ar;
int ready[14];

void swap(int *a,int *b){
    int tmp=*a;
    *a=*b;
    *b=tmp;
    return;
}

void addJob(int task, int start, int end) {
    pthread_mutex_lock(&jobMutex);
    jobQueue[jobCount][0] = task;
    jobQueue[jobCount][1] = start;
    jobQueue[jobCount][2] = end;
    jobCount++;
    sem_post(&jobSemaphore);
    pthread_mutex_unlock(&jobMutex);
}

void getJob(int *task, int *start, int *end) {
    sem_wait(&jobSemaphore);
    pthread_mutex_lock(&jobMutex);
    *task = jobQueue[jobCount - 1][0];
    *start = jobQueue[jobCount - 1][1];
    *end = jobQueue[jobCount - 1][2];
    jobCount--;
    pthread_mutex_unlock(&jobMutex);
}

void bubblesort(int start,int end){
    for(int k=0;k<(end-start);k++){
        for(int i=0;i<(end-start)-k;i++){
            if(ar[start+i]>ar[start+i+1]){
                swap(&ar[start+i],&ar[start+i+1]);
            }
        }
    }
}

void merge(int start,int end){
    //printf("in\n");
    int size=(end-start+1);//each size
    int *left=(int*)malloc(sizeof(int)*size);
    int *right=(int*)malloc(sizeof(int)*size);
    //printf("here1\n");
    memcpy(left,ar+start,size*sizeof(int));
    memcpy(right,ar+start+size,size*sizeof(int));
    /*printf("%d %d\n",start,end);
    for(int i=0;i<size;i++){
        printf("%d ",left[i]);
    }        
    printf("\n");
    for(int i=0;i<size;i++){
        printf("%d ",right[i]);

    }    
    printf("\n");*/
    //printf("here2\n");
    int idxleft=0;
    int idxright=0;
    int tmp=start;
    while(idxleft<size&&idxright<size){
        if(left[idxleft]<right[idxright]){
            ar[tmp]=left[idxleft];
            idxleft++;
        }else{
            ar[tmp]=right[idxright];
            idxright++;
        }
        tmp++;
    }//printf("%d %d\n",idxleft,idxright);
    while(idxleft<size){
        ar[tmp]=left[idxleft];
        idxleft++;
        tmp++;
    }
    while(idxright<size){
        ar[tmp]=right[idxright];
        idxright++;
        tmp++;
    }
    /*for(int i=start;i<start+2*size;i++){
        printf("%d ",ar[i]);
    }printf("\n");*/
    free(left);
    free(right);
    //printf("out\n");
}
void *workerThread(){
    while(1){
        int jobnum,start,end;
        getJob(&jobnum,&start,&end);
        //printf("%d\n",jobnum);
        if(jobnum==15){
            break;
        }
        if(jobnum<8){
            //printf("%d\n",jobnum);
            bubblesort(start,end);
            //sem_post(&mergeSemaphore[jobnum]);
            //pthread_mutex_lock(&rdMutex);
            ready[jobnum]=1;
            //printf("bubble %d\n",jobnum);
            //pthread_mutex_unlock(&rdMutex);
        }else{
            //printf("%d\n",jobnum);
            merge(start,end);//傳入第一個陣列的開始位置及結束位置
            //sem_post(&mergeSemaphore[jobnum]);
            //pthread_mutex_lock(&rdMutex);
            ready[jobnum]=1;
            //printf("merge %d\n",jobnum);
            //pthread_mutex_unlock(&jobMutex);
        }
        /*for(int i=0;i<n;i++){
            printf("%d ",ar[i]);
        }
        printf("\n");*/
        /*for(int i=0;i<16;i++){
            int value;
            sem_getvalue(&mergeSemaphore[i],&value);
            printf("%d ",value);
        }
        printf("\n");*/
    }
}


int main(){
    FILE *input=fopen("input.txt","r");
    if(input==NULL){
        printf("Input Error\n");
        return 1;
    }

    //int n;
    fscanf(input,"%d",&n);
    array=(int*)malloc(sizeof(int)*n);

    for(int i=0;i<n;i++){
        fscanf(input,"%d",array+i);
    }

    fclose(input);

    for(int num_thread=1;num_thread<=8;num_thread++){

        struct timeval start1,end1;
        gettimeofday(&start1,0);
        
        //printf("%d\n",num_thread);

        ar=(int*)malloc(sizeof(int)*n);
        memcpy(ar,array,sizeof(int)*n);


        sem_init(&jobSemaphore,0,0);
        pthread_mutex_init(&jobMutex,NULL);
        //pthread_mutex_init(&rdMutex,NULL);

        

        for(int i=0;i<16;i++){//initialize sema
            ready[i]=0;
        }
        int frac=n/8;
        for(int i=0;i<8;i++){
            addJob(i,frac*i,frac*(i+1)-1);//8 bubblesort
        }

        pthread_t pthreads[num_thread];
        for(int i=0;i<num_thread;i++){
            pthread_create(&pthreads[i],NULL,workerThread,NULL);
        }
        while(1){
            //printf("in here\n");
            //1st
            for(int i=0;i<=6;i=i+2){
                //sem_wait(&mergeSemaphore[i]);
                //sem_wait(&mergeSemaphore[i+1]);
                if(ready[i]==1&&ready[i+1]==1){
                    addJob(8+(i/2),frac*i,frac*(i+1)-1);
                    ready[i]=0;ready[i+1]=0;
                }
            }
            //2nd
            for(int i=0;i<2;i++){
                //sem_wait(&mergeSemaphore[8+2*i]);
                //sem_wait(&mergeSemaphore[8+2*i+1]);
                if(ready[8+2*i]==1&&ready[8+2*i+1]==1){
                    addJob(12+i,4*i*frac,(4*i+2)*frac-1);
                    ready[8+2*i]=0;ready[8+2*i+1]=0;
                }
            }
            //3rd
            //sem_wait(&mergeSemaphore[12]);
            //sem_wait(&mergeSemaphore[13]);
            if(ready[12]==1&&ready[13]==1){
                addJob(14,0,(frac*4)-1);
                ready[12]=0;ready[13]=0;
            }
            //sem_wait(&mergeSemaphore[14]);
            //no job 
            if(ready[14]==1){
                addJob(15,4,5);
                break;
                //ready[14]=0;
            }
        }

        /*for(int i=0;i<n;i++){
            printf("%d ",ar[i]);
        }printf("\n");*/
        /*for(int i=0;i<num_thread;i++){
            pthread_join(pthreads[i],NULL);
        }*/
        gettimeofday(&end1,0);
        int sec=end1.tv_sec-start1.tv_sec;
        int usec=end1.tv_usec-start1.tv_usec;

        printf("worker thread #%d, elapsed %f ms\n",num_thread,sec*1000+(usec/1000.0));
        char outputName[15];
        sprintf(outputName,"output_%d.txt",num_thread);
        FILE *output=fopen(outputName,"w");
        for(int i=0;i<n;i++){
            fprintf(output,"%d ",ar[i]);
        }
        fclose(output);
        free(ar);
    }
    free(array);
}