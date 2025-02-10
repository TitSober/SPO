
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <stdint.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/wait.h>


#include <sys/ioctl.h>
#include <net/if.h>


#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }
int PORTS[] = {514,585,623,1645,8811,9876,12345,65535};

#define MAXLINE 1024
#define LISTENQ 2




void SIGCHLD_handler(int sig){
    // Set up the SIGCHLD handler to reap zombie processes
   
    // Log when the handler is triggered
    printf("SIGCHLD handler triggered\n");
    // Reap all dead child processes
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        // Log each time a child process is successfully reaped
        printf("Child process reaped\n");
    }

}

int main(int argc, char *argv[])
{
	
	int n;
	int listenfd, connfd;
	struct sockaddr_in servaddr, cliaddr;
	unsigned short Port;
    char buf[MAXLINE];
	
	socklen_t len;
	char ip[] = "127.0.0.1";
	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err("socket")
	
	
	int check;
    int i = 0;
    do{ //do while loop checking for open predefined ports 
        Port = PORTS[i];
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
	    servaddr.sin_port = htons(Port);
		//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        //printf("Trying port %d\n", Port);
        if ( inet_pton(AF_INET, ip, &servaddr.sin_addr.s_addr) <= 0)
		    err("pton")
        i++;
    }while( i < 8 && (check = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0);
	if (check < 0)
        err("bind")
	printf(". Listening on port %d\n", Port);
	
	
	
	if ( listen(listenfd, LISTENQ) < 0)
		err("listen")

  
    int pid;
	while (1) {	
		len = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) < 0)
			err("accept")
        if((pid = fork()) < 0){
            err("fork");   
        }
        if(pid == 0){
            // child dela tukaj
            if(recv(connfd,buf,MAXLINE,0) < 0){
                err("recv");
            }
            int nit = atoi(buf);
            pid_t mypid = getpid();
            printf("Otrok %d streznika sem stregel niti %d odjemalca (%s:%d)\n", mypid, nit, inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
            if(mypid%nit == 0){
                snprintf(buf,MAXLINE,"Nit %d čestitam dobili ste nagrado\n\0",nit);
            }else{
                snprintf(buf,MAXLINE,"Nit %d več sreče drugič\n\0",nit);
            }
           send(connfd,buf,strlen(buf),0);
           send(connfd,"\0",1,0);
           kill(mypid,SIGCHLD);
           close(connfd);



        }else{
            printf(".");
            //parent
          signal(SIGCHLD, SIGCHLD_handler);

        }

		

		
	}
}

