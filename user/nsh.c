//my simple shell

#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MAXARGS 10
#define MAXLEN 100

int getcmd(char *buf, int nbuf);
void parse_cmd(char * cmd , int * argc_p , char ** argv);
void run_cmd(int argc, char ** argv);
void panic(char *s);
int fork1(void);
void run_pipe(int argc, char ** argv, int index);

//main process : getcmd--parse_cmd--run_cmd
int main(void)
{
    char buf[MAXLEN];
    int argc;
    char* argv[MAXARGS];
    while(getcmd(buf, sizeof(buf)) >= 0)
    {
        if (fork() == 0)
        {
            parse_cmd(buf, &argc, argv);    //parse cmd
            run_cmd(argc, argv);        //run cmd
        }
        wait(0);
    }
    exit(0);
}

//function getcmd refer from sh.c
int getcmd(char *buf, int nbuf)
{
    fprintf(2, "@");                  //difference to sh.c
    memset(buf, 0, nbuf);
    gets(buf, nbuf);
    if(buf[0] == 0) // EOF
        return -1;
    return 0;
}

void parse_cmd(char * cmd , int * argc_p , char ** argv)
{
    char whitespace[] = " \t\r\n\v";        //refer to sh.c
    int i = 0;  //num of cmd
    int j = 0;  //num of arg
    int len = strlen(cmd);
    while(i < len && cmd[i])    //not end
    {
        while(i < len && strchr(whitespace,cmd[i])){   //pass the whitespace
            i++;
        }
        if(i < len)
        {
            argv[j++] = cmd + i;    //start of an arg
        }
        while(i < len && !strchr(whitespace, cmd[i])){      //not yet to whitespce---continue
            i++;
        }
        cmd[i++] = 0;   //end of an arg
    }
    argv[j] = 0; //end of argv
    *argc_p = j;
}

void run_cmd(int argc, char ** argv)
{
    int i;
    for(i = 1 ; i < argc ; i++)
    {
        if(!strcmp(argv[i], "|"))
        {
            run_pipe(argc, argv, i);
        }
    }
    for(i = 1 ; i < argc ; i++)
    {
        if(!strcmp(argv[i], ">"))       //process >
        {
            close(1);   //close stdout
            open(argv[i+1], O_WRONLY|O_CREATE);
            argv[i] = 0;    //exec argv[0]---argv[i-1]
        }
        if(!strcmp(argv[i], "<"))       //process <
        {
            close(0);   //close stdin
            open(argv[i+1],O_RDONLY);
            argv[i] = 0;   //exec argv[0]---argv[i-1]
        }
    }
    exec(argv[0],argv);
}

//func panic refer to sh.c
void panic(char *s)
{
  fprintf(2, "%s\n", s);
  exit(-1);
}

//func fork1 refer to sh.c 
int fork1(void)
{
  int pid;
  pid = fork();
  if(pid == -1)
    panic("fork");
  return pid;
}

void run_pipe(int argc, char ** argv, int index)
{
    int p [2];
    argv[index] = 0;
    if(pipe(p) < 0)
        panic("pipe");
    if(fork1() == 0)    //child process exec left cmd
    {
        close(1);   //close stdout 
        dup(p[1]);      //1 as p[1]
        close(p[0]);
        close(p[1]);
        run_cmd(index,argv);    //output to 1 as p[1]
    }
    else    //parent process exec right cmd
    {
        close(0);       //close stdin
        dup(p[0]);      //0 as p[0]
        close(p[0]);
        close(p[1]);
        run_cmd(argc-index-1 , argv+index+1);       //p[1] as input
    }
} 