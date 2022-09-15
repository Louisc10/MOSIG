/*
7 Reading/Writing to a file
Write a C program that performs a copy of file1.txt (created previously) into a new file
file_copy.txt. To solve this exercise, use system calls: open(), close(), read(), write(), etc.
*/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h> 
#include<unistd.h>

char *PATH_1 = "./excercise4/file1.txt";
char *PATH_2 = "./excercise4/file_copy.txt";
int main(){
    int fd, fd1;
    int nbytes;
    char buf[512];
    
    puts("Its started");

    //read
    if((fd = open(PATH_1, O_RDONLY)) < 0){
        exit(-1);
    }
    puts("Its Get the File");

    if((nbytes = read(fd, buf, sizeof(buf))) < 0){
        exit(2);
    }
    puts("Its Get the Text");
    printf(">>%s<<\n", buf);

    if (close(fd) < 0) 
    { 
        perror("c1"); 
        exit(1); 
    } 

    //write
    if((fd1 = open(PATH_2, O_WRONLY | O_CREAT, 0666)) < 0){
        exit(3);
    }
    puts("Its Open the File");

    if((nbytes = write(fd1, buf, nbytes)) < 0){
        exit(4);
    }
    puts("Its Write the File");

    if (close(fd1) < 0) 
    { 
        perror("c1"); 
        exit(-1); 
    } 
    puts("Its the end");

    return 0;
}