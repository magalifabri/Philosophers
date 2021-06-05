#ifndef PHILO_ONE_H
# define PHILO_ONE_H

# include <stdio.h>
# include <unistd.h>
# include <pthread.h>
# include <stdlib.h>
# include <sys/time.h>

# define B_RED "\033[1;31m"
# define B_GREEN "\033[1;32m"
# define RESET "\033[0m"

# define ERROR_AC 1
# define ERROR_BAD_ARGS 2
# define ERROR_MALLOC 3
# define ERROR_GETTIMEOFDAY 4
# define ERROR_USLEEP 5
# define ERROR_PTHREAD_CREATE 6
# define ERROR_PTHREAD_JOIN 13
# define ERROR_MUTEX_LOCK 7
# define ERROR_MUTEX_UNLOCK 9
# define ERROR_MUTEX_INIT 8
# define ERROR_MUTEX_DESTROY 10
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
	t_frk			*forks;
	pthread_mutex_t	print_lock;
	int				print_lock_initialized;
	pthread_mutex_t	id_lock;
	int				id_lock_initialized;
	int				n_fork_locks_initialized;

	long long		start_time;
	long long		current_time;
	int				phi_n;
	int				number_of_philosophers;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				number_of_times_each_philosopher_must_eat;
	pthread_t		philosopher_thread;
	int				pthreads_created;
	int				phi_n_c;
	int				number_of_fat_philosophers;
	int				*n_times_eaten;
	int				exit_code;
	long long		*time_last_meal;
}					t_tab;

// initialize_variables.c
void				pre_initialisation(t_tab *tab);
int					initialize_variables(t_tab *tab, int ac, char **av);

// phi_f.c
void				*phi_f(void *arg);

// utils_1.c
int					mutex_unlock__return_0(t_tab *tab, pthread_mutex_t *lock_1,
						pthread_mutex_t *lock_2, int return_value);

int					put_status(t_tab *tab, int phi_n, char *msg);
void				*set_exit_code(t_tab *tab, int exit_code);
int					wrap_up(t_tab *tab);

// utils_2.c
long long			get_current_time(void);
int					ft_atoi(const char *str);

#endif
