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
#define err_sys(mess) { fprintf(stderr, "Error: %s\n", mess); exit(1); }

static void sig_usr(int signo);

void parse(char *niz_ukaz, char *args[]) {
    int counter = 0;
    bool isChanged = true;
    for (int i = 0; niz_ukaz[i] != '\0'; i++) {
        if (niz_ukaz[i] == ' ' || niz_ukaz[i] == '\t') {
            niz_ukaz[i] = '\0';
            isChanged = true;
        } else if (isChanged) {
            args[counter++] = &niz_ukaz[i];
            isChanged = false;
        }
    }
    args[counter] = NULL;
}

int isPipe(char *niz_ukaz) {
    for (int i = 0; niz_ukaz[i] != '\0'; i++) {
        if (niz_ukaz[i] == '=' && niz_ukaz[i + 1] != '\0' && niz_ukaz[i + 1] == '>') {
            return 1;
        }
    }
    return 0;
}

void splitPipe(char *niz_ukaz, char *left_cmd, char *right_cmd) {
    char *delim = strstr(niz_ukaz, "=>");
    if (delim != NULL) {
        *delim = '\0';
        strcpy(left_cmd, niz_ukaz);
        strcpy(right_cmd, delim + 2);
    }
}

int main() {
    pid_t pid, cpid;
    char buf[BUFSIZE];
    char *args[ARGSIZE];
    char left_cmd[BUFSIZE], right_cmd[BUFSIZE];
    int pf[2];

    if (signal(SIGUSR1, sig_usr) == SIG_ERR)
        err_sys("can't catch SIGUSR1");
    if (signal(SIGINT, sig_usr) == SIG_ERR)
        err_sys("can't catch SIGINT");

    printf("tit-shell:$ ");
    while (fgets(buf, BUFSIZE, stdin) != NULL) {
        buf[strlen(buf) - 1] = 0; // Odstranimo newline znak

        if (isPipe(buf)) {
            splitPipe(buf, left_cmd, right_cmd);
            if (pipe(pf) < 0) {
                err_sys("pipe");
            }

            if ((pid = fork()) < 0) {
                err_sys("fork");
            } else if (pid == 0) {
                // P2 
                //printf("P2");
                close(pf[0]); 
                dup2(pf[1], STDOUT_FILENO);
                close(pf[1]);

                parse(left_cmd, args);
                execvp(args[0], args);
                err_sys("Ne morem izvesti levega ukaza");
            }

            if ((cpid = fork()) < 0) {
                err_sys("fork");
            } else if (cpid == 0) {
                // P3
                //printf("P3");
                close(pf[1]); 
                dup2(pf[0], STDIN_FILENO); 
                close(pf[0]);

                parse(right_cmd, args);
                execvp(args[0], args);
                err_sys("Ne morem izvesti desnega ukaza");
            }

            // P1 proces: Zapremo cev in čakamo na otroke
            close(pf[0]);
            close(pf[1]);
            waitpid(pid, NULL, 0); // Počakamo na P2
            waitpid(cpid, NULL, 0); // Počakamo na P3
        } else {
            if ((pid = fork()) < 0) {
                err_sys("fork");
            } else if (pid == 0) {
                parse(buf, args);
                execvp(args[0], args);
                err_sys("Ne morem izvesti ukaza");
            }

            waitpid(pid, NULL, 0); // Počakamo na otroka
        }

        printf("tit-shell:$ ");
    }
    return 0;
}

static void sig_usr(int signo) {
    if (signo == SIGUSR1)
        printf("received SIGUSR1\n");
    else if (signo == SIGINT)
        printf("received SIGINT\n");
    else
        printf("received signal %d\n", signo);
    return;
}
