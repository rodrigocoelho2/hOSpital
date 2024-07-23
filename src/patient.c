/*
* Nº grupo: 19
* David Cândido, 59857
* Diogo Pereira, 59829
* Rodrigo Coelho, 60744
*/

#include <stdio.h>
#include <stdlib.h>
#include "../include/patient.h"
#include "../include/memory.h"
#include "../include/synchronization.h"

int execute_patient(int patient_id, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    while (1) {
        struct admission ad;
        patient_receive_admission(&ad, patient_id, data, comm, sems);
        if (*(data->terminate) == 1) {
            return data->patient_stats[patient_id];
        } else if (ad.id != -1) {
            patient_process_admission(&ad, patient_id, data, sems);
            patient_send_admission(&ad, data, comm, sems);
        }
    }
}

void patient_receive_admission(struct admission* ad, int patient_id, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    if (*(data->terminate) == 1) {
        return;
    } else {
        semaphore_lock(sems->main_patient[patient_id].full);
        semaphore_lock(sems->main_patient[patient_id].mutex);
        read_patient_receptionist_buffer(&comm->main_patient[patient_id], data->buffers_size, ad);
        semaphore_unlock(sems->main_patient[patient_id].mutex);
        semaphore_unlock(sems->main_patient[patient_id].empty);
    }
}


void patient_process_admission(struct admission* ad, int patient_id, struct data_container* data, struct semaphores* sems) {
    ad->receiving_patient = patient_id;
    ad->status = 'P';
    semaphore_lock(sems->patient_stats_mutex); 
    data->patient_stats[patient_id]++;
    semaphore_unlock(sems->patient_stats_mutex); 
    data->results[ad->id] = *ad;
}

void patient_send_admission(struct admission* ad, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    semaphore_lock(sems->patient_receptionist->full);
    semaphore_lock(sems->patient_receptionist->mutex);
    write_patient_receptionist_buffer(comm->patient_receptionist, data->buffers_size, ad);
    semaphore_unlock(sems->patient_receptionist->mutex);
    semaphore_unlock(sems->patient_receptionist->empty);
}