#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

#define MAXLINE 1024
void connectToServ(int args[]){
    int sockfd;
	struct sockaddr_in servaddr;
	char buf[MAXLINE];
    unsigned short Port;;
    

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err("socket")
    int check;
   
   
    Port = args[0]; 
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(Port);
        //printf("Trying port %d\n", Port);
    if ( inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr.s_addr) <= 0)
	    err("pton")
 
    check = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    
    if (check < 0)
        err("connect")
    
    snprintf(buf, MAXLINE,"%d",Port);
    send(sockfd, buf, strlen(buf), 0);

    while(recv(sockfd, buf, MAXLINE, 0) > 0){
        printf("%s\n",buf);
    }
    close(sockfd);
    

}

int main(int argc, char *argv[])
{	
	if(argc < 3){
		printf("Usage: %s  <PORT> <NUMBER OF THREADS>\n", argv[0]);
		err("Usage")
	}

    int n = atoi(argv[2]);
    int port = atoi(argv[1]);
    
	
    
    pthread_t threads[n];


    for(int i = 0; i < n; i++){
        int args[2] = {port, i};
        pthread_create(&threads[i],NULL,(void *)connectToServ,(void *)args);
        printf("Created thread %d\n", i);
    }

    for(int i = 0; i < n; i++){
        pthread_join(threads[i],NULL);
    }
	
    printf("\n\n\n\nClient done\n\n\n\n");
	return 0;
}