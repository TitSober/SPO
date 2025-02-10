#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define stavek "lorem ipsem oaisjdlaksndlasjndlaskjdlak"
#define MIL5 5000000
#define BUFSIZE1 10
#define BUFSIZE2 100


int main(int argc, char *argv[]){
    char buffer[BUFSIZE1];
    char buffer2[BUFSIZE2];
    FILE *file1;
    FILE *file2;
    
    
    if((file1 = fopen("izhod1","w")) < 0){
        perror("error opening file 1");
        exit(-1);
    }
    if((file2 = fopen("izhod2","w")) < 0){
        perror("error opening file 2");
        exit(-2);
    }
    setvbuf(file1,buffer,BUFSIZE1,_IOFBF);


    clock_t t; 
    t = clock(); 
    for(int i = 0; i < MIL5; i++){
        fputs(stavek,file1);
    }
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
    printf("dat 1 took %f seconds to execute \n", time_taken); 
    
    setvbuf(file2,buffer2,BUFSIZE2,_IOFBF);
    t = clock(); 
    for(int i = 0; i < MIL5; i++){
        fputs(stavek,file2);
    }
    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
    printf("dat 2 took %f seconds to execute \n", time_taken); 
    


    fclose(file1);
    fclose(file2);
    return 0;   
}