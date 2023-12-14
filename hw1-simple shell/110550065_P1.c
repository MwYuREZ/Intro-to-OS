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
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

void sigchld_handler(int signo) {
    (void)signo; // Silence the "unused parameter" warning
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    }
}

int main() {
    //
    struct sigaction s;
    s.sa_handler = sigchld_handler;
    s.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &s, NULL);
    //
    
    char input[256];

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        int background = 0;
        if (input[strlen(input) - 1] == '&') {
            background = 1;
            input[strlen(input) - 1] = '\0';
        }

        // handle input
        char *args[64];
        int count = 0;
        char *token = strtok(input, " ");//remove blank and parse
        while (token != NULL) {
            args[count] = token;
            count++;
            token = strtok(NULL, " ");
        }
        args[count] = NULL;

        //handle bonus
        int pipe_pos=0;
        int redir_output=0;
        int redir_input=0;
        //find "|" ">" "<"
        for(int i=0;i<count;i++){
            if(strcmp(args[i],"|")==0){//if str1=str2 ,=0
                pipe_pos=i;
                break;
            }else if(strcmp(args[i],">")==0){
                redir_output=i;
                break;
            }else if(strcmp(args[i],"<")==0){
                redir_input=i;
                break;
            }
        }
        
        int fd[2];
        pipe(fd);
        
        pid_t pid = fork();

        if (pid < 0) { /* error occurred */
            fprintf(stderr, "Fork Failed\n");
            exit(-1);
        } else if (pid == 0) { /* Child process */
            

            //printf("%d\n",redir_output);
            if(pipe_pos>0){
   
                //pid_t first_childpid=fork();
                //sender
                    close(fd[0]);//close read
                    dup2(fd[1],STDOUT_FILENO);//將pipe寫入端取代成stdout
                    close(fd[1]);
                    args[pipe_pos]=NULL;
                    execvp(args[0],args);
                    
                    /*
                    char *args2[64];
                    int tmp=0;
                    args2[tmp]=args[pipe_pos+1];
                    tmp++;
                    for(int i=pipe_pos+2;i<count;i++){
                        args2[tmp]=args[i];
                        tmp++;
                    }
                    args2[tmp]=NULL;
                    execvp(args2[0],args2);*/
            }else if(redir_output>0){
                int out;
                out=creat(args[redir_output+1],0644);
                dup2(out,STDOUT_FILENO);
                close(out);
                args[redir_output]=NULL;
                execvp(args[0],args);
            }else if(redir_input>0){
                int in;
                in=open(args[redir_input+1],O_WRONLY | O_CREAT | O_TRUNC,0664);
                dup2(in,STDIN_FILENO);
                close(in);
                args[redir_output]=NULL;
                execvp(args[0],args);
            }else{
                execvp(args[0], args);
                exit(0);
            }
        } else { /* Parent process */
            if(pipe_pos>0){//handle pipe
                waitpid(pid,NULL,0);//wait sender
                pid=fork();
                if(pid==0){
                    close(fd[1]);//close write
                    dup2(fd[0],STDIN_FILENO);//將pipe讀取端取代成stdin
                    close(fd[0]);
                    execvp(args[pipe_pos+1],&args[pipe_pos+1]);
                }else{
                    close(fd[0]);
                    close(fd[1]);
                    waitpid(pid,NULL,0);
                }
            }else if(!background){//other case
                waitpid(pid, NULL, 0);
            }
        }
    }

    return 0;
}
