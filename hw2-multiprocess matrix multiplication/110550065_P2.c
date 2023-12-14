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
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>

int main(){
    int dim;
    printf("Input the matrix dimension:");
    scanf("%d",&dim);
    int matA=shmget(0,3000000,IPC_CREAT|0600);
    int matC=shmget(0,3000000,IPC_CREAT|0600);
    unsigned int *A=(unsigned int *)shmat(matA,NULL,0);
    unsigned int *C=(unsigned int *)shmat(matC,NULL,0);
    //init A
    for(int i=0;i<dim*dim;i++){
        A[i]=i;
    }
    struct timeval start,end;
    pid_t pid;
    for(int process=1;process<=16;process++){
        printf("Multiplying matrices using %d process\n",process);
        //init C
        for(int i=0;i<dim*dim;i++){
            C[i]=i;
        }
        int base=dim/process;
        int more=dim%process;
        gettimeofday(&start,0);
        for(int forktime=1;forktime<=process;forktime++){
            pid=fork();
            if(pid==0){//child process
                if(forktime<=more){
                    int begin=(forktime-1)*(base+1);
                    for(int i=begin;i<begin+(base+1);i++){
                        for(int j=0;j<dim;j++){
                            unsigned int sum=0;
                            for(int k=0;k<dim;k++){
                                sum+=A[i*dim+k]*A[k*dim+j];
                            }
                            C[i*dim+j]=sum;
                        }
                    }
                }else{
                    int begin=(base+1)*more+base*((forktime-1)-more);
                    for(int i=begin;i<begin+base;i++){
                        for(int j=0;j<dim;j++){
                            unsigned int sum=0;
                            for(int k=0;k<dim;k++){
                                sum+=A[i*dim+k]*A[k*dim+j];
                            }
                            C[i*dim+j]=sum;
                        }
                    }
                }
                exit(0);
            }
        }
        signal(SIGCHLD,SIG_IGN);
        wait(NULL);
        gettimeofday(&end,0);
        int sec=end.tv_sec-start.tv_sec;
        int usec=end.tv_usec-start.tv_usec;
        unsigned int csum=0;
        for(int i=0;i<dim*dim;i++){
            csum+=C[i];
        }
        printf("Elapsed time:%fsec,Checksum:%u\n",sec+(usec/1000000.0),csum);
    }
    shmctl(matA,IPC_RMID,NULL);
    shmctl(matC,IPC_RMID,NULL);
    shmdt(A);
    shmdt(C);
    return 0;
}