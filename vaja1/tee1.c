#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#define err() {char msg[] = "Napačna uporaba programa!\n";write(STDOUT_FILENO,msg,sizeof(msg)-1);return 1;}
#define BUFFSIZE 1024
int main(int argc, char *argv[]){
    if(argc > 3 || argc < 2){
        err()//preverimo pravilno število argumentov
    }
    if(strcmp(argv[argc-1],"-a") == 0){
        
        err();
    }
    int fd,n;
    char buf[BUFFSIZE];
    if(argc == 2){
        if((fd=open(argv[argc-1],O_APPEND | O_CREAT | O_WRONLY | O_TRUNC,0644))>0){
        while((n= read(STDIN_FILENO,buf,BUFFSIZE)) > 0){
            write(STDOUT_FILENO,buf, n);
            write(fd,buf,n);
            }
        close(fd);
        return 0;
        }
    }else if (strcmp(argv[1],"-a") == 0){
        if((fd=open(argv[argc-1],O_APPEND | O_CREAT | O_WRONLY,0644))>0){
        while((n= read(STDIN_FILENO,buf,BUFFSIZE)) > 0){
            write(STDOUT_FILENO,buf, n);
            write(fd,buf,n);
            }
        close(fd);
        return 0;
        }
    }else{
        err()
    }

    return 0;
}