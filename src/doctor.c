/*
* Nº grupo: 19
* David Cândido, 59857
* Diogo Pereira, 59829
* Rodrigo Coelho, 60744
*/

#include "../include/doctor.h"
#include "../include/memory.h"
#include <stdio.h>

int execute_doctor(int doctor_id, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    int consultations_processed = 0;
    struct admission ad;

    while (1) {
        doctor_receive_admission(&ad, doctor_id, data, comm, sems);
        if (ad.id != -1 && *(data->terminate) == 0){
            doctor_process_admission(&ad, doctor_id, data, sems);
            consultations_processed++;
        } else if (*(data->terminate) == 1) {
            break;
        }
    }
    return consultations_processed;
}

void doctor_receive_admission(struct admission* ad, int doctor_id, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    if (*(data->terminate) == 1) {
        return; 
    }

    semaphore_lock(sems->receptionist_doctor->empty);
    semaphore_lock(sems->receptionist_doctor->mutex);

    read_receptionist_doctor_buffer(comm->receptionist_doctor, doctor_id, data->buffers_size, ad);

    semaphore_unlock(sems->receptionist_doctor->mutex);
    semaphore_unlock(sems->receptionist_doctor->full);
}


void doctor_process_admission(struct admission* ad, int doctor_id, struct data_container* data, struct semaphores* sems) {
    ad->receiving_doctor = doctor_id;

    int consultas_totais = 0;
    for (int i = 0; i < data->n_doctors; i++) {
        consultas_totais += data->doctor_stats[i];
    }

    if (consultas_totais < data->max_ads) {
        ad->status = 'A';
        semaphore_lock(sems->doctor_stats_mutex);
        data->doctor_stats[doctor_id]++; 
        semaphore_unlock(sems->doctor_stats_mutex);
    } else {
        ad->status = 'N'; 
    }

    semaphore_lock(sems->results_mutex);
    for (int i = 0; i < data->max_ads; i++) {
        if (data->results[i].id == ad->id) {
            data->results[i].receiving_doctor = doctor_id;
            data->results[i].status = ad->status;
            break;
        }
    }
    semaphore_unlock(sems->results_mutex);
}