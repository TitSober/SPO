#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>


#define SHMSIZE 1024
#define MSGSIZE 1024


struct msgbuf {
    long mtype; 
    char mtext[MSGSIZE];
};


void sem_lock(int semid) {
    struct sembuf buf = {0, -1, 0};
    semop(semid, &buf, 1);
}


void sem_unlock(int semid) {
    struct sembuf buf = {0, 1, 0};
    semop(semid, &buf, 1);
}

int main() {
    key_t key = ftok("/tmp", 'A'); // Ustvari ključ za IPC
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    int semid = semget(key, 1, 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    
    int shmid = shmget(key, SHMSIZE, 0644);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }


    char *data = (char *)shmat(shmid, NULL, 0);
    if (data == (void *)-1) {
        perror("shmat");
        exit(1);
    }

   
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    char prev_msg[SHMSIZE] = ""; 
    struct msgbuf msg;
    msg.mtype = 1;

    while (1) {
        sleep(1); 
        
        sem_lock(semid);

       
        if (strcmp(data, prev_msg) != 0) {
            strcpy(prev_msg, data);
            printf("Preberano sporočilo: %s\n", data);
            
            strncpy(msg.mtext, data, MSGSIZE);

           
            if (msgsnd(msgid, &msg, strlen(msg.mtext) + 1, 0) == -1) {
                perror("msgsnd");
                break;
            }

            
            if (data[0] == '\0') {
                break;
            }
        }

     
        sem_unlock(semid);
    }

   
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl");
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
    }

    
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
    }

   
    if (shmdt(data) == -1) {
        perror("shmdt");
    }

    printf("Posrednik končan.\n");
    return 0;
}
