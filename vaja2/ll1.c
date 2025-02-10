#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>

void print_permissions(mode_t mode) {
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

int main(int argc, char *argv[]){
    DIR *dp;
    struct dirent *dirp;
    struct stat statbuf;
    if((dp = opendir(".")) == NULL){
        perror("cant open dir");
        return 1;
    }
    while((dirp = readdir(dp)) != NULL){
        if(stat(dirp->d_name, &statbuf) == -1){
            perror("statError");
            continue;
        }
       
        print_permissions(statbuf.st_mode);
        printf(" %lu",statbuf.st_nlink);
        printf(" %s\n",dirp->d_name);
    }
    closedir(dp);
    
    return 0;
}