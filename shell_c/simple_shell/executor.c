/*115970458*/ 
	#include <stdio.h> 
	#include "command.h" 
	#include "executor.h" 
	#include <unistd.h> 
	#include <err.h> 
	#include <sys/types.h> 
	#include <sys/wait.h> 
	#include <string.h> 
	#include <stdlib.h> 
#include <sysexits.h> 
	#include <fcntl.h> 
	 
	#define MAX_LEN 1024 
	 
/*static void print_tree(struct tree *t);*/ 
int execute_aux(struct tree *t,int parent_i_fd,int parent_o_fd); 
	 
int execute(struct tree *t) { 
   /*print_tree(t);*/ 
   return execute_aux(t,STDIN_FILENO, STDOUT_FILENO); 
} 
	 
int execute_aux(struct tree *t,int parent_i_fd,int parent_o_fd){ 
	 
        pid_t pid; 
        int fd, status, error, input_fd, output_fd; 
        int pfd[2]; 
	 
        input_fd = parent_i_fd; 
        output_fd = parent_o_fd;  
	 
        if(t->conjunction == NONE){ 
                if(strcmp(t->argv[0],"cd") == 0){ 
                        if(t->argv[1] == NULL){ 
                                error = chdir(getenv("HOME")); 
                                if(chdir(getenv("HOME")) == -1){ 
                                        perror("Failed to execute cd"); 
                                        fflush(stdout); 
                                        return 1; 
                                }        
                        }else{ 
                                if(chdir(t->argv[1]) == -1){ 
                                        fprintf(stderr,"Failure to execute %s",t->argv[0]); 
                                        fflush(stdout); 
                                        return 1;        
                                }else{ 
                                        return 0;        
                                } 
                        } 
                }else if(strcmp(t->argv[0],"exit") == 0){ 
                        exit(0); 
                }else{ 
                        if((pipe(pfd)) < 0){ 
                                perror("pipe"); 
                        } 
                        if((pid = fork()) < 0){ 
                                perror("fork"); 
                        } 
 
                        if(pid == 0){ 
                                if(t->input != NULL){ 
                                        if((fd = open(t->input,O_RDONLY)) < 0){ 
                                                perror("File open failed"); 
                                        } 
                                        if(dup2(fd, STDIN_FILENO) < 0){ 
                                                err(EX_OSERR, "dup2 error"); 
                                        } 
                                        dup2(pfd[0],fd); 
                                } 
                                if(t->output != NULL){ 
                                        if((fd = open(t->output,O_RDONLY)) < 0){ 
                                                perror("File open error"); 
                                        } 
                                        if(dup2(fd,STDOUT_FILENO)<0){ 
                                                perror("dup2 error"); 
                                        } 
                                        dup2(pfd[1],fd); 
                                } 
                                if(close(pfd[0]) < 0){ 
                                        perror("close pipe"); 
                                } 
                                if(close(pfd[1]) < 0){ 
                                        perror("close pipe"); 
                                } 
                                execvp(t->argv[0],t->argv);      
                                 
                                fprintf(stderr,"Failed to execute %s", t->argv[0]); 
                                fflush(stdout); 
                                exit(EX_OSERR); 
                        }else{ 
                                if(close(pfd[1]) < 0){ 
                                        perror("close pipe"); 
                                } 
                                if(close(pfd[0]) < 0){ 
                                        perror("close pipe"); 
                                } 
 
                                wait(&status); 
                                if(!WIFEXITED(status)){ 
                                        fprintf(stderr,"Failed to execute %s", t->argv[0]); 
                                        fflush(stdout); 
                                        exit(EXIT_FAILURE); 
                                }else{ 
                                        if(WEXITSTATUS(status) == EXIT_SUCCESS){ 
                                                fflush(stdout); 
                                                return 0; 
                                        }else{ 
                                                fflush(stdout); 
                                                return EXIT_FAILURE; 
                                        }        
                                } 
                        } 
                } 
                fflush(stdout); 
                exit(EXIT_SUCCESS); 
        }else if(t->conjunction == PIPE){                  
                if(t->right->input != NULL){ 
                        printf("Ambiguous input redirect.\n"); 
                        fflush(stdout); 
                        return 0; 
                } 
                if(t->left->output != NULL){ 
                        printf("Ambiguous output redirect.\n"); 
                        fflush(stdout); 
                        return 0; 
                } 
 
                if((pipe(pfd)) < 0){ 
                        perror("pipe"); 
                }        
                if((pid = fork()) < 0){ 
                        perror("fork"); 
                } 
 
                if(pid == 0){ /* child process(left tree)*/ 
                        error = close(pfd[0]); 
                        if(error < 0){ 
                                perror("closing pipe"); 
                        } 
                        if(t->left->input != NULL){ 
                                if((fd = open(t->left->input,O_RDONLY)) < 0){ 
                                } 
                                if(dup2(fd,STDIN_FILENO)<0){ 
                                        perror("dup2 error"); 
                                } 
                                input_fd = fd; 
                        } 
 
                        dup2(pfd[1], output_fd); 
                        status = execute_aux(t->left,input_fd,output_fd); 
                        if(status != 0){  
                                exit(1); 
                        } 
                        error = close(pfd[1]); 
                        if(error < 0){ 
                                perror("closing pipe"); 
                        } 
                        exit(status); 
                }else{ /*parent process (right tree)*/ 
                        error = close(pfd[1]); 
                        if(error < 0){ 
                                perror("closing pipe"); 
                        } 
                        if(t->right->output != NULL){ 
                                if((fd = open(t->right->output,O_RDWR)) < 0){ 
                                        perror("File open error"); 
                                } 
                                if(dup2(fd,STDOUT_FILENO)<0){ 
                                        perror("dup2 error"); 
                                } 
                                input_fd = fd; 
                        } 
                        wait(&status); 
                        if(!WIFEXITED(status)){ 
                                perror("child process failure"); 
                        }else{ 
                                if(WEXITSTATUS(status) != EXIT_SUCCESS){ 
                                        status = 1; 
                                }else{ 
                                        status = 0; 
                                } 
                        } 
                        dup2(pfd[0],input_fd); 
                        status = execute_aux(t->right,input_fd,output_fd); 
                        if(close(pfd[0]) < 0){ 
                                perror("closing pipe"); 
                        } 
                        return status; 
                } 
 
        }else if(t->conjunction == AND){ 
 
                status = execute_aux(t->left,parent_i_fd,parent_o_fd); 
                if(status == 0){ 
                        status = execute_aux(t->right,parent_i_fd,parent_o_fd); 
                } 
                return status; 
 
        }else if(t->conjunction == SUBSHELL){ 
                         
                if((pid = fork()) < 0){ 
                        perror("fork"); 
                }        
 
                if(t->left->input != NULL){ 
                        if((fd = open(t->left->input,O_RDONLY)) < 0){ 
                                perror("File open failed"); 
                        } 
                        if(dup2(fd,STDIN_FILENO)<0){ 
                                perror("dup2 error"); 
                        } 
                        input_fd = fd; 
                } 
                if(t->left->output != NULL){ 
                        if((fd = open(t->left->output,O_RDONLY)) < 0){ 
                                perror("File open failed"); 
                        } 
                        if(dup2(fd,STDOUT_FILENO)<0){ 
                                perror("dup2 error"); 
                        } 
                        output_fd = fd; 
                } 
                                 
                if(pid == 0){ /*child process*/ 
 
                        status = execute_aux(t->left,input_fd,output_fd); 
                        exit(status); 
                 
                }else{ 
                        wait(&status); 
                        if(WIFEXITED(status)){ /*child finishes normally*/ 
                                if(WEXITSTATUS(status) == EXIT_SUCCESS){ 
                                        return 0;        
                                }else{ /*child returned failure*/ 
                                        return 1; 
                                }  
                        }else{ 
                                perror("child process failure"); 
                        } 
                } 
        } 
        return 0; 
} 
 
/*static void print_tree(struct tree *t) { 
   if (t != NULL) { 
      print_tree(t->left); 
 
      if (t->conjunction == NONE) { 
         printf("NONE: %s, ", t->argv[0]); 
      } else { 
         printf("%s, ", conj[t->conjunction]); 
      } 
      printf("IR: %s, ", t->input); 
      printf("OR: %s\n", t->output); 
 
      print_tree(t->right); 
   } 
}*/  
