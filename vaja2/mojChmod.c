#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_permissions(mode_t mode) {
    printf("Nova dovoljenja: ");
    if (S_ISREG(mode))
        printf("-");
    else if (S_ISDIR(mode))
        printf("d");
    else if (S_ISCHR(mode))
        printf("c");
    else if (S_ISBLK(mode))
        printf("b");
    else if (S_ISFIFO(mode))
        printf("p");
    else if (S_ISLNK(mode))
        printf("l");
    else if (S_ISSOCK(mode))
        printf("s");
    
    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    if (mode & S_ISUID)
        printf( (mode & S_IXUSR) ? "s" : "S");
    else
        printf( (mode & S_IXUSR) ? "x" : "-");

    
    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    if (mode & S_ISGID)
        printf( (mode & S_IXGRP) ? "s" : "S");
    else
        printf( (mode & S_IXGRP) ? "x" : "-");

    
    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    if (mode & S_ISVTX)
        printf( (mode & S_IXOTH) ? "t" : "T");
    else
        printf( (mode & S_IXOTH) ? "x" : "-");
}

//nacin parsanja besednih ukazov
mode_t returnModeVerbal(char verb[],mode_t prevMode){
    mode_t mode = prevMode;
    if (strcmp(verb, "u+s") == 0) {
        mode |= S_ISUID;  
    } else if (strcmp(verb, "u-s") == 0) {
        mode &= ~S_ISUID; 
    } else if (strcmp(verb, "g+s") == 0) {
        mode |= S_ISGID; 
    } else if (strcmp(verb, "g-s") == 0) {
        mode &= ~S_ISGID;
    } else {
        perror("Neveljaven niz za spreminjanje dovoljenj");
        return -1;
    }
    return mode;

}

//nacin parsanja številčnih ukazov
mode_t returnModeOctal(char oct[]){
    mode_t mode = strtol(oct,NULL,8);
    return mode;

}


int main(int argc, char *argv[]){
    if(argc > 3 || argc < 3){
        perror("Nepravilna uporaba programa");
        return 1;
    }
    struct stat statbuf;
    const char *filename = argv[2];
    mode_t prevMode = stat(filename,&statbuf); 
    mode_t new_mode;
    if(prevMode < 0){
        perror("stat error");
    }
    if(argv[1][0] >= '0' && argv[1][0] <= '7'){
        new_mode = returnModeOctal(argv[1]);
        if((chmod(filename,new_mode)) < 0){
            perror("chmod error");
            return 1;
        }
        if(stat(filename,&statbuf) < 0){
            perror("stat error new");
        }
        print_permissions(statbuf.st_mode);
        return 0;


    }else{
        new_mode = returnModeVerbal(argv[1],prevMode); 
        if((chmod(filename,new_mode)) < 0){
            perror("chmod error");
            return 1;
        }
        if(stat(filename,&statbuf) < 0){
            perror("stat error new");
        }
        print_permissions(statbuf.st_mode);
        return 0;
    }

    
    
    
    return 0;
}