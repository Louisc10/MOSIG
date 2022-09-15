/*
6 Fork/Exec
Write a program that creates 2 processes. The second process should be created via fork(). After the fork, the child should execute (via execve or another primitive of the same family - see man 2 execve and man 3 exec) the binary /bin/ls. The parent process should print a "hello world" message after forking. How many "hello world" messages will be printed?
*/
#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>

int main(){
    pid_t pid1 = fork();
    if(pid1 == 0){
        pid_t pid2 = fork();
        int res;
        if(pid2 == 0){
            char *args[] = {"/bin/ls", NULL};
            res = execv(args[0], args);
        }
    }
    else{
        printf("Hello World");
    }

}