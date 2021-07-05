#include "kernel/types.h"
#include "user/user.h"

#define READ 0
#define WRITE 1

void primes(int p_parent[2])
{
    int n,prime;
    int p_child[2];
    close(p_parent[WRITE]);
    if((read(p_parent[READ], &prime, sizeof(int))) == 0)    //read nothing
    {
        close(p_parent[READ]);
        exit();
    }
    else
    {
        printf("prime %d\n",prime);
        pipe(p_child);
        if(fork() == 0)     //child's  child
        {
            close(p_parent[READ]);
            primes(p_child);
        }
        else    //child as parent
        {
            close(p_child[READ]);
            while((read(p_parent[READ] , &n , sizeof(int))) != 0){
                if((n % prime) != 0)
                    write(p_child[WRITE] , &n , sizeof(int));
            }
            close(p_parent[READ]);
            close(p_child[WRITE]);
            wait();
        }
    }
    exit();
}


int main (int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("Error: call error\n");
        exit();
    }
    int n;
    int p_parent[2];
    pipe(p_parent);
    if(fork() == 0)     //child
    {
        primes(p_parent);      //pass the pipe
    }
    else    //parent first
    {
        close(p_parent[READ]);
        for(n = 2 ; n <= 35 ; n++){
            write(p_parent[WRITE], &n ,sizeof(int));
        }
        close(p_parent[WRITE]);
        wait();
    }
    exit();
}