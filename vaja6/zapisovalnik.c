#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MSGSIZE 1024


struct msgbuf {
    long mtype;         
    char mtext[MSGSIZE]; 
};

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        fprintf(stderr, "Uporaba: %s <ime_datoteke>\n", argv[0]);
        exit(1);
    }

    
    FILE *file = fopen(argv[1], "w");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }

   
    key_t key = ftok("/tmp", 'A'); 
    if (key == -1) {
        perror("ftok");
        fclose(file);
        exit(1);
    }

    int msgid = msgget(key, 0666);
    if (msgid == -1) {
        perror("msgget");
        fclose(file);
        exit(1);
    }

    struct msgbuf msg;
    while (1) {
        
        if (msgrcv(msgid, &msg, MSGSIZE, 0, 0) == -1) {
            perror("msgrcv");
            break;
        }

       
        if (msg.mtext[0] == '\0') {
            fprintf(file, "Prazno sporočilo prejeto.\n");
            break;
        }

      
        fprintf(file, "%s\n", msg.mtext);
    }

   
    fclose(file);

   
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
    }

    printf("Program končan, sporočilna vrsta pobrisana, datoteka zaprta.\n");
    return 0;
}
