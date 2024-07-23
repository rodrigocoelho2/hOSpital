/*
* Nº grupo: 19
* David Cândido, 59857
* Diogo Pereira, 59829
* Rodrigo Coelho, 60744
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h> 

#include "../include/main.h"
#include "../include/main-private.h"
#include "../include/process.h"
#include "../include/memory.h"
#include "../include/doctor.h"
#include "../include/patient.h"
#include "../include/receptionist.h"
#include "../include/synchronization.h"
#include "../include/log.h"
#include "../include/stats.h"

#define BUFFER_SIZE 10
#define MAX_LINE_LENGTH 256

int ad_counter = 0;
int global_patient_id = -1;
int global_doctor_id = -1;
int global_info_id = -1;

void main_args(int argc, char *argv[], struct data_container *data) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <config_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *config_filename = argv[1];
    FILE *file = fopen(config_filename, "r");
    if (file == NULL) {
        perror("Erro ao abrir o ficheiro");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    
    if (fgets(line, sizeof(line), file)) data->max_ads = atoi(line);
    if (fgets(line, sizeof(line), file)) data->buffers_size = atoi(line);
    if (fgets(line, sizeof(line), file)) data->n_patients = atoi(line);
    if (fgets(line, sizeof(line), file)) data->n_receptionists = atoi(line);
    if (fgets(line, sizeof(line), file)) data->n_doctors = atoi(line);
    if (fgets(line, sizeof(line), file)) strncpy(data->log_filename, line, sizeof(data->log_filename) - 1);
    if (fgets(line, sizeof(line), file)) strncpy(data->statistics_filename, line, sizeof(data->statistics_filename) - 1);
    if (fgets(line, sizeof(line), file)) data->alarm_time = atoi(line);

    data->log_filename[strcspn(data->log_filename, "\n")] = 0;
    data->statistics_filename[strcspn(data->statistics_filename, "\n")] = 0;

    fclose(file);
}


void allocate_dynamic_memory_buffers(struct data_container* data) {
    free(data->patient_pids);
    data->patient_pids = allocate_dynamic_memory(data->n_patients * sizeof(pid_t));
    free(data->receptionist_pids);
    data->receptionist_pids = allocate_dynamic_memory(data->n_receptionists * sizeof(pid_t));
    free(data->doctor_pids);
    data->doctor_pids = allocate_dynamic_memory(data->n_doctors * sizeof(pid_t));

    data->patient_stats = allocate_dynamic_memory(data->n_patients * sizeof(int));
    data->receptionist_stats = allocate_dynamic_memory(data->n_receptionists * sizeof(int));
    data->doctor_stats = allocate_dynamic_memory(data->n_doctors * sizeof(int));
}

void create_shared_memory_buffers(struct data_container* data, struct communication* comm) {
    comm->main_patient->ptrs = create_shared_memory(STR_SHM_MAIN_PATIENT_PTR, sizeof(struct pointers));
    comm->main_patient->buffer = create_shared_memory(STR_SHM_MAIN_PATIENT_BUFFER, data->buffers_size);

    comm->patient_receptionist->ptrs = create_shared_memory(STR_SHM_PATIENT_RECEPT_PTR, sizeof(struct pointers));
    comm->patient_receptionist->buffer = create_shared_memory(STR_SHM_PATIENT_RECEPT_BUFFER, data->buffers_size);

    comm->receptionist_doctor->ptrs = create_shared_memory(STR_SHM_RECEPT_DOCTOR_PTR, sizeof(struct pointers));
    comm->receptionist_doctor->buffer = create_shared_memory(STR_SHM_RECEPT_DOCTOR_BUFFER, data->buffers_size);

    data->results = create_shared_memory(STR_SHM_RESULTS, MAX_RESULTS * sizeof(struct admission));
    data->terminate = create_shared_memory(STR_SHM_TERMINATE, sizeof(int));
}

void launch_processes(struct data_container* data, struct communication* comm, struct semaphores* sems) {
    for (int i = 0; i < data->n_patients; i++) {
        data->patient_pids[i] = launch_patient(i, data, comm, sems);
    }
    for (int i = 0; i < data->n_receptionists; i++) {
        data->receptionist_pids[i] = launch_receptionist(i, data, comm, sems);
    }
    for (int i = 0; i < data->n_doctors; i++) {
        data->doctor_pids[i] = launch_doctor(i, data, comm, sems);
    }
}

void user_interaction(struct data_container* data, struct communication* comm, struct semaphores* sems) {
    char command[15];
    int run = 1;
    print_help();
    
    while (run) {
        printf("\n[Main] Introduzir ação: ");
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            fprintf(stderr, "Erro ao ler o comando.\n");
            continue;
        }

        command[strcspn(command, "\n")] = '\0';
        printf("[Debug] Comando lido: %s\n", command);

        if (strncmp(command, "ad", 2) == 0) {
            if (sscanf(command, "ad %d %d", &global_patient_id, &global_doctor_id) != 2) {
                fprintf(stderr, "Uso correto: ad <patient_id> <doctor_id>\n");
                continue;
            }
            char arguments[20]; // ou tamanho suficiente para conter seus argumentos
            sprintf(arguments, "%d %d", global_patient_id, global_doctor_id);
            log_operation("ad", arguments);
            create_request(&ad_counter, data, comm, sems);
        } else if (strncmp(command, "info", 1) == 0) {
            if (sscanf(command, "info %d", &global_info_id) != 1) {
                fprintf(stderr, "Uso correto: info <id>\n");
                continue;
            }else{
                char arguments[20]; // ou tamanho suficiente para conter seus argumentos
                sprintf(arguments, "%d", global_info_id);
                log_operation("info", arguments);
                read_info(data, sems);
            }
        } else if (strcmp(command, "status") == 0) {
            
            log_operation("status", "");
            print_status(data, sems);
        } else if (strcmp(command, "help") == 0) {
            log_operation("help", "");
            print_help();
        } else if (strcmp(command, "end") == 0) {
            log_operation("end", "");
            end_execution(data, comm, sems);
            run = 0;
        } else {
            printf("Comando não reconhecido. Tente novamente.\n");
        }
    }
}

void print_help() {
    printf("[Main] Ações disponíveis:\n");
    printf("[Main]  ad <patient_id> <doctor_id> - cria uma nova admissão.\n");
    printf("[Main]  info <id> - verifica o estado de uma admissão id.\n");
    printf("[Main]  status - imprime o estado das variáveis da estrutura <data_container>.\n");
    printf("[Main]  help - imprime informação sobre os comandos disponíveis.\n");
    printf("[Main]  end - termina a execução do hOSpital.\n");
}

void create_request(int* ad_counter, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    int patient = global_patient_id;
    int doctor = global_doctor_id;

    if (patient < 0 || doctor < 0 || doctor >= data->n_doctors || patient >= data->n_patients) {
        fprintf(stderr, "Erro: ID do paciente ou médico fora do intervalo válido.\n");
        return;
    }

    printf("[Debug] ID do paciente: %d, ID do médico: %d\n", patient, doctor);

    struct admission ad;
    ad.id = *ad_counter;
    ad.requesting_patient = patient;
    ad.requested_doctor = doctor;
    ad.status = 'M';

    produce_begin(sems->main_patient);
    write_main_patient_buffer(comm->main_patient, data->buffers_size, &ad);
    produce_end(sems->main_patient);

    printf("Id da admissão: %d\n", *ad_counter);
    data->patient_stats[patient]++;
    (*ad_counter)++;
}

void read_info(struct data_container* data, struct semaphores* sems) {
    int id = global_info_id;

    int total_admissions = 0;
    for (int i = 0; i < MAX_RESULTS; i++) {
        if (data->results[i].id >= 0) {
            total_admissions++;
        }
    }

    if (id < 0 || id >= MAX_RESULTS) {
        fprintf(stderr, "ID de admissão inválido.\n");
        return;
    }

    printf("[Debug] ID da admissão lido: %d\n", id);

    semaphore_lock(sems->results_mutex);
    if (data->results[id].status != '\0') {
        printf("Admissão: %d\n", id);
        printf("Estado: %c\n", data->results[id].status);
        printf("Paciente que fez o pedido: %d\n", data->results[id].requesting_patient);
        printf("Médico requisitado: %d\n", data->results[id].requested_doctor);
        printf("Paciente que processou: %d\n", data->results[id].receiving_patient);
        printf("Rececionista que processou: %d\n", data->results[id].receiving_receptionist);
        printf("Médico que processou: %d\n", data->results[id].receiving_doctor);
    } else {
        printf("Admissão %d não existe.\n", id);
    }
    semaphore_unlock(sems->results_mutex);
}

void print_status(struct data_container* data, struct semaphores* sems) {
    printf("[Main] Pacientes:\n");
    for (int i = 0; i < data->n_patients; i++) {
        printf("[Main]  Paciente %d -> Pid %d\n", i, data->patient_pids[i]);
    }

    printf("[Main] Recepcionistas:\n");
    for (int i = 0; i < data->n_receptionists; i++) {
        printf("[Main]  Recepcionista %d -> Pid %d\n", i, data->receptionist_pids[i]);
    }

    printf("[Main] Médicos:\n");
    for (int i = 0; i < data->n_doctors; i++) {
        printf("[Main]  Médico %d -> Pid %d\n", i, data->doctor_pids[i]);
    }

    printf("[Main] Estatísticas dos Pacientes:\n");
    for (int i = 0; i < data->n_patients; i++) {
        printf("[Main]  Paciente %d -> Número de Admissões: %d\n", i, data->patient_stats[i]);
    }

    printf("[Main] Estatísticas dos Recepcionistas:\n");
    for (int i = 0; i < data->n_receptionists; i++) {
        printf("[Main]  Recepcionista %d -> Número de Atendimentos: %d\n", i, data->receptionist_stats[i]);
    }

    printf("[Main] Estatísticas dos Médicos:\n");
    for (int i = 0; i < data->n_doctors; i++) {
        printf("[Main]  Médico %d -> Número de Atendimentos: %d\n", i, data->doctor_stats[i]);
    }
}

void end_execution(struct data_container* data, struct communication* comm, struct semaphores* sems) {
    printf("Fim do programa.\n");
    write_log(data->log_filename);
    write_statistics(data);
    *(data->terminate) = 1;

    // Destroy semaphores
    semaphore_destroy(STR_SEM_MAIN_PATIENT_FULL, sems->main_patient->full);
    semaphore_destroy(STR_SEM_MAIN_PATIENT_EMPTY, sems->main_patient->empty);
    semaphore_destroy(STR_SEM_MAIN_PATIENT_MUTEX, sems->main_patient->mutex);

    semaphore_destroy(STR_SEM_PATIENT_RECEPT_FULL, sems->patient_receptionist->full);
    semaphore_destroy(STR_SEM_PATIENT_RECEPT_EMPTY, sems->patient_receptionist->empty);
    semaphore_destroy(STR_SEM_PATIENT_RECEPT_MUTEX, sems->patient_receptionist->mutex);

    semaphore_destroy(STR_SEM_RECEPT_DOCTOR_FULL, sems->receptionist_doctor->full);
    semaphore_destroy(STR_SEM_RECEPT_DOCTOR_EMPTY, sems->receptionist_doctor->empty);
    semaphore_destroy(STR_SEM_RECEPT_DOCTOR_MUTEX, sems->receptionist_doctor->mutex);

    semaphore_destroy(STR_SEM_PATIENT_STATS_MUTEX, sems->patient_stats_mutex);
    semaphore_destroy(STR_SEM_RECEPT_STATS_MUTEX, sems->receptionist_stats_mutex);
    semaphore_destroy(STR_SEM_DOCTOR_STATS_MUTEX, sems->doctor_stats_mutex);
    semaphore_destroy(STR_SEM_RESULTS_MUTEX, sems->results_mutex);
    semaphore_destroy(STR_SEM_TERMINATE_MUTEX, sems->terminate_mutex);

    // Destroy shared memory
    destroy_shared_memory(STR_SHM_MAIN_PATIENT_PTR, comm->main_patient->ptrs, sizeof(struct pointers));
    destroy_shared_memory(STR_SHM_MAIN_PATIENT_BUFFER, comm->main_patient->buffer, data->buffers_size);
    destroy_shared_memory(STR_SHM_PATIENT_RECEPT_PTR, comm->patient_receptionist->ptrs, data->buffers_size);
    destroy_shared_memory(STR_SHM_PATIENT_RECEPT_BUFFER, comm->patient_receptionist->buffer, data->buffers_size);
    destroy_shared_memory(STR_SHM_RECEPT_DOCTOR_PTR, comm->receptionist_doctor->ptrs, sizeof(struct pointers));
    destroy_shared_memory(STR_SHM_RECEPT_DOCTOR_BUFFER, comm->receptionist_doctor->buffer, data->buffers_size);
    destroy_shared_memory(STR_SHM_RESULTS, data->results, MAX_RESULTS * sizeof(struct admission));
    destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, sizeof(int));

    // Free dynamic memory
    free(data->patient_pids);
    free(data->receptionist_pids);
    free(data->doctor_pids);
    free(data->patient_stats);
    free(data->receptionist_stats);
    free(data->doctor_stats);

    exit(EXIT_SUCCESS); // Saída normal
}

void wait_processes(struct data_container* data) {
    for (int i = 0; i < data->n_patients; i++) {
        wait_process(data->patient_pids[i]);
    }
    for (int i = 0; i < data->n_receptionists; i++) {
        wait_process(data->receptionist_pids[i]);
    }
    for (int i = 0; i < data->n_doctors; i++) {
        wait_process(data->doctor_pids[i]);
    }
}

void write_statistics(struct data_container* data) {
    FILE *file = fopen(data->statistics_filename, "w");
    if (file == NULL) {
        perror("Erro ao abrir o ficheiro de estatísticas");
        return;
    }

    fprintf(file, "Estatísticas dos Pacientes:\n");
    for (int i = 0; i < data->n_patients; i++) {
        fprintf(file, "Paciente %d -> Número de Admissões: %d\n", i, data->patient_stats[i]);
    }

    fprintf(file, "Estatísticas dos Recepcionistas:\n");
    for (int i = 0; i < data->n_receptionists; i++) {
        fprintf(file, "Recepcionista %d -> Número de Atendimentos: %d\n", i, data->receptionist_stats[i]);
    }

    fprintf(file, "Estatísticas dos Médicos:\n");
    for (int i = 0; i < data->n_doctors; i++) {
        fprintf(file, "Médico %d -> Número de Atendimentos: %d\n", i, data->doctor_stats[i]);
    }

    fclose(file);
}

void destroy_memory_buffers(struct data_container* data, struct communication* comm) {
    deallocate_dynamic_memory(data->patient_pids);
    deallocate_dynamic_memory(data->receptionist_pids);
    deallocate_dynamic_memory(data->doctor_pids);
    deallocate_dynamic_memory(data->patient_stats);
    deallocate_dynamic_memory(data->receptionist_stats);
    deallocate_dynamic_memory(data->doctor_stats);

    destroy_shared_memory(STR_SHM_MAIN_PATIENT_PTR, comm->main_patient->ptrs, sizeof(struct pointers));
    destroy_shared_memory(STR_SHM_MAIN_PATIENT_BUFFER, comm->main_patient->buffer, data->buffers_size);
    destroy_shared_memory(STR_SHM_PATIENT_RECEPT_PTR, comm->patient_receptionist->ptrs, data->buffers_size);
    destroy_shared_memory(STR_SHM_PATIENT_RECEPT_BUFFER, comm->patient_receptionist->buffer, data->buffers_size);
    destroy_shared_memory(STR_SHM_RECEPT_DOCTOR_PTR, comm->receptionist_doctor->ptrs, sizeof(struct pointers));
    destroy_shared_memory(STR_SHM_RECEPT_DOCTOR_BUFFER, comm->receptionist_doctor->buffer, data->buffers_size);
    destroy_shared_memory(STR_SHM_RESULTS, data->results, MAX_RESULTS*sizeof(struct admission));
    destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, sizeof(int));
}

void create_semaphores(struct data_container* data, struct semaphores* sems){
    // Inicialização dos semáforos
    sems->main_patient->full = semaphore_create(STR_SEM_MAIN_PATIENT_FULL, 0);
    sems->main_patient->empty = semaphore_create(STR_SEM_MAIN_PATIENT_EMPTY, BUFFER_SIZE);
    sems->main_patient->mutex = semaphore_create(STR_SEM_MAIN_PATIENT_MUTEX, 1);

    sems->patient_receptionist->full = semaphore_create(STR_SEM_PATIENT_RECEPT_FULL, 0);
    sems->patient_receptionist->empty = semaphore_create(STR_SEM_PATIENT_RECEPT_EMPTY, BUFFER_SIZE);
    sems->patient_receptionist->mutex = semaphore_create(STR_SEM_PATIENT_RECEPT_MUTEX, 1);

    sems->receptionist_doctor->full = semaphore_create(STR_SEM_RECEPT_DOCTOR_FULL, 0);
    sems->receptionist_doctor->empty = semaphore_create(STR_SEM_RECEPT_DOCTOR_EMPTY, BUFFER_SIZE);
    sems->receptionist_doctor->mutex = semaphore_create(STR_SEM_RECEPT_DOCTOR_MUTEX, 1);

    sems->patient_stats_mutex = semaphore_create(STR_SEM_PATIENT_STATS_MUTEX, 1);
    sems->receptionist_stats_mutex = semaphore_create(STR_SEM_RECEPT_STATS_MUTEX, 1);
    sems->doctor_stats_mutex = semaphore_create(STR_SEM_DOCTOR_STATS_MUTEX, 1);
    sems->results_mutex = semaphore_create(STR_SEM_RESULTS_MUTEX, 1);
    sems->terminate_mutex = semaphore_create(STR_SEM_TERMINATE_MUTEX, 1);

    if (sems->main_patient->full == NULL || sems->main_patient->empty == NULL || sems->main_patient->mutex == NULL ||
        sems->patient_receptionist->full == NULL || sems->patient_receptionist->empty == NULL || sems->patient_receptionist->mutex == NULL ||
        sems->receptionist_doctor->full == NULL || sems->receptionist_doctor->empty == NULL || sems->receptionist_doctor->mutex == NULL ||
        sems->patient_stats_mutex == NULL || sems->receptionist_stats_mutex == NULL || sems->doctor_stats_mutex == NULL ||
        sems->results_mutex == NULL || sems->terminate_mutex == NULL) {
        perror("Failed to create semaphores");
        deallocate_dynamic_memory(sems->receptionist_doctor);
        deallocate_dynamic_memory(sems->patient_receptionist);
        deallocate_dynamic_memory(sems->main_patient);
        deallocate_dynamic_memory(sems);
        deallocate_dynamic_memory(data);
        exit(EXIT_FAILURE);
    }
}

void destroy_semaphores(struct semaphores* sems) {
    semaphore_destroy(STR_SEM_MAIN_PATIENT_FULL, sems->main_patient->full);
    semaphore_destroy(STR_SEM_MAIN_PATIENT_EMPTY, sems->main_patient->empty);
    semaphore_destroy(STR_SEM_MAIN_PATIENT_MUTEX, sems->main_patient->mutex);

    semaphore_destroy(STR_SEM_PATIENT_RECEPT_FULL, sems->patient_receptionist->full);
    semaphore_destroy(STR_SEM_PATIENT_RECEPT_EMPTY, sems->patient_receptionist->empty);
    semaphore_destroy(STR_SEM_PATIENT_RECEPT_MUTEX, sems->patient_receptionist->mutex);

    semaphore_destroy(STR_SEM_RECEPT_DOCTOR_FULL, sems->receptionist_doctor->full);
    semaphore_destroy(STR_SEM_RECEPT_DOCTOR_EMPTY, sems->receptionist_doctor->empty);
    semaphore_destroy(STR_SEM_RECEPT_DOCTOR_MUTEX, sems->receptionist_doctor->mutex);

    semaphore_destroy(STR_SEM_PATIENT_STATS_MUTEX, sems->patient_stats_mutex);
    semaphore_destroy(STR_SEM_RECEPT_STATS_MUTEX, sems->receptionist_stats_mutex);
    semaphore_destroy(STR_SEM_DOCTOR_STATS_MUTEX, sems->doctor_stats_mutex);
    semaphore_destroy(STR_SEM_RESULTS_MUTEX, sems->results_mutex);
    semaphore_destroy(STR_SEM_TERMINATE_MUTEX, sems->terminate_mutex);

    deallocate_dynamic_memory(sems->receptionist_doctor);
    deallocate_dynamic_memory(sems->patient_receptionist);
    deallocate_dynamic_memory(sems->main_patient);

    deallocate_dynamic_memory(sems);
}

struct data_container* initialize_data_container() {
    struct data_container* data = allocate_dynamic_memory(sizeof(struct data_container));
    if (data == NULL) {
        perror("Failed to allocate memory for data_container");
        exit(EXIT_FAILURE);
    }
    return data;
}

struct communication* initialize_communication() {
    struct communication* comm = allocate_dynamic_memory(sizeof(struct communication));
    if (comm == NULL) {
        perror("Failed to allocate memory for communication");
        exit(EXIT_FAILURE);
    }

    comm->main_patient = allocate_dynamic_memory(sizeof(struct circular_buffer));
    if (comm->main_patient == NULL) {
        perror("Failed to allocate memory for main_patient buffer");
        deallocate_dynamic_memory(comm);
        exit(EXIT_FAILURE);
    }

    comm->patient_receptionist = allocate_dynamic_memory(sizeof(struct rnd_access_buffer));
    if (comm->patient_receptionist == NULL) {
        perror("Failed to allocate memory for patient_receptionist buffer");
        deallocate_dynamic_memory(comm->main_patient);
        deallocate_dynamic_memory(comm);
        exit(EXIT_FAILURE);
    }

    comm->receptionist_doctor = allocate_dynamic_memory(sizeof(struct circular_buffer));
    if (comm->receptionist_doctor == NULL) {
        perror("Failed to allocate memory for receptionist_doctor buffer");
        deallocate_dynamic_memory(comm->patient_receptionist);
        deallocate_dynamic_memory(comm->main_patient);
        deallocate_dynamic_memory(comm);
        exit(EXIT_FAILURE);
    }

    return comm;
}

int main(int argc, char *argv[]) {
    // Inicialização das estruturas de dados
    struct data_container* data = initialize_data_container();
    struct communication* comm = initialize_communication();

    // Inicialização da estrutura sems
    struct semaphores sems;
    sems.main_patient = malloc(sizeof(struct prodcons));
    sems.patient_receptionist = malloc(sizeof(struct prodcons));
    sems.receptionist_doctor = malloc(sizeof(struct prodcons));

    if (sems.main_patient == NULL || sems.patient_receptionist == NULL || sems.receptionist_doctor == NULL) {
        perror("Failed to allocate memory for semaphores");
        exit(EXIT_FAILURE);
    }

    initialize_stats(data);

    // Execução do código principal
    main_args(argc, argv, data);
    allocate_dynamic_memory_buffers(data);
    create_shared_memory_buffers(data, comm);
    create_semaphores(data, &sems);
    launch_processes(data, comm, &sems);
    initialize_log();
    user_interaction(data, comm, &sems);

    // Liberação de memória antes de terminar
    end_execution(data, comm, &sems);
    destroy_memory_buffers(data, comm);
    destroy_semaphores(&sems);

    deallocate_dynamic_memory(data);
    deallocate_dynamic_memory(comm->main_patient);
    deallocate_dynamic_memory(comm->patient_receptionist);
    deallocate_dynamic_memory(comm->receptionist_doctor);
    deallocate_dynamic_memory(comm);
    deallocate_dynamic_memory(&sems);

    return 0;
}