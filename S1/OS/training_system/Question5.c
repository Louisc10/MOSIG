/*
Write a program that executes n processes using fork(). Each child process should do a sleep()
followed by printing its own pid to stdout. Then each child exits. What do you have to take care
of to ensure that the program is terminated before you can start another program in the same
terminal?
*/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int N = 4;

int main(){
    pid_t pid[N];
    int i;
    for (i = 0; i < N; i++){
        if((pid[i] = fork()) == 0){
            exit(100+1);
            
        }
    }
}