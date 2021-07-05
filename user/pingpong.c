//pingpong.c

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READ 0
#define WRITE 1

int main()
{
    int parent_fd[2];
    int child_fd[2];
    int pid;
    pipe(parent_fd);
    pipe(child_fd);
    char p2c[] = "V";       //parent 2 child one byte
    int n_p2c = sizeof(p2c);
    char c2p[] = "X";       //child 2 parent one byte
    int n_c2p = sizeof(c2p);
    if (pipe(parent_fd) < 0 || pipe(child_fd)<0)
    {
        printf("Error: pipe error\n");
        exit();
    }
    else if ((pid = fork()) < 0)
    {
        printf("Error: fork error\n");
        exit();
    }
    else if (pid == 0)   //  in child
    {
        close(parent_fd[WRITE]);
        close(child_fd[READ]);
        if(write(child_fd[WRITE],c2p,n_c2p) != n_c2p)
        {
            printf("Error: child write into pipe error\n");
            exit();
        }
        if(read(parent_fd[READ],c2p,n_c2p) != n_c2p)
        {
            printf("Error: parent read from pipe error\n");
            exit();
        }
        printf("received ping\n");
        exit();  
    }
    else        //  in parent
    {
        close(parent_fd[READ]);
        close(child_fd[WRITE]);
        if(write(parent_fd[WRITE],p2c,n_p2c) != n_p2c)
        {
            printf("Error: parent write into pipe error\n");
            exit();
        }
        if(read(child_fd[READ],p2c,n_p2c) != n_p2c)
        {
            printf("Error: child read from pipe error\n");
            exit();
        }
        printf("received pong\n");
        //wait();
        exit();
    }
}