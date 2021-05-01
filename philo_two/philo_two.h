#ifndef PHILO_TWO_H
# define PHILO_TWO_H

# include <stdio.h>
# include <unistd.h>
# include <pthread.h>
# include <stdlib.h>
# include <sys/time.h>
# include <semaphore.h>

# define B_RED "\033[1;31m"
# define B_GREEN "\033[1;32m"
# define RESET "\033[0m"

# define ERROR_AC 1
# define ERROR_BAD_ARGS 2
# define ERROR_MALLOC 3
# define ERROR_GETTIMEOFDAY 4
# define ERROR_USLEEP 5
# define ERROR_PTHREAD_CREATE 6
# define ERROR_SEM_OPEN 7
# define ERROR_SEM_UNLINK 8
# define ERROR_SEM_WAIT 9
# define ERROR_SEM_POST 10
# define DEATH 11
# define ALL_FAT 12

// main struct for variables used all throughout the source code
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
	pthread_t	philosopher_thread;
	sem_t		*fork_sem;
	sem_t		*id_sem;
	sem_t		*print_sem;
	int			phi_n_c;
	int			number_of_fat_philosophers;
	int			*n_times_eaten;
	int			exit_code;
	int			pthreads_created;
}				t_tab;

// struct for variables used in phi_f() (the threads)
typedef struct s_thread_variable_struct
{
	int			phi_n;
	long long	time_last_meal;
	t_tab		*tab;
}				t_thread_var_struct;

// initialize_variables.c
int				initialize_variables(t_tab *tab, int ac, char **av);

// main.c
long long		get_current_time(void);

// phi_f.c
void			*phi_f(void *arg);

// utils_1.c
int				abort_eating(t_tab *tab, sem_t *sem, int return_value,
					int exit_code);
int				put_status_msg(t_tab *tab, t_thread_var_struct *s, char *msg);
void			*set_exit_code(t_tab *tab, int exit_code);
void			free_malloced_variables(t_tab *tab);
void			wrap_up(t_tab *tab);

// utils_2.c
int				ft_atoi(const char *str);

#endif
