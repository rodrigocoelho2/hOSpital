#ifndef STATS_H
#define STATS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/main.h" 

/* Função que inicializa as estatísticas na estrutura data_container.
* 
* @param data Ponteiro para a estrutura data_container que será inicializada.
*/
void initialize_stats(struct data_container* data);

/* Função que escreve as estatísticas da estrutura data_container em um arquivo.
* 
* @param data Ponteiro para a estrutura data_container que contém as estatísticas a serem escritas.
*/
void write_stats_to_file(struct data_container *data);

/* Função que formata o tempo de uma estrutura timespec em uma string.
* 
* @param buffer Buffer onde o tempo formatado será armazenado.
* @param ts Ponteiro para a estrutura timespec que contém o tempo a ser formatado.
*/
void format_time(char *buffer, struct timespec *ts);

#endif /* STATS_H */