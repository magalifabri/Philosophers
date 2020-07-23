#ifndef PHILO_ONE_H
#define PHILO_ONE_H

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h> // required for gettimeofday()

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

#define ERROR_MUTEX_LOCK 1
#define ERROR_MUTEX_UNLOCK 2
#define ERROR_MUTEX_INIT 8
#define ERROR_PTHREAD_CREATE 9
#define ERROR_GETTIMEOFDAY 3
#define ERROR_MALLOC 4
#define ERROR_BAD_ARGS 5
#define ERROR_AC 6
#define ERROR_USLEEP 7

typedef struct s_frk
{
	int available;
	pthread_mutex_t lock;
} t_frk;

typedef struct s_tab
{
	long long start_tp;
	int phi_n;
	int number_of_philosophers;
	int time_to_die;   // time in ms before the next meal needs to start
	int time_to_eat;   // duration in ms that the philo will spend eating
	int time_to_sleep; // duration in ms that the philosopher will spend sleeping
	int number_of_times_each_philosopher_must_eat;
	t_frk *forks;
	int phi_died;
	int *n_times_eaten;
	int error_encountered;
} t_tab;

int put_status_msg(long long time, int phi_n, char *message);
int ft_atoi(const char *str);
long long get_current_time(t_tab *tab);
void *return_error(t_tab *tab, int error_num);


#endif