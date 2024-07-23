/*
* Nº grupo: 19
* David Cândido, 59857
* Diogo Pereira, 59829
* Rodrigo Coelho, 60744
*/

#include "../include/log.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX_OPERATIONS 1000

int max_log_entries = 1000;
struct LogEntry *log_entries = NULL;
static int log_index = 0;

void initialize_log() {
    log_entries = malloc(max_log_entries * sizeof(struct LogEntry));

    if (log_entries == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o log.\n");
        exit(EXIT_FAILURE);
    }
}

void log_operation(char* operation, char* arguments) {
    if (operation == NULL) {
        fprintf(stderr, "Erro: A operação não pode ser nula.\n");
        return;
    }

    if (log_index >= MAX_OPERATIONS) {
        fprintf(stderr, "Erro: Limite máximo de operações no log atingido.\n");
        return;
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm *tm_info = localtime(&ts.tv_sec);

    char buffer[25];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y_%H:%M:%S", tm_info);
    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), ".%03ld", ts.tv_nsec / 1000000);

    strncpy(log_entries[log_index].timestamp, buffer, sizeof(log_entries[log_index].timestamp) - 1);
    log_entries[log_index].timestamp[sizeof(log_entries[log_index].timestamp) - 1] = '\0';

    strncpy(log_entries[log_index].operation, operation, sizeof(log_entries[log_index].operation) - 1);
    log_entries[log_index].operation[sizeof(log_entries[log_index].operation) - 1] = '\0';
    strncpy(log_entries[log_index].arguments, arguments, sizeof(log_entries[log_index].arguments) - 1);
    log_entries[log_index].arguments[sizeof(log_entries[log_index].arguments) - 1] = '\0';

    log_index++;
}

void write_log(char *log_filename) {
    FILE *file = fopen(log_filename, "w");
    if (file == NULL) {
        perror("Erro ao abrir o ficheiro de log");
        return;
    }

    for (int i = 0; i < log_index; i++) {
        fprintf(file, "%s %s %s\n", log_entries[i].timestamp, log_entries[i].operation, log_entries[i].arguments);
    }

    fclose(file);
}
