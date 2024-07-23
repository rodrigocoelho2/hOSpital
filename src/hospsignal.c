/*
* Nº grupo: 19
* David Cândido, 59857
* Diogo Pereira, 59829
* Rodrigo Coelho, 60744
*/

#include "../include/hospsignal.h"
#include <stdio.h>
#include <signal.h>

void set_timer(int intervalo, void (*handler)(int)) {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("Erro ao definir temporizador");
    }
    alarm(intervalo);
}

void set_intr_handler(void (*handler)(int)) {
    if (signal(SIGINT, handler) == SIG_ERR) {
        perror("Erro ao definir manipulador para SIGINT");
    }
}