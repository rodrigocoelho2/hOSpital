#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 

// Estrutura que contém os argumentos do hospital
struct hospital_arguments {
    int max_ads;                 // Número máximo de anúncios permitidos
    int buffers_size;            // Tamanho dos buffers usados pelo sistema
    int n_patients;              // Número de pacientes no hospital
    int n_receptionists;         // Número de recepcionistas no hospital
    int n_doctors;               // Número de médicos no hospital
    char log_filename[256];      // Nome do arquivo de log
    char statistics_filename[256]; // Nome do arquivo de estatísticas
    int alarm_time;              // Tempo de alarme em segundos
};

// Estrutura que representa um arquivo de configuração
struct ConfigurationFile {
    FILE* ptr;  // Ponteiro para o arquivo de configuração
};

/* Função que inicializa um objeto ConfigurationFile e abre o arquivo de configuração.
* 
* @param filename O nome do arquivo de configuração.
* @return Ponteiro para a estrutura ConfigurationFile inicializada.
*/
struct ConfigurationFile* CONFIG_INIT(const char* filename);

/* Função que libera os recursos associados a um objeto ConfigurationFile.
* 
* @param config_file Ponteiro para a estrutura ConfigurationFile a ser liberada.
*/
void CONFIG_FREE(struct ConfigurationFile* config_file);

/* Função que carrega os argumentos do hospital a partir de um arquivo de configuração.
* 
* @param config_file Ponteiro para a estrutura ConfigurationFile.
* @param args Ponteiro para a estrutura hospital_arguments a ser preenchida.
* @return 0 em caso de sucesso, ou um valor negativo em caso de erro.
*/
int CONFIG_LOAD(struct ConfigurationFile* config_file, struct hospital_arguments* args);

/* Função que lê o arquivo de configuração e preenche a estrutura hospital_arguments.
* 
* @param config_filename O nome do arquivo de configuração.
* @param args Ponteiro para a estrutura hospital_arguments a ser preenchida.
*/
void read_configuration_file(const char* config_filename, struct hospital_arguments* args);

#endif /* CONFIGURATION_H */