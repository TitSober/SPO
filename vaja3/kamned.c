#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 1024

void append_to_file(const char *filename) {
    FILE *file;
    if((file = fopen(filename, "a"))< 0){
        perror("Ne morem odpreti datoteke");
        exit(-1);
    }

    char line[MAX_LINE_LEN];
    printf("Vnesite vrstice za dodajanje:\n");
    while (fgets(line, sizeof(line), stdin)) {
        fputs(line, file);
    }

    fclose(file);
}

void delete_line_from_file(const char *filename, int line_num) {
    FILE *file;
    FILE *temp;
    
    if((file = fopen(filename, "r"))<0){
        perror("Napaka pri odpiranju datoteke");
        exit(-1);
    }
    if((temp = fopen("temp.txt", "w"))<0){
        perror("Napaka pri odpiranju datoteke");
        exit(-1);
    }
   

    char line[MAX_LINE_LEN];
    int current_line = 1;

    while (fgets(line, sizeof(line), file)) {
        if (current_line != line_num) {
            fputs(line, temp);
        }
        current_line++;
    }

    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp.txt", filename);
}

void insert_line_into_file(const char *filename, int line_num) {
    FILE *file;
    FILE *temp;
    if((file = fopen(filename, "r"))<0){
        perror("Napaka pri odpiranju datoteke");
        exit(-1);
    }
   
    if((temp = fopen("temp.txt", "w"))<0){
        perror("Napaka pri odpiranju datoteke");
        exit(-1);
    }
   

    char line[MAX_LINE_LEN];
    int current_line = 1;

    printf("Vnesite vrstico za vstavljanje:\n");
    char new_line[MAX_LINE_LEN];
    fgets(new_line, sizeof(new_line), stdin);

    while (fgets(line, sizeof(line), file)) {
        if (current_line == line_num) {
            fputs(new_line, temp);
        }
        fputs(line, temp);
        current_line++;
    }
    if (current_line == line_num) {
        fputs(new_line, temp);
    }

    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp.txt", filename);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uporaba: kamned ime_datoteke ukaz \n");
        return -1;
    }

    const char *filename = argv[1];
    const char *command = argv[2];

    if (strcmp(command, "-a") == 0) {
        append_to_file(filename);
    } else if (strcmp(command, "-d") == 0 && argc == 4) {
        int line_num = atoi(argv[3]);
        delete_line_from_file(filename, line_num);
    } else if (strcmp(command, "-i") == 0 && argc == 4) {
        int line_num = atoi(argv[3]);
        insert_line_into_file(filename, line_num);
    } else {
        fprintf(stderr, "Neveljaven ukaz.\n");
        return -1;
    }

    return 0;
}
