/*
* Nº grupo: 19
* David Cândido, 59857
* Diogo Pereira, 59829
* Rodrigo Coelho, 60744
*/

#include "../include/configuration.h"
#include "../include/memory.h"
#include "../include/main-private.h"


struct ConfigurationFile* CONFIG_INIT(const char* filename) {
    struct ConfigurationFile* config_file = (struct ConfigurationFile*)allocate_dynamic_memory(sizeof(struct ConfigurationFile));
    
    if (config_file != NULL) {
        config_file->ptr = fopen(filename, "r");

        if (config_file->ptr == NULL) {
            fprintf(stderr, "Erro ao abrir o arquivo de configuração: %s\n", filename);
            deallocate_dynamic_memory(config_file);
            return NULL;
        }
    }
    return config_file;
}

void CONFIG_FREE(struct ConfigurationFile* config_file) {
    if (config_file && config_file->ptr)
        fclose(config_file->ptr);
    deallocate_dynamic_memory(config_file);
}

int CONFIG_LOAD(struct ConfigurationFile* config_file, struct hospital_arguments* args) {
    if (config_file == NULL || config_file->ptr == NULL) {
        fprintf(stderr, "Arquivo de configuração não está ativo.\n");
        return -1;
    }

    char line[256];
    int line_number = 0;

    while (fgets(line, sizeof(line), config_file->ptr) != NULL) {
        line_number++;

        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';

        switch (line_number) {
            case 1:
                args->max_ads = atoi(line);
                break;
            case 2:
                args->buffers_size = atoi(line);
                break;
            case 3:
                args->n_patients = atoi(line);
                break;
            case 4:
                args->n_receptionists = atoi(line);
                break;
            case 5:
                args->n_doctors = atoi(line);
                break;
            case 6:
                strncpy(args->log_filename, line, sizeof(args->log_filename) - 1);
                args->log_filename[sizeof(args->log_filename) - 1] = '\0';
                break;
            case 7:
                strncpy(args->statistics_filename, line, sizeof(args->statistics_filename) - 1);
                args->statistics_filename[sizeof(args->statistics_filename) - 1] = '\0';
                break;
            case 8:
                args->alarm_time = atoi(line);
                break;
            default:
                fprintf(stderr, "Sintaxe incorreta no arquivo de configuração na linha %d\n", line_number);
                return -1;
        }
    }

    return line_number;
}

void read_configuration_file(const char* config_filename, struct hospital_arguments* args) {
    struct ConfigurationFile* config_file = CONFIG_INIT(config_filename);
    if (!config_file || !config_file->ptr) {
        fprintf(stderr, "Erro ao inicializar o arquivo de configuração.\n");
        CONFIG_FREE(config_file);
        return;
    }

    int loadedLines = CONFIG_LOAD(config_file, args);

    if (loadedLines < 8) {
        fprintf(stderr, "Erro: o arquivo de configuração está faltando campos obrigatórios.\n");
        CONFIG_FREE(config_file);
        return;
    }

    if (loadedLines > 8) {
        printf("Aviso: o arquivo de configuração tem mais linhas do que o esperado (%d linhas, esperado 8).\n", loadedLines);
    }

    printf("Arquivo de configuração carregado com sucesso: %s\n", config_filename);
    CONFIG_FREE(config_file);
}
