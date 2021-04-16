#ifndef PHILO_THREE_H
# define PHILO_THREE_H

# include <stdio.h>
# include <unistd.h>
# include <pthread.h>
# include <stdlib.h>
# include <sys/time.h>
# include <semaphore.h> 
# include <signal.h>

# define B_RED "\033[1;31m"
# define B_GREEN "\033[1;32m"
# define CYAN "\033[0;36m"
# define RESET "\033[0m"

# define EXIT_EATEN_ENOUGH 0
# define EXIT_DEATH 1
# define EXIT_ERROR 2

# define ERROR_AC 9
# define ERROR_BAD_ARGS 5
# define ERROR_MALLOC 3
# define ERROR_GETTIMEOFDAY 2
# define ERROR_USLEEP 6
# define ERROR_FORK 4
# define ERROR_CHILD 10
# define ERROR_MUTEX 1
# define ERROR_SEM_OPEN 7
# define ERROR_SEM_UNLINK 8

typedef struct s_tab
{
	long long	start_time;
	long long	current_time;
	int			phi_n;
	int			number_of_philosophers;
	int			time_to_die;
	int			time_to_eat;
	int			time_to_sleep;
	int			number_of_times_each_philosopher_must_eat;
	sem_t		*fork_availability;
	int			*phi_pid;
	int			times_eaten;
	long long	time_last_meal;
}				t_tab;

// initialize_variables.c
int				initialize_variables(t_tab *tab, int ac, char **av);

// utils.c
int				ft_atoi(const char *str);
void			*return_error(t_tab *tab, int error_num);
long long		get_current_time(t_tab *tab);
void			free_malloced_variables(t_tab *tab);

#endif
