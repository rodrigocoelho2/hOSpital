/*
* Nº grupo: 19
* David Cândido, 59857
* Diogo Pereira, 59829
* Rodrigo Coelho, 60744
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

#include "../include/synchronization.h"

/* Função que cria um novo semáforo com nome name e valor inicial igual a
* value. Pode concatenar o resultado da função getpid() a name, para tornar
* o nome único para o processo.
*/
sem_t * semaphore_create(char* name, int value) {
    sem_unlink(name);
    char nameWithId[strlen(name) + 12];
    sprintf(nameWithId, "/%s_%d", name, getpid());
    sem_t* sem = sem_open(nameWithId, O_CREAT | O_EXCL, 0644, value); 
    if (sem == SEM_FAILED) {
        return NULL;
    }
    return sem;
}

/* Função que destroi o semáforo passado em argumento.
*/
void semaphore_destroy(char* name, sem_t* semaphore) {
    sem_close(semaphore);
    sem_unlink(name);
}

/* Função que inicia o processo de produzir, fazendo sem_wait nos semáforos
* corretos da estrutura passada em argumento.
*/
void produce_begin(struct prodcons* pc) {
    sem_wait(pc->empty);
    sem_wait(pc->mutex);
}

/* Função que termina o processo de produzir, fazendo sem_post nos semáforos
* corretos da estrutura passada em argumento.
*/
void produce_end(struct prodcons* pc) {
    sem_post(pc->mutex);
    sem_post(pc->full);
}

/* Função que inicia o processo de consumir, fazendo sem_wait nos semáforos
* corretos da estrutura passada em argumento.
*/
void consume_begin(struct prodcons* pc) {
    sem_wait(pc->full);
    sem_wait(pc->mutex);
}

/* Função que termina o processo de consumir, fazendo sem_post nos semáforos
* corretos da estrutura passada em argumento.
*/
void consume_end(struct prodcons* pc) {
    sem_post(pc->mutex);
    sem_post(pc->empty);
}

/* Função que faz wait a um semáforo.
*/
void semaphore_lock(sem_t* sem) {
    sem_wait(sem);
}

/* Função que faz post a um semáforo.
*/
void semaphore_unlock(sem_t* sem) {
    sem_post(sem);
}