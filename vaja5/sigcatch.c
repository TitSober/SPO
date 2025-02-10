#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>




void handle_sigint(int signo){
    
    int neprespan_cas = alarm(0);
    printf("\nPrejel sem signal %d.\n", signo);
    printf("neprespan cas = %d.\n", neprespan_cas);

    alarm(neprespan_cas);
    pause();

}

void handle_alarm(int signo){
    //return
}

int main(){
     printf("Lovim SIGINT\n");
    signal(SIGINT, handle_sigint);
    signal(SIGALRM, handle_alarm);
   
    alarm(5); 

    pause(); 
    printf("Alarm v casu t = 5\n");

    
    printf("Ignoriram signal SIGINT \n");
    signal(SIGINT, SIG_IGN); 
    alarm(5); 
    pause(); 
    printf("Alarm v casu t = 10\n");

    
    printf("Privzeta akcija za SIGINT\n");
    signal(SIGINT, SIG_DFL);
    alarm(5); 

    pause();
    printf("Alarm v casu t = 15\n");

    return 0;
}