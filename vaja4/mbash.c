#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>



#define BUFSIZE 1024
#define ARGSIZE 64
#define err_sys(mess) { fprintf(stderr,"Error: %s.", mess); exit(1); }

static void sig_usr(int); 
 void parse(char *niz_ukaz, char *args[]){
    int counter = 0;
    bool isChanged = true;
    for(int i = 0; niz_ukaz[i] != '\0'; i++){
        if(niz_ukaz[i] == ' ' || niz_ukaz[i] == '\t'){
            niz_ukaz[i] = '\0';
            isChanged = true;
            
        }else if(isChanged){
            args[counter++] = &niz_ukaz[i];
            isChanged = false;
        }
    }
    args[counter] = NULL;
 }

int countArgs(char *niz){
    int counter = 0;
    bool changed = true;
    for(int i = 0; niz[i] != '\0';i++){
        if(niz[i] == ' ' || niz[i] == '\t'){
            
            changed = true;
        }
        else if(changed){
            counter++;
            
            changed = false;
        }
    }

   
    return counter;
}

void parseHandler(char *niz_ukaz, char *args[]){
    int argc;
    if((argc = countArgs(niz_ukaz)) % 2 == 0){
        parse(niz_ukaz,args);

    }else{
        
        char *token = strtok(niz_ukaz," ");
        int i = 0;
        while(token != NULL){
            args[i++] = token;
            token = strtok(NULL, " ");
            
        }
        args[i] = NULL;

    }
}



void print_args(char *args[]) {
    int i = 0;
    while (args[i] != NULL) {
        printf("args[%d]: %s\n", i, args[i]);
        i++;
    }
    if (i == 0) {
        printf("No tokens found.\n");
    }
}

int main(){
    pid_t pid;
	char buf[BUFSIZE];
    char *args[ARGSIZE];

    if (signal(SIGUSR1, sig_usr) == SIG_ERR)
        err_sys("can't catch SIGUSR1")
    if (signal(SIGINT, sig_usr) == SIG_ERR)
        err_sys("can't catch SIGINT")
	
	printf("tit-shell:$ ");
	while ( fgets( buf, BUFSIZE, stdin) != NULL) {
		buf[strlen(buf)-1] = 0;
		
		if ( (pid = fork()) < 0){
			err_sys("fork");
            }
		else if ( pid == 0) {
            parseHandler(buf,args);
			execvp(args[0], args);
            
			printf("Ne morem izvesti ukaza ...\n");
			exit(127);
		}

		if ( waitpid(pid, NULL, 0) < 0)	
			perror("waitpid");
		printf("tit-shell:$ ");
	}    
    return 0;
}


static void sig_usr(int signo) // argument je št. signala
{
    if (signo == SIGUSR1)
        printf("received SIGUSR1\n");
    else if (signo == SIGINT)
        printf("received SIGINT\n");
    else
        printf("received signal %d\n", signo);
    return;
}