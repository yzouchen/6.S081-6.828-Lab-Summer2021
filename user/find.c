#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


void find(char *path, char *name)
{
    //refer to ls.c
    char buf[512];
    char *p;
    int fd;
    struct dirent de;
    struct stat st;
    if((fd = open(path ,0)) < 0)   //open path error
    {
        printf("Error: cannot open %s\n", path);
        return;
    }
    if(fstat(fd, &st) < 0 || st.type != T_DIR)  //not DIR
    {
        printf("Error: the first arg should be dir path\n");
        close(fd);
        return;
    }

    while (read(fd, &de, sizeof(de)) == sizeof(de))       //sub-(dir)
    {
        //following some codes copy from ls.c
        if(de.inum == 0)
            continue;
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0){
            printf("ls: cannot stat %s\n", buf);
            continue;
        }
        if(st.type == T_FILE)
        {
            if(strcmp(name, de.name) == 0)        //an answer(file not dir)
                printf("%s\n",buf);     
        }
        else if(st.type == T_DIR)   //recursion
        {
            if(strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0)
            {
                find(buf,name); //recursion
            }
        }
    } 
    close(fd); 
}


int main(int argc, char *argv[]){
    if (argc < 3){
        printf("Usage: find [path] [filename]\n");
        exit();
    }
    find(argv[1], argv[2]);
    exit();
}