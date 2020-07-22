#include "../philo_three.h"

#define EXIT_EATEN_ENOUGH 0
#define EXIT_DEATH 1
#define EXIT_ERROR 2

#define ERROR_MUTEX 1
#define ERROR_GETTIMEOFDAY 2
#define ERROR_MALLOC 3
#define ERROR_FORK 4
#define ERROR_TIMES_TO_EAT 5
#define ERROR_USLEEP 6
#define ERROR_SEM_OPEN 7
#define ERROR_SEM_UNLINK 8
#define ERROR_AC 9

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
	long long *time_last_meal;
	// int phi_died;
	int *n_times_eaten;
	int *phi_pid;
	int error_encountered;
} t_tab;

void *return_error(t_tab *tab, int error_num)
{
	tab->error_encountered = 1;
	write(2, B_RED"ERROR: "RESET, 19);
	if (error_num == ERROR_MUTEX)
		write(2, "pthread_mutex_(un)lock() returned -1\n", 38);
	else if (error_num == ERROR_GETTIMEOFDAY)
		write(2, "gettimeofday() returned -1\n", 28);
	else if (error_num == ERROR_MALLOC)
		write(2, "malloc() returned NULL\n", 24);
	else if (error_num == ERROR_USLEEP)
		write(2, "usleep() returned -1\n", 22);
	else if (error_num == ERROR_TIMES_TO_EAT)
		write(2, "number_of_times_each_philosopher_must_eat can't be 0\n", 54);
	else if (error_num == ERROR_FORK)
		write(2, "fork() returned < 0\n", 21);
	else if (error_num == ERROR_SEM_OPEN)
		write(2, "sem_open() failed\n", 19);
	else if (error_num == ERROR_SEM_UNLINK)
		write(2, "sem_unlink() returned -1\n", 26);
	else if (error_num == ERROR_AC)
		write(2, "too few or too many arguments\n", 31);
	return (NULL);
}

long long get_current_time(t_tab *tab)
{
	struct timeval tp;
	long long passed_time;

	if (gettimeofday(&tp, 0) == -1)
		return ((long long)return_error(tab, ERROR_GETTIMEOFDAY));
	passed_time = tp.tv_sec;
	passed_time *= 1000;
	passed_time += (tp.tv_usec / 1000);
	return (passed_time);
}

/*
Philosophers may die of starvation while standing in the dining room queue (semaphore) and it is important to report on their unfortunate demise within 10ms of its occurrance and halt the program immediately afterwards. But while they are waiting, they are incapable of doing anything else, and the other philosophers or the parent process also isn't aware of whether or not the philosopher has actually starved. Therefore a pthread is created within the child process to keep an eye on things.
*/

void *death_signaller(void *arg)
{
	t_tab *tab;
	tab = (t_tab *)arg;

	while (1)
	{
		if (usleep(1000) == -1)
			exit(EXIT_ERROR);
		if ((tab->current_time = get_current_time(tab)) == -1)
			exit(EXIT_ERROR);
		if (tab->time_last_meal[tab->phi_n] + tab->time_to_die <= tab->current_time)
		{
			if (!(put_status_msg((tab->current_time - tab->start_time), tab->phi_n + 1, B_RED"died\n"RESET)))
				exit(EXIT_ERROR);
			// tab->phi_died = 1;
			exit(EXIT_DEATH);
		}
	}
}

/*
difference with philo_one and philo_two
- don't share the struct
- Here, the semaphore is like a dining room. The room can only hold so many people. So when the room is full, any new philosopher that wants to enter, has to wait outside. This means that a philosopher (process) makes use of (part of) the semaphore for the entire duration of it's meal, instead of only for the time that it takes to see if there are any forks and to possible grab one or two. And other philosophers (processes) that the semaphore can no longer accommodate, are held up at the dining room entrance queue for as long as it takes for space to become available.
- Because of this the current time for the status message has to be gotten both before eating (for messages that are posted before) as well as after (for messages that are posted after), because eating itself takes a long time.
*/

void phi_f(void *arg)
{
	t_tab *tab = (t_tab *)arg;
	pthread_t death_signaller_t;
	int phi_state;
	long long time_sleep_start;
	long long current_time;
	
	if ((tab->current_time = get_current_time(tab)) == -1)
		exit(EXIT_ERROR);
	phi_state = 't';
	time_sleep_start = tab->current_time;
	tab->time_last_meal[tab->phi_n] = tab->current_time;
	if (pthread_create(&death_signaller_t, NULL, death_signaller, tab) != 0)
		exit(EXIT_ERROR);
	while(1)
	{
		if (phi_state == 't')
		{
			if (sem_wait(tab->fork_availability) == -1)
				exit(EXIT_ERROR);
			if (!(put_status_msg((tab->current_time - tab->start_time), tab->phi_n + 1, "is eating\n")))
				exit(EXIT_ERROR);
			tab->time_last_meal[tab->phi_n] = tab->current_time;
			if (usleep(tab->time_to_eat * 1000) == -1)
				exit(EXIT_ERROR);
			// CHECK if philo has eaten enough times yet
			tab->n_times_eaten[tab->phi_n]++;
			if (tab->number_of_times_each_philosopher_must_eat != -1
			&& tab->n_times_eaten[tab->phi_n]
			>= tab->number_of_times_each_philosopher_must_eat)
			{
				// TESTING ----------------------------------------------------
				printf(GREEN"%lld %d has fattened up enough\n"RESET, (tab->current_time - tab->start_time), tab->phi_n + 1);
				// ------------------------------------------------------------
				if (sem_post(tab->fork_availability) == -1)
					exit(EXIT_ERROR);
				exit(EXIT_EATEN_ENOUGH);
			}
			phi_state = 's';
			time_sleep_start = tab->current_time;
			if (!(put_status_msg((tab->current_time - tab->start_time), tab->phi_n + 1, "is sleeping\n")))
				exit(EXIT_ERROR);
			if (sem_post(tab->fork_availability) == -1)
				exit(EXIT_ERROR);
		}
		if (phi_state == 's' && time_sleep_start + tab->time_to_sleep < tab->current_time)
		{
			phi_state = 't';
			if (!(put_status_msg((tab->current_time - tab->start_time), tab->phi_n + 1, "is thinking\n")))
				exit(EXIT_ERROR);
		}
		// if (usleep(5000) == -1)
		// 	exit(EXIT_ERROR);
	}
}

	// CHECK number of arguments
	// DECLARE struct and initialize variables
	// tab.number_of_times_each_philosopher_must_eat = -1; // sentinel value for absence of value

int initialize_variables_part_2(t_tab *tab)
{
	if (sem_unlink("fork_availability") == -1)
		return((int)return_error(tab, ERROR_SEM_UNLINK));
	if ((tab->fork_availability = sem_open("fork_availability", O_CREAT, 0644, tab->number_of_philosophers / 2)) == SEM_FAILED)
		return ((int)return_error(tab, ERROR_SEM_OPEN));
	if (!(tab->n_times_eaten = malloc(sizeof(int) * tab->number_of_philosophers)))
		return ((int)return_error(tab, ERROR_MALLOC));
	int i = -1;
	while (++i < tab->number_of_philosophers)
		tab->n_times_eaten[i] = 0;
	if ((tab->start_time = get_current_time(tab)) == -1)
		return (0);
	if (!(tab->time_last_meal = malloc(sizeof(long long) * tab->number_of_philosophers)))
		return ((int)return_error(tab, ERROR_MALLOC));
	if (!(tab->phi_pid = malloc(sizeof(int) * tab->number_of_philosophers)))
		return ((int)return_error(tab, ERROR_MALLOC));
	return (1);
}

int initialize_variables(t_tab *tab, int ac, char **av)
{
	tab->number_of_philosophers = ft_atoi(av[1]);
	tab->time_to_die = ft_atoi(av[2]);
	tab->time_to_eat = ft_atoi(av[3]);
	tab->time_to_sleep = ft_atoi(av[4]);
	if (ac == 5)
		tab->number_of_times_each_philosopher_must_eat = -1;
	else if ((tab->number_of_times_each_philosopher_must_eat = ft_atoi(av[5])) == 0)
		return ((int)return_error(tab, ERROR_TIMES_TO_EAT));
	// tab->phi_died = 0;
	initialize_variables_part_2(tab);

	// TESTING ----------------------------------------------------------------
	// write(1, UNDERLINE"Program Configurations:\n"RESET, 33);
	// printf("number_of_philosophers: %d\n", tab.number_of_philosophers);
	// printf("time_to_die: %d milliseconds\n", tab.time_to_die);
	// printf("time_to_eat: %d milliseconds\n", tab.time_to_eat);
	// printf("time_to_sleep: %d milliseconds\n", tab.time_to_sleep);
	// printf("number_of_times_each_philosopher_must_eat: %d\n", tab.number_of_times_each_philosopher_must_eat);
	// ------------------------------------------------------------------------

	return (1);
}

int monitor_child_processes(t_tab *tab)
{
	int philosophers_left;
	int exit_status;
	pid_t pid;

	philosophers_left = tab->number_of_philosophers;
	while (philosophers_left--)
	{
		pid = wait(&exit_status);
		if (WEXITSTATUS(exit_status) == EXIT_DEATH)
		{
			while (tab->number_of_philosophers--)
			{
				// printf(RED"kill'ing philosopher with pid %d\n"RESET, tab->phi_pid[tab->number_of_philosophers]);
				kill(tab->phi_pid[tab->number_of_philosophers], SIGKILL);

			}
			// printf("all philosophers kill'ed\n");
			break;
		}
		// As soon as wait() returns with a pid, we know a philospopher has exited, either because he's fat or because he's dead. If he's fat, we let the other philosopher's continue; if he's dead, we kill the other philosophers. In order to kill the other philosophers we have to 
	}
	return (1);
}

int main(int ac, char **av)
{
	t_tab tab;
	int i;
	if (ac < 5 || ac > 6)
		return ((int)return_error(&tab, ERROR_AC));
	initialize_variables(&tab, ac, av);
	i = -1;
	while (++i < tab.number_of_philosophers)
	{
		tab.phi_n = i;
		pid_t fork_ret = fork();
		if (fork_ret == 0)
			phi_f(&tab);
		else if (fork_ret > 0)
			tab.phi_pid[i] = fork_ret;
		else
			return ((int)return_error(&tab, ERROR_FORK));
	}

	monitor_child_processes(&tab);
	sem_unlink("fork_availability");
	return (1);
}
