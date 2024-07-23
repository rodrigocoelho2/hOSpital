/*
* Nº grupo: 19
* David Cândido, 59857
* Diogo Pereira, 59829
* Rodrigo Coelho, 60744
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../include/process.h"
#include "../include/patient.h"
#include "../include/receptionist.h"
#include "../include/doctor.h"

int launch_patient(int patient_id, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    pid_t process_id;
    process_id = fork();

    if (process_id == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (process_id == 0) {
        // Processo filho (paciente)
        execute_patient(patient_id, data, comm, sems);
        exit(EXIT_SUCCESS);
    } else {
        return process_id;
    }
}

int launch_receptionist(int receptionist_id, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    pid_t process_id;
    process_id = fork();

    if (process_id == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (process_id == 0) {
        // Processo filho (receptionista)
        execute_receptionist(receptionist_id, data, comm, sems);
        exit(EXIT_SUCCESS);
    } else {
        return process_id;
    }
}

int launch_doctor(int doctor_id, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    pid_t process_id;
    process_id = fork();

    if (process_id == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (process_id == 0) {
        // Processo filho (médico)
        execute_doctor(doctor_id, data, comm, sems);
        exit(EXIT_SUCCESS);
    } else {
        return process_id;
    }
}

int wait_process(int process_id) {
    int status;
    if(waitpid(process_id, &status, 0) == -1){
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else {
        fprintf(stderr, "O processo com o PID %d não terminou normalmente.\n", process_id);
        exit(EXIT_FAILURE);
    }
}
