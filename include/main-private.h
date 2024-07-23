#ifndef MAIN_PRIVATE_H
#define MAIN_PRIVATE_H

// Mensagens de alarme
#define ALARM_MSG_BEGIN "[Alarm] Resultados das operações:\n"
#define ALARM_MSG_NOP "Nenhuma operação realizada ainda.\n"
#define ALARM_MSG_OPERATION "Operação %d -> Estado: %c\n"
#define ALARM_MSG_MAIN_CREATED " - Criada pelo processo principal em: %s\n"
#define ALARM_MSG_CLIENT_PROCESSED " - Processada pelo cliente %d em: %s\n"
#define ALARM_MSG_INTERM_PROCESSED " - Processada pelo intermediário %d em: %s\n"
#define ALARM_MSG_ENTERP_BOOKED " - Reservada na central em: %s\n"
#define ALARM_MSG_ENTERP_PROCESSED " - Processada na central pelo processo %d em: %s\n"

/* Função que imprime as instruções de uso do programa.
*/
void print_help();

/* Função que inicializa e aloca memória para a estrutura data_container.
* 
* @return Ponteiro para a estrutura data_container inicializada.
*/
struct data_container* initialize_data_container();

/* Função que inicializa e aloca memória para a estrutura communication.
* 
* @return Ponteiro para a estrutura communication inicializada.
*/
struct communication* initialize_communication();

/* Função que escreve as estatísticas finais do programa, como o número de operações realizadas.
* 
* @param data Ponteiro para a estrutura data_container que contém os dados a serem escritos.
*/
void write_statistics(struct data_container* data);

#endif /* MAIN_PRIVATE_H */