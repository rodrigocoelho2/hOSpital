/*

* Nº grupo: 19

* David Cândido, 59857

* Diogo Pereira, 59829

* Rodrigo Coelho, 60744

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "../include/memory.h"
#include "../include/main.h"
#include <unistd.h>

void* create_shared_memory(char* name, int size) {
    char shm_name[27] = "/";
    strcat(shm_name, name);
    shm_unlink(shm_name);
    int fd = shm_open(shm_name, O_RDWR | O_CREAT, S_IRGRP | S_IWGRP);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }
    if (ftruncate(fd, size) == -1) {
        perror("ftruncate");
        exit(2);
    }
    void* ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(3);
    }
    return ptr;
}

void* allocate_dynamic_memory(int size){
    return calloc(1, size);
}

void destroy_shared_memory(char* name, void* ptr, int size) {
    if (munmap(ptr, size) == -1) {
        perror("munmap");
        exit(1);
    }
    if (shm_unlink(name) == -1) {
        perror("shm_unlink");
        exit(1);
    }
}

void deallocate_dynamic_memory(void* ptr) {
    free(ptr);
}

void write_main_patient_buffer(struct circular_buffer* buffer, int buffer_size, struct admission* ad) {
    int next = (buffer->ptrs->in + 1) % buffer_size;
    if (next != buffer->ptrs->out) {
        buffer->buffer[buffer->ptrs->in] = *ad;
        buffer->ptrs->in = next;
    } else {
        fprintf(stderr, "Buffer is full, cannot write\n");
    }
}

void write_patient_receptionist_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct admission* ad) {
    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 0) {
            buffer->buffer[i] = *ad;
            buffer->ptrs[i] = 1;
            break;
        }
    }
}

void write_receptionist_doctor_buffer(struct circular_buffer* buffer, int buffer_size, struct admission* ad) {
    int next = (buffer->ptrs->in + 1) % buffer_size;
    if (next != buffer->ptrs->out) {
        buffer->buffer[buffer->ptrs->in] = *ad;
        buffer->ptrs->in = next;
    }
}

void read_main_patient_buffer(struct circular_buffer* buffer, int patient_id, int buffer_size, struct admission* ad) {
    if (buffer->ptrs->out != buffer->ptrs->in) {
        if (buffer->buffer[buffer->ptrs->out].id == patient_id) {
            *ad = buffer->buffer[buffer->ptrs->out];
            buffer->ptrs->out = (buffer->ptrs->out + 1) % buffer_size;
        } else {
            ad->id = -1;
        }
    } else {
        ad->id = -1;
    }
}

void read_patient_receptionist_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct admission* ad) {
    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 1) {
            *ad = buffer->buffer[i];
            buffer->ptrs[i] = 0;
            break;
        } else {
            ad->id = -1;
        }
    }
}

void read_receptionist_doctor_buffer(struct circular_buffer* buffer, int doctor_id, int buffer_size, struct admission* ad) {
    if (buffer->ptrs->out != buffer->ptrs->in) {
        if (buffer->buffer[buffer->ptrs->out].id == doctor_id) {
            *ad = buffer->buffer[buffer->ptrs->out];
            buffer->ptrs->out = (buffer->ptrs->out + 1) % buffer_size;
        } else {
            ad->id = -1;
        }
    } else {
        ad->id = -1;
    }
}
