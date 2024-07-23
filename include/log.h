#ifndef LOG_H
#define LOG_H

#include <time.h>

#define MAX_OPERATIONS 1000

// Definição da estrutura LogEntry
struct LogEntry {
    char timestamp[25];  // Data e hora formatada
    char operation[10];  // Nome da operação
    char arguments[10];  // Argumentos da operação
};

// Declare o array de LogEntry com o tamanho definido pela constante MAX_OPERATIONS
extern struct LogEntry *log_entries;

/* Função que inicializa o log, alocando memória para o array de LogEntry.
*/
void initialize_log();

/* Função que registra uma operação no log, incluindo timestamp, nome da operação e argumentos.
* 
* @param operation Nome da operação a ser registrada.
* @param arguments Argumentos da operação a serem registrados.
*/
void log_operation(char *operation, char *arguments);

/* Função que escreve o conteúdo do log em um arquivo especificado.
* 
* @param log_filename Nome do arquivo onde o log será escrito.
*/
void write_log(char *log_filename);

#endif /* LOG_H */