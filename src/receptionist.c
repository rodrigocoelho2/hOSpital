/*
* Nº grupo: 19
* David Cândido, 59857
* Diogo Pereira, 59829
* Rodrigo Coelho, 60744
*/

#include <stdio.h>
#include <stdlib.h>
#include "../include/receptionist.h"
#include "../include/memory.h"
#include "../include/synchronization.h"

int execute_receptionist(int receptionist_id, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    int admissions_processed = 0;
    struct admission ad;

    while (1) {
        receptionist_receive_admission(&ad, data, comm, sems);
        if (*(data->terminate) == 1) {
            break;
        } else if (ad.id != -1) {
            receptionist_process_admission(&ad, receptionist_id, data, sems);
            receptionist_send_admission(&ad, data, comm, sems);
            admissions_processed++;
        }
    }

    return admissions_processed;
}

void receptionist_receive_admission(struct admission* ad, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    if (*(data->terminate) == 1) {
        return;
    }

    semaphore_lock(sems->patient_receptionist->full);
    semaphore_lock(sems->patient_receptionist->mutex);
    read_patient_receptionist_buffer(comm->patient_receptionist, data->buffers_size, ad);
    semaphore_unlock(sems->patient_receptionist->mutex);
    semaphore_unlock(sems->patient_receptionist->empty);
}

void receptionist_process_admission(struct admission* ad, int receptionist_id, struct data_container* data, struct semaphores* sems) {
    ad->receiving_receptionist = receptionist_id;
    ad->status = 'R';
    
    semaphore_lock(sems->receptionist_stats_mutex);
    data->receptionist_stats[receptionist_id]++;
    semaphore_unlock(sems->receptionist_stats_mutex);
    
    for (int i = 0; i < data->max_ads; i++) {
        if (data->results[i].id == ad->id) {
            data->results[i].receiving_receptionist = receptionist_id;
            data->results[i].status = 'R';
            break;
        }
    }
}

void receptionist_send_admission(struct admission* ad, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    semaphore_lock(sems->receptionist_doctor->full);
    semaphore_lock(sems->receptionist_doctor->mutex);
    write_receptionist_doctor_buffer(comm->receptionist_doctor, data->buffers_size, ad);
    semaphore_unlock(sems->receptionist_doctor->mutex);
    semaphore_unlock(sems->receptionist_doctor->empty);
}