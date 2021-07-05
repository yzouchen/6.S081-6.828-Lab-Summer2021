//sleep.c

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    if(argc == 1)
    {
        printf("You should input one argument\n");
        exit();
    }
    else if(argc > 2)
    {
        printf("You should not input more than one arguments\n");
        exit();
    }
    else
    {
        int count = atoi(argv[1]);
        if(count <= 0)
        {
            printf("Sleep time should be more than zero\n");
            exit();
        }
	printf("Sleep for a while...(%d)\n",count);        
	sleep(count);
        exit();
    }
}
