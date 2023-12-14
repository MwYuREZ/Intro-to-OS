/*
Student No.: 110550065
Student Name: Mao Wei Yu
Email: zxcv9907800@gmail.com
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not supposed to be posted to a
public server, such as a public GitHub repository or a public web page.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

//#define _BSD_SOURCE

#define PATH_MAX_LENGTH 4096

struct{
        char hash_string[SHA_DIGEST_LENGTH*2+1];
        char file_path[PATH_MAX_LENGTH];
}hash_map[960];

int check_max=0;

void digest_sha1(const char *path,unsigned char *hash){
    FILE *file=fopen(path,"rb");
    //printf("compute : %s\n",path);
    if(file==NULL){
        printf("file does not exist\n");
        return;
    }

    SHA_CTX ctx;
    SHA1_Init(&ctx);

    int c;
    while((c=fgetc(file))!=EOF){
        unsigned char tmp=(unsigned char) c;
        SHA1_Update(&ctx,&tmp,1);
    }
    // ssize_t bytes;
    // unsigned char data[1024];
    // while((bytes=fread(data,1,1024,file))!=0){
    //     SHA1_Update(&ctx,data,bytes);
    // }


    SHA1_Final(hash,&ctx);
    fclose(file);
}

void solve(const char *tar_dir_path){
    DIR *dir=opendir(tar_dir_path);
    if(dir==NULL){
        //printf("no such dir\n");
        return;
    }

    struct dirent *target;
    while((target=readdir(dir))!=NULL){
        if(strcmp(target->d_name,".")!=0&&strcmp(target->d_name,"..")!=0){
            char entry_path[PATH_MAX_LENGTH];
            snprintf(entry_path,sizeof(entry_path),"%s/%s",tar_dir_path,target->d_name);
            //printf("%s\n",entry_path);
            if(target->d_type==DT_REG){//file
                //printf("in file\n");
                //printf("%s\n",target->d_name);
                unsigned char hash[SHA_DIGEST_LENGTH];
                digest_sha1(entry_path,hash);

                //////////2 to 16
                char hash_string[SHA_DIGEST_LENGTH*2 +1];
                hash_string[SHA_DIGEST_LENGTH*2]='\0';
                for(int i=0;i<SHA_DIGEST_LENGTH;i++){
                    sprintf(&hash_string[i*2],"%02x",hash[i]);
                }

                // if((strcmp(entry_path,"./test2/dir2/c.txt"))==0){
                //     printf("%s\n",entry_path);
                //     printf("%s\n",hash_string);
                // }
                // if((strcmp(entry_path,"./test2/dir1/col4.txt"))==0){
                //     printf("%s\n",entry_path);
                //     printf("%s\n",hash_string);
                // }

                ///////compare
                // char check_path[PATH_MAX_LENGTH];
                // snprintf(check_path,sizeof(check_path),"%s%s",tar_dir_path,hash_string);

                // struct stat buf;
                // if(lstat(check_path,&buf)==0){
                //     unlink(entry_path);
                //     link(check_path,entry_path);
                // }


                int exist=-1;
                for(int i=0;i<check_max;i++){
                    if(strcmp(hash_map[i].hash_string,hash_string)==0){
                        exist=i;
                        break;
                    }
                }

                if(exist!=-1){
                    unlink(entry_path);
                    link(hash_map[exist].file_path,entry_path);
                    //printf("target:%s\n",hash_map[exist].file_path);
                    //printf("victim:%s\n",entry_path);
                    //printf("%s\n",hash_string);
                }else{
                    strcpy(hash_map[check_max].hash_string,hash_string);
                    strcpy(hash_map[check_max].file_path,entry_path);
                    check_max++;
                }

                // for(int k=0;k<check_max;k++){
                //     if((strcmp(hash_map[k].hash_string,"0a0a9f2a6772942557ab5355d76af442f8f65e01"))==0){
                //         printf("find\n");
                //         printf("path:%s\n",hash_map[k].file_path);
                //     }
                // }
            }else if(target->d_type==DT_DIR){
                solve(entry_path);
            }  
        }
         
    }
    closedir(dir);
}

int main(int argc,char **argv){
    //printf("in\n");
    solve(argv[1]);
}