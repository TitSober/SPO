
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


#include <sys/ioctl.h>
#include <net/if.h>


#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }
int PORTS[] = {514,585,623,1645,8811,9876,12345,65535};
char CARDS[] = {'A','2','3','4','5','6','7','8','9','T','J','Q','K'};
int VALUES[] = {1,2,3,4,5,6,7,8,9,10,10,10,10};


#define MAXLINE 1024
#define LISTENQ 2



char *getIP(){
    int fd;
    struct ifreq ifr;
    char ip[40];

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, "wlp1s0", IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);

    /*return (char *)*/inet_ntop(AF_INET, &((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr, ip, sizeof(ip));
    printf("IP address: %s\n", ip);
    return strdup(ip);
	

    //return ip;
}



void blackJack(int sockfd) {
    int cards[] = {4,4,4,4,4,4,4,4,4,4,4,4,4};
    srand(time(NULL));
    char buf[MAXLINE];

    int randomCard;
    int dealerPoints = 0;
    int clientPoints = 0;
    char firstCard;

	// karte igralca
    for (int i = 0; i < 2; i++) {
        do {
            randomCard = rand() % 13;
        } while (cards[randomCard]-- <= 0);
        clientPoints += VALUES[randomCard];
        snprintf(buf, MAXLINE, "You got %c ", CARDS[randomCard]);
        send(sockfd, buf, strlen(buf), 0);
		sleep(1);
    }
    snprintf(buf, MAXLINE, "and your points are %d\n", clientPoints);
    send(sockfd, buf, strlen(buf), 0);

    // karte dilerja
    do {
        randomCard = rand() % 13;
    } while (cards[randomCard]-- <= 0);
    dealerPoints += VALUES[randomCard];
    firstCard = CARDS[randomCard];
	sleep(1);
    do {
        randomCard = rand() % 13;
    } while (cards[randomCard]-- <= 0);
    dealerPoints += VALUES[randomCard];
    snprintf(buf, MAXLINE, "The dealer got █ and %c\n", CARDS[randomCard]); //pošlje katero karto je dobil dealer
    send(sockfd, buf, strlen(buf), 0);
	sleep(1);
    while (1) { // game
        snprintf(buf, MAXLINE, "Send H to hit or S to stand\n");
        send(sockfd, buf, strlen(buf), 0);
		sleep(1);
		send(sockfd, ">", 1, 0);
		sleep(1);
        int check = recv(sockfd, buf, MAXLINE, 0);
        if (check < 0) err("recv");

        if (buf[0] == 'H') {
            do {
                randomCard = rand() % 13;
            } while (cards[randomCard]-- <= 0);
            clientPoints += VALUES[randomCard];
            snprintf(buf, MAXLINE, "You got %c and your points are %d\n", CARDS[randomCard], clientPoints);
            send(sockfd, buf, strlen(buf), 0);
            if (clientPoints > 21) {
                snprintf(buf, MAXLINE, "BUST! You lost\n");
                send(sockfd, buf, strlen(buf), 0);
                pthread_exit(0);
            }
        } else if (buf[0] == 'S') {
            snprintf(buf, MAXLINE, "The dealer's first card was %c\n", firstCard);
            send(sockfd, buf, strlen(buf), 0);
            while (dealerPoints <= 17) {
                do {
                    randomCard = rand() % 13;
                } while (cards[randomCard]-- <= 0);
                dealerPoints += VALUES[randomCard];
                snprintf(buf, MAXLINE, "The dealer got %c\n", CARDS[randomCard]);
                send(sockfd, buf, strlen(buf), 0);
            }
            if (dealerPoints > 21) {
                snprintf(buf, MAXLINE, "DEALER BUST! You won\n");
                send(sockfd, buf, strlen(buf), 0);
            } else if (dealerPoints > clientPoints) {
                snprintf(buf, MAXLINE, "You lost\n");
                send(sockfd, buf, strlen(buf), 0);
            } else if (dealerPoints < clientPoints) {
                snprintf(buf, MAXLINE, "You won\n");
                send(sockfd, buf, strlen(buf), 0);
            } else {
                snprintf(buf, MAXLINE, "Draw. Your points:%d Dealer's points:%d\n", clientPoints, dealerPoints);
                send(sockfd, buf, strlen(buf), 0);
            }
            pthread_exit(0);
        }
    }
}

int main(int argc, char *argv[])
{
	
	int n;
	int listenfd, connfd;
	struct sockaddr_in servaddr, cliaddr;
	unsigned short Port;
	time_t ticks;
	socklen_t len;
	char *ip = getIP();
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
	
	
	
	
	if ( listen(listenfd, LISTENQ) < 0)
		err("listen")

	while (1) {	
		len = sizeof(cliaddr);
		pthread_t tid;
		if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) < 0)
			err("accept")

		if (pthread_create(&tid, NULL, (void *)blackJack, (void *)(intptr_t)connfd) != 0)
			err("pthread_create")

		pthread_detach(tid);
	}
}

