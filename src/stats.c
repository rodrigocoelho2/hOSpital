/*
* Nº grupo: 19
* David Cândido, 59857
* Diogo Pereira, 59829
* Rodrigo Coelho, 60744
*/

#include "../include/stats.h"
#include "../include/main.h"
#include <string.h>

void initialize_stats(struct data_container* data) {
    for (int i = 0; i < data->n_patients; i++) {
        data->patient_stats[i] = 0; 
    }
}

void format_time(char *buffer, struct timespec *ts) {
    struct tm *tm_info;
    tm_info = localtime(&ts->tv_sec);
    strftime(buffer, 30, "%d/%m/%Y_%H:%M:%S", tm_info);
    sprintf(buffer + strlen(buffer), ".%03ld", ts->tv_nsec / 1000000);
}

void write_stats_to_file(struct data_container *data) {
    FILE *file = fopen(data->statistics_filename, "w");
    if (file == NULL) {
        perror("Error opening statistics file");
        return;
    }

    fprintf(file, "Process Statistics:\n");
    for (int i = 0; i < data->n_patients; i++) {
        fprintf(file, "Patient %d requested %d admissions!\n", data->patient_pids[i], data->patient_stats[i]);
    }
    for (int i = 0; i < data->n_receptionists; i++) {
        fprintf(file, "Receptionist %d forwarded %d admissions!\n", data->receptionist_pids[i], data->receptionist_stats[i]);
    }
    for (int i = 0; i < data->n_doctors; i++) {
        fprintf(file, "Doctor %d processed %d admissions!\n", data->doctor_pids[i], data->doctor_stats[i]);
    }

    fprintf(file, "Admission Statistics:\n");
    for (int i = 0; i < data->max_ads; i++) {
        if (data->results[i].status != '\0') {
            char start_time_str[30], patient_time_str[30], receptionist_time_str[30], doctor_time_str[30];
            format_time(start_time_str, &data->results[i].create_time);
            format_time(patient_time_str, &data->results[i].patient_time);
            format_time(receptionist_time_str, &data->results[i].receptionist_time);
            format_time(doctor_time_str, &data->results[i].doctor_time);

            double total_time = difftime(data->results[i].doctor_time.tv_sec, data->results[i].create_time.tv_sec) 
                                + (data->results[i].doctor_time.tv_nsec - data->results[i].create_time.tv_nsec) / 1e9;

            fprintf(file, "Admission: %d\n", data->results[i].id);
            fprintf(file, "Status: %c\n", data->results[i].status);
            fprintf(file, "Patient id: %d\n", data->results[i].requesting_patient);
            fprintf(file, "Receptionist id: %d\n", data->results[i].receiving_receptionist);
            fprintf(file, "Doctor id: %d\n", data->results[i].receiving_doctor);
            fprintf(file, "Start time: %s\n", start_time_str);
            fprintf(file, "Patient time: %s\n", patient_time_str);
            fprintf(file, "Receptionist time: %s\n", receptionist_time_str);
            fprintf(file, "Doctor time: %s\n", doctor_time_str);
            fprintf(file, "Total Time: %.3f\n", total_time);
        }
    }

    fclose(file);
}
