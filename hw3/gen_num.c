#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    FILE *gen_num=fopen("input.txt","w");

    srand(time(NULL));

    int min=1;
    int max=100000;

    int n=1000000;

    fprintf(gen_num,"%d\n",n);

    for(int i=0;i<n;i++){
        int x=rand()%(max-min+1)+min;
        fprintf(gen_num,"%d ",x);
    }

    fclose(gen_num);
    return 0;
}
