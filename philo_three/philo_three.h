#ifndef PHILO_THREE_H
#define PHILO_THREE_H

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h> // required for gettimeofday()
#include <semaphore.h> 
#include <signal.h> // required for kill()

#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"
#define RED "\033[0;31m"	
#define B_RED "\033[1;31m"	
#define GREEN "\033[0;32m"	
#define B_GREEN "\033[1;32m"	
#define YELLOW "\033[0;33m"	
#define B_YELLOW "\033[01;33m"	
#define BLUE "\033[0;34m"	
#define B_BLUE "\033[1;34m"	
#define MAGENTA "\033[0;35m"	
#define B_MAGENTA "\033[1;35m"	
#define CYAN "\033[0;36m"	
#define B_CYAN "\033[1;36m"	
#define RESET "\033[0m"	

int			ft_atoi(const char *str);
// char *concatenate_strings(int num, ...);
int put_status_msg(long long time, int phi_n, char *message);

#endif