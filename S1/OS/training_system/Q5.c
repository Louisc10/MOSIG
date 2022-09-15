/*
5 Fork/Wait
Write a program that executes n processes using fork(). Each child process should do a sleep()
followed by printing its own pid to stdout. Then each child exits. What do you have to take care
of to ensure that the program is terminated before you can start another program in the same
terminal?
*/

#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>

int N = 4;

int main(){
    pid_t pid[N];
    int i;
    for (i = 0; i < N; i++){
        if((pid[i] = fork()) == 0){
            sleep(3);
            printf("%d\n", getpid());
            exit(100+i);
        }
    }
    int child_status;
    for (i = 0; i < N; i++) {
        pid_t wpid = waitpid(pid[i], &child_status, 0);
        if (WIFEXITED(child_status))
            printf("Child %d is terminated %d\n", wpid, WEXITSTATUS(child_status));
        else
            printf("Child %d terminated abnormally\n", wpid);
    }
}