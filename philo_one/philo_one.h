#ifndef PHILO_ONE_H
# define PHILO_ONE_H

# include <stdio.h>
# include <unistd.h>
# include <pthread.h>
# include <stdlib.h>
# include <sys/time.h>
// # include <stdarg.h> // required by put_status_msg()

# define RED "\033[0;31m"	
# define B_RED "\033[1;31m"	
# define GREEN "\033[0;32m"	
# define B_GREEN "\033[1;32m"	
# define YELLOW "\033[0;33m"	
# define B_YELLOW "\033[01;33m"	
# define BLUE "\033[0;34m"	
# define B_BLUE "\033[1;34m"	
# define MAGENTA "\033[0;35m"	
# define B_MAGENTA "\033[1;35m"	
# define CYAN "\033[0;36m"	
# define B_CYAN "\033[1;36m"	
# define RESET "\033[0m"	

# define ERROR_MUTEX_LOCK 1
# define ERROR_MUTEX_UNLOCK 2
# define ERROR_MUTEX_INIT 8
# define ERROR_MUTEX_DESTROY 10
# define ERROR_PTHREAD_CREATE 9
# define ERROR_GETTIMEOFDAY 3
# define ERROR_MALLOC 4
# define ERROR_BAD_ARGS 5
# define ERROR_AC 6
# define ERROR_USLEEP 7

// struct for variables used in phi_f() (the threads)
typedef struct		s_thread_variable_struct
{
	int				phi_n;
	int				left_fork_held;
	int				right_fork_held;
	int				phi_state;
	long long		time_last_meal;
	long long		time_sleep_start;
}					t_thread_var_struct;

// struct for variables related to the forks (cutlery)
typedef struct		s_frk
{
	int				available;
	pthread_mutex_t	lock;
}					t_frk;

// main struct for variables used all throughout the source code
typedef struct		s_tab
{
	long long		start_time;
	long long		current_time;
	int				phi_n;
	int				number_of_philosophers;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				number_of_times_each_philosopher_must_eat;
	t_frk 			*forks;
	int				phi_died;
	int				*n_times_eaten;
	int				error_encountered;
	pthread_t		*phi_t;
	int				mutexes_initialized;
}					t_tab;

// main.c
void				*return_error(t_tab *tab, int error_num);

// initialize_variables.c
void				initialize_malloc_and_mutex_indicators(t_tab *tab);
int					initialize_variables_and_locks(t_tab *tab, int ac, char **av);
void				initialize_variables_phi_f(t_tab *tab, t_thread_var_struct *s);

// phi_f.c
void				*phi_f(void *arg);
int					eating_to_thinking(t_tab *tab, t_thread_var_struct *s);

// utils.c
long long			get_current_time(t_tab *tab);
int					ft_atoi(const char *str);
int					destroy_locks(t_tab *tab);
void				free_malloced_variables(t_tab *tab);

#endif