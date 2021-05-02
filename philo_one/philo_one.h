#ifndef PHILO_ONE_H
# define PHILO_ONE_H

# include <stdio.h>
# include <unistd.h>
# include <pthread.h>
# include <stdlib.h>
# include <sys/time.h>

# define B_RED "\033[1;31m"
# define B_GREEN "\033[1;32m"
# define CYAN "\033[0;36m"
# define RESET "\033[0m"

# define ERROR_MUTEX_LOCK 1
# define ERROR_MUTEX_UNLOCK 2
# define ERROR_MUTEX_INIT 8
# define ERROR_MUTEX_DESTROY 10
# define ERROR_PTHREAD_CREATE 9
# define ERROR_PTHREAD_JOIN 13
# define ERROR_GETTIMEOFDAY 3
# define ERROR_MALLOC 4
# define ERROR_BAD_ARGS 5
# define ERROR_AC 6
# define ERROR_USLEEP 7
# define DEATH 11
# define ALL_FAT 12

// struct for variables related to the forks (cutlery)
typedef struct s_frk
{
	int				available;
	pthread_mutex_t	lock;
}					t_frk;

// main struct for variables used all throughout the source code
typedef struct s_tab
{
	long long		start_time;
	long long		current_time;
	int				phi_n;
	int				phi_n_c;
	int				number_of_philosophers;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				number_of_times_each_philosopher_must_eat;
	t_frk			*forks;
	int				n_fork_locks_initialized;
	int				number_of_fat_philosophers;
	int				*n_times_eaten;
	int				exit_code;
	pthread_mutex_t	print_lock;
	int				print_lock_initialized;
	pthread_mutex_t	id_lock;
	int				id_lock_initialized;
	pthread_mutex_t	eating_lock;
	int				eating_lock_initialized;
	pthread_t		philosopher_thread;
	int				pthreads_created;
}					t_tab;

// struct for variables used in phi_f() (the threads)
typedef struct s_thread_variable_struct
{
	int				phi_n;
	int				left_fork_i;
	long long		time_last_meal;
	t_tab			*tab;
	int				eating;
}					t_thread_var_struct;

// initialize_variables.c
void				pre_initialisation(t_tab *tab);
void				initialize_malloc_and_mutex_indicators(t_tab *tab);
int					initialize_variables_and_locks(t_tab *tab, int ac,
						char **av);
int					initialize_variables_phi_f(t_tab *tab,
						t_thread_var_struct *s);

// main.c
int					return_error(t_tab *tab, int error_num);
void				*set_exit_code(t_tab *tab, int exit_code);

// phi_f.c
void				*phi_f(void *arg);

// utils_1.c
int					put_status(t_tab *tab, int philo_n, char *msg);
long long			get_current_time(t_tab *tab);
int					destroy_locks(t_tab *tab);
void				free_malloced_variables(t_tab *tab);
int					wrap_up(t_tab *tab);

// utils_2.c
int					mutex_unlock__return_0(t_tab *tab, pthread_mutex_t *lock_1,
						pthread_mutex_t *lock_2, int return_value);
int					ft_atoi(const char *str);

#endif
