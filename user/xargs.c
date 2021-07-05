#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char * argv[])
{
    char temp_arg[MAXARG][64];
    char * full_argv[MAXARG];
    int i,j,len,space;
    char buf;

    if(argc < 2){
        printf("Error: xargs command\n");
        exit();
    }
    if(argc + 1 > MAXARG){
        printf("Error: too many args\n");
        exit();
    }

    for(i = 1 ; i < argc ; i++){        //copy
        strcpy(temp_arg[i-1] , argv[i]);
    }
    //temp_arg[argc-1] = 0;        //to make it can exec without extra arg

    while(1)       //Loop always
    {
        //in this loop i means the num of argv ; j means the char num in a argv
        i = 0;
        j = 0;
        space = 1;
        memset(temp_arg[argc-1], 0, (MAXARG-argc+1) * 64);      //clear
        while ((i+argc ) < MAXARG )
        {
            if((len = read(0, &buf, 1)) <= 0)       //read one char from stdin 
            {
                //ctrl + d    ------exit the whole
                wait();
                exit();
            }
            if(buf == '\n')         //arg end
            {  
                break;      //end read loop and exec
            }
            if(buf == ' ')          //"space"means next arg
            {
                //"space" is used to combine several space together
                if(space == 0)      //no space before
                {
                    i++;
                    j = 0;
                    space = 1;      //have space already
                }
                continue;
            }
            temp_arg[argc -1 + i][j] = buf;
            j++;
            space = 0;      //can swich argv again
        }

        for(j = 0;j < MAXARG; j++)
        {
            full_argv[j] = temp_arg[j];
        }

        full_argv[argc+i] = 0;

        //Use fork and exec and wait to finish every output. 
        if(fork() == 0) {    //child 
            exec(full_argv[0], full_argv);
            exit();
        }
        else{
            wait();
        }
    }
    exit();
}


//argv   0 1  2 3 4 5          argc-1
//full         0  1 2 3 4          argc-2