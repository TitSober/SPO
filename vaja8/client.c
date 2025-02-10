#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <time.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

#define MAXLINE 1024
int PORTS[] = {514,585,623,1645,8811,9876,12345,65535};

int main(int argc, char *argv[])
{	
	if(argc < 2){
		printf("Usage: %s <IP>\n", argv[0]);
		err("Usage")
	}

	int n;
	int sockfd;
	struct sockaddr_in servaddr;
	char buf[MAXLINE];
    unsigned short Port;
    //unsigned short Port = (argc == 2)? atoi(argv[1]): 13;

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err("socket")
    int check;
    int i = 0;
    do{ //do while loop checking for open predefined ports 
        Port = PORTS[i];
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
	    servaddr.sin_port = htons(Port);
        //printf("Trying port %d\n", Port);
        if ( inet_pton(AF_INET, argv[1], &servaddr.sin_addr.s_addr) <= 0)
		    err("pton")
        i++;
    }while( i < 8 && (check = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0);
    
    if (check < 0)
        err("connect")

    //printf("Port %d is open\n", Port);
	char input[2];
	while (1) {
		while ((n = read(sockfd, buf, MAXLINE)) > 0) {
			if(buf[0] == '>') {
				//printf("GOT Z \n");
				scanf("%1s", input);
				sleep(2);
				if (send(sockfd, input, 1, 0) < 0) err("send");
			}else{
				buf[n-1] = '\0';
				puts(buf);
				//printf("loop\n");
			}
			
			
		}
	}



	if ( close(sockfd) < 0)
		err("close")

	return 0;
}