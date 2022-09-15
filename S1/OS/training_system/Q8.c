/*
8 Simple Pipe
Write a program that creates two processes. Create a pipe and send the pid of the parent process to the child process. Let the child and the parent print the pid of the parent.
*/

#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>

#define BUFSIZE 10

int main(){
    char bufin[BUFSIZE] = "";
    char bufout[BUFSIZE];
    int id = getpid();
    sprintf(bufout, "%d", id);
    int bytein, byteout;

    pid_t pid[2];
    int fd[2];
    pipe(fd);

    for(int i = 0; i < 2; i++){
        pid[i] = fork();
        if(pid[i] == 0){
            printf("[%d] Child -> %d", getpid(), byteout);
        }
    }




}
