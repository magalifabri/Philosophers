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

#define EXIT_EATEN_ENOUGH 0
#define EXIT_DEATH 1
#define EXIT_ERROR 2

#define ERROR_MUTEX 1
#define ERROR_GETTIMEOFDAY 2
#define ERROR_MALLOC 3
#define ERROR_FORK 4
#define ERROR_BAD_ARGS 5
#define ERROR_USLEEP 6
#define ERROR_SEM_OPEN 7
#define ERROR_SEM_UNLINK 8
#define ERROR_AC 9
#define ERROR_CHILD 10

typedef struct s_tab
{
	long long start_time;
	long long current_time;
	int phi_n;
	int number_of_philosophers;
	int time_to_die;   // time in ms before the next meal needs to start
	int time_to_eat;   // duration in ms that the philo will spend eating
	int time_to_sleep; // duration in ms that the philosopher will spend sleeping
	int number_of_times_each_philosopher_must_eat;
	sem_t *fork_availability;
	int *phi_pid;
	int times_eaten;
	long long time_last_meal;
	int malloc_phi_pid;
} t_tab;

int			ft_atoi(const char *str);
// char *concatenate_strings(int num, ...);
int put_status_msg(long long time, int phi_n, char *message);
int initialize_variables(t_tab *tab, int ac, char **av);
void initialize_malloc_indicators(t_tab *tab);
void *return_error(t_tab *tab, int error_num);
long long get_current_time(t_tab *tab);
void free_malloced_variables(t_tab *tab);

#endif