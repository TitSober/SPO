#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



#define BUFFER 1
#define err() {char msg[] = "Napačna uporaba programa\n";write(STDOUT_FILENO,msg,sizeof(msg)-1);return 1;}

#define offset() {if(offsetPos == -1){char errmsg[] = "seek failed\n";write(STDERR_FILENO,errmsg,sizeof(errmsg) -1);return -1;}}
int main(int argc, char* argv[]){
        int fd;
        int n = 5; // default število vrstic prebranih od dna
        if(argc > 4 || argc < 2){ //preverja število argumentov podanih v klic programa.
            err()
        }
        if(argc == 4 && strcmp(argv[1],"-n") == 0 && atoi(argv[2]) > 0){
            n = atoi(argv[2]);
        }
        if((fd = open(argv[argc-1], O_RDONLY)) > 0){
            off_t offsetPos = lseek(fd,-1,SEEK_END);
            offset()

            char buf[BUFFER];
            while(n > 0){
               
                offsetPos = lseek(fd, -1, SEEK_CUR); 
                if(offsetPos == -1){
                    lseek(fd,0,SEEK_SET);
                    break;
                }
                if (read(fd, buf, BUFFER) != BUFFER) {
                    char readerr[] = "file reading failed";
                    write(STDERR_FILENO,readerr,sizeof(readerr) -1);
                    close(fd);
                    return -1;
                }
                if(buf[0] == '\n'){
                    n--;
                }
                lseek(fd, -1, SEEK_CUR);
            }
           
            while((read(fd,buf,BUFFER)) != 0){
                write(STDOUT_FILENO,buf,1);
            }
            close(fd);
        }else{
            err()
        }

    return 0;
}