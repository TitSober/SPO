#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <string.h>

#define SHMSIZE 1024


void sem_lock(int semid) {
    struct sembuf buf = {0, -1, 0}; 
    semop(semid, &buf, 1);
}


void sem_unlock(int semid) {
    struct sembuf buf = {0, 1, 0}; 
    semop(semid, &buf, 1);
}

int main() {
    key_t key = ftok("/tmp", 'A'); 
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    
    int semid = semget(key, 1, 0666 | IPC_CREAT);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    
    if (semctl(semid, 0, SETVAL, 1) == -1) {
        perror("semctl");
        exit(1);
    }

    
    int shmid = shmget(key, SHMSIZE, 0644 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    
    char *data = (char *)shmat(shmid, NULL, 0);
    if (data == (void *)-1) {
        perror("shmat");
        exit(1);
    }

   
    for (int i = 1; i < 6; i++) {
        sleep(2); 

        sem_lock(semid);
        if (i == 5) {
            
            data[0] = '\0';
        } else {
            snprintf(data, SHMSIZE, "ID %d: To je sporočilo.\n", i);
            printf("Sporočilo %d je poslan.\n", i);
        }
        sleep(2); 
        sem_unlock(semid);
    }

   
    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}
