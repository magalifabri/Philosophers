#include "../philo_three.h"

void free_malloced_variables(t_tab *tab)
{
	if (tab->malloc_phi_pid)
		free(tab->phi_pid);
	// if (tab->malloc_n_times_eaten)
	// 	free(tab->n_times_eaten);
	if (tab->malloc_time_last_meal)
		free(tab->time_last_meal);
}

void *return_error(t_tab *tab, int error_num)
{
	write(2, B_RED"ERROR: "RESET, 19);
	if (error_num == ERROR_MUTEX)
		write(2, "pthread_mutex_(un)lock() returned -1\n", 38);
	else if (error_num == ERROR_GETTIMEOFDAY)
		write(2, "gettimeofday() returned -1\n", 28);
	else if (error_num == ERROR_MALLOC)
		write(2, "malloc() returned NULL\n", 24);
	else if (error_num == ERROR_USLEEP)
		write(2, "usleep() returned -1\n", 22);
	else if (error_num == ERROR_BAD_ARGS)
		write(2, "bad arguments. Try again.\n", 27);
	else if (error_num == ERROR_FORK)
		write(2, "fork() returned < 0\n", 21);
	else if (error_num == ERROR_SEM_OPEN)
		write(2, "sem_open() failed\n", 19);
	else if (error_num == ERROR_SEM_UNLINK)
		write(2, "sem_unlink() returned -1\n", 26);
	else if (error_num == ERROR_AC)
		write(2, "too few or too many arguments\n", 31);
	free_malloced_variables(tab);
	sem_unlink("fork_availability");
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
			tab->times_eaten++;
			if (tab->number_of_times_each_philosopher_must_eat != -1 &&
			tab->times_eaten >= tab->number_of_times_each_philosopher_must_eat)
			{
				// TESTING ----------------------------------------------------
				if (!(put_status_msg((tab->current_time - tab->start_time),
				tab->phi_n + 1, B_GREEN"is fat enough\n"RESET)))
					exit(EXIT_ERROR);
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
	}
}

	// CHECK number of arguments
	// DECLARE struct and initialize variables
	// tab.number_of_times_each_philosopher_must_eat = -1; // sentinel value for absence of value



// As soon as wait() returns with a pid, we know a philospopher has exited, either because he's fat or because he's dead. If he's fat, we let the other philosopher's continue; if he's dead, we kill the other philosophers. In order to kill the other philosophers we have to 

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
				kill(tab->phi_pid[tab->number_of_philosophers], SIGKILL);
			break;
		}
		// if (WEXITSTATUS(exit_status) == EXIT_EATEN_ENOUGH)
		// 	write(1, B_GREEN"is fat enough\n"RESET, 26);
	}
	if (philosophers_left == -1)
		write(1, B_GREEN"Good job. They're all fat.\n"RESET, 39);
	else
		write(1, B_RED"One starved. The rest murdered.\n"RESET, 42);
	return (1);
}

int main(int ac, char **av)
{
	t_tab tab;
	int i;

	initialize_malloc_indicators(&tab);
	if (ac < 5 || ac > 6)
		return ((int)return_error(&tab, ERROR_AC));
	if (!initialize_variables(&tab, ac, av))
		return (0);
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
	if (sem_unlink("fork_availability") == -1)
		return ((int)return_error(&tab, ERROR_SEM_UNLINK));
	free_malloced_variables(&tab);
	return (1);
}
