/* Implement your shell here */
#include <sys/wait.h>
#include <stdio.h>
#include <sysexits.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(){
        pid_t pid;
        const char *command = NULL;
        char * argv[3] = {NULL,NULL,NULL};
        char line[1024], first[1024], second[1024];
        int count = 0, status = 0;

        command = fgets(line,1024,stdin);

        printf("shell_jr: ");
        fflush(stdout);  while(command != NULL){
                count = sscanf(command,"%s %s",first,second);

                if(count >= 1){
                        argv[0] = first;
                }
                if(count == 2){
                        argv[1] = second;
                }

                if(strcmp(argv[0],"exit") == 0){
                        printf("See you\n");
                        exit(0);
                }else if(strcmp(argv[0],"hastalavista") == 0){
                        printf("See you\n");
                        exit(0);
                }else if(strcmp("cd",argv[0]) == 0 ){
                        status = chdir(argv[1]);
               
                        if(status == -1){
                                printf("Cannot change to directory %s\n",argv[1]);
                                fflush(stdout);
                        }
                }else{
                        pid = fork();

                        if(pid < 0){
                                err(EX_OSERR, "fork error");
                        }else if(pid == 0){
                                execvp(argv[0],argv);

                                printf("Failed to execute %s\n",argv[0]);
                                fflush(stdout);
                                exit(EX_OSERR);
                        }else{
                                wait(NULL);
                        }
                }strcpy(argv[0],"");
                if(count == 2){
                        strcpy(argv[1],"");
                }
                command = fgets(line,1024,stdin);  

                printf("shell_jr: ");
                fflush(stdout);
        }
return 0;
}
                
