#ifndef HOSPSIGNAL_H
#define HOSPSIGNAL_H

#include <signal.h>

void signal_handler_main(int signum);
void alarm_print_status(int signum);

#endif /* HOSPSIGNAL_H */