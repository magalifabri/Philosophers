#include "../philo_three.h"

#define EXIT_EATEN_ENOUGH 0
#define EXIT_DEATH 3
#define EXIT_ERROR 2

#define ERROR_MUTEX 1
#define ERROR_GETTIMEOFDAY 2
#define ERROR_MALLOC 3
#define ERROR_PTHREAD_CREATE 4
#define ERROR_TIMES_TO_EAT 5
// #define ERROR_

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
	int phi_died;
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
		write(2, "gettimeofday() returned -1\n", 38);
	return (NULL);
}

long long get_current_time(t_tab *tab)
{
	struct timeval tp;
	long long passed_time;

	if (gettimeofday(&tp, 0) == -1)
		return ((long long)return_error(tab, 2));
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
	// long long current_time;
	tab = (t_tab *)arg;

	while (1)
	{
		usleep(5000);
		if ((tab->current_time = get_current_time(tab)) == -1)
			exit(EXIT_ERROR);
		if (tab->time_last_meal[tab->phi_n] + tab->time_to_die <= tab->current_time)
		{
			if (!(put_status_msg((tab->current_time - tab->start_time), tab->phi_n + 1, B_RED"died, exiting\n"RESET)))
				exit(EXIT_ERROR);
			tab->phi_died = 1;
			exit(EXIT_DEATH);
		}
	}
}

/*
difference between philo_one and philo_two
- don't share the struct
- Here, the semaphore is like a dining room. The room can only hold so many people. So when the room is full, any new philosopher that wants to enter, has to wait outside. This means that a philosopher (process) makes use of (part of) the semaphore for the entire duration of it's meal, instead of only for the time that it takes to see if there are any forks and to possible grab one or two. And other philosophers (processes) that the semaphore can no longer accommodate, are held up at the dining room entrance queue for as long as it takes for space to become available.
- Because of this the current time for the status message has to be gotten both before eating (for messages that are posted before) as well as after (for messages that are posted after), because eating itself takes a long time.
*/

void phi_f(void *arg)
{
	t_tab *tab = (t_tab *)arg;

	// int phi_n = -1; // sentinel value for an uninitialized philosopher
	int phi_state;
	long long time_sleep_start;
	long long current_time;
	
	if ((current_time = get_current_time(tab)) == -1)
		exit(EXIT_ERROR);
	phi_state = 't'; // 's' = sleep, 't' = thinking, 'e' = eating
	time_sleep_start = current_time;
	tab->time_last_meal[tab->phi_n] = current_time; // let's be nice and assume the philo's start on a full stomach
	
	pthread_t death_signaller_t;
	pthread_create(&death_signaller_t, NULL, death_signaller, tab);

	// LIFE of a philosipher
	while(1)
	{
		if ((current_time = get_current_time(tab)) == -1)
			exit(EXIT_ERROR);

		// DETERMINE activity

		// THINKING: eat when <= half of the philosophers are eating (then enough forks are available)
		if (phi_state == 't')
		{
			if (sem_wait(tab->fork_availability) == -1)
				exit(EXIT_ERROR);
			if ((current_time = get_current_time(tab)) == -1)
				exit(EXIT_ERROR);
			tab->time_last_meal[tab->phi_n] = current_time;
			if (!(put_status_msg((current_time - tab->start_time), tab->phi_n + 1, "is eating\n")))
				exit(EXIT_ERROR);
			if (usleep(tab->time_to_eat * 1000) == -1)
				exit(EXIT_ERROR);
			if ((current_time = get_current_time(tab)) == -1)
				exit(EXIT_ERROR);
			// CHECK if philo has eaten enough times yet
			tab->n_times_eaten[tab->phi_n]++;
			if (tab->number_of_times_each_philosopher_must_eat != -1
			&& tab->n_times_eaten[tab->phi_n]
			>= tab->number_of_times_each_philosopher_must_eat)
			{
				// TESTING ----------------------------------------------------
				printf("%lld %d has fattened up enough\n", (current_time - tab->start_time), tab->phi_n + 1);
				// ------------------------------------------------------------
				if (sem_post(tab->fork_availability) == -1)
					exit(EXIT_ERROR);
				exit(EXIT_EATEN_ENOUGH);
			}
			phi_state = 's';
			time_sleep_start = current_time;
			if (!(put_status_msg((current_time - tab->start_time), tab->phi_n + 1, "is sleeping\n")))
				exit(EXIT_ERROR);
			if (sem_post(tab->fork_availability) == -1)
				exit(EXIT_ERROR);
		}

		// SLEEPING -> thinking, if time_to_sleep has elapsed
		if (phi_state == 's' && time_sleep_start + tab->time_to_sleep < current_time)
		{
			phi_state = 't';
			if (!(put_status_msg((current_time - tab->start_time), tab->phi_n + 1, "is thinking\n")))
				exit(EXIT_ERROR);
		}
		usleep(5000);
	}
	// exit(0);
}

	// CHECK number of arguments
	// DECLARE struct and initialize variables
	// tab.number_of_times_each_philosopher_must_eat = -1; // sentinel value for absence of value

int main(int ac, char **av)
{
	if (ac < 5 || ac > 6)
	{
		write(2, B_RED"ERROR: "RESET"too few or too many arguments\n", 49);
		return (0);
	}

	t_tab tab;
	tab.number_of_philosophers = ft_atoi(av[1]);
	tab.time_to_die = ft_atoi(av[2]);
	tab.time_to_eat = ft_atoi(av[3]);
	tab.time_to_sleep = ft_atoi(av[4]);
	if (ac == 5)
		tab.number_of_times_each_philosopher_must_eat = -1;
	else if ((tab.number_of_times_each_philosopher_must_eat = ft_atoi(av[5])) == 0)
		return ((int)return_error(&tab, ERROR_TIMES_TO_EAT));
	tab.phi_died = 0;
	
	// TESTING ----------------------------------------------------------------
	// write(1, UNDERLINE"Program Configurations:\n"RESET, 33);
	// printf("number_of_philosophers: %d\n", tab.number_of_philosophers);
	// printf("time_to_die: %d milliseconds\n", tab.time_to_die);
	// printf("time_to_eat: %d milliseconds\n", tab.time_to_eat);
	// printf("time_to_sleep: %d milliseconds\n", tab.time_to_sleep);
	// printf("number_of_times_each_philosopher_must_eat: %d\n", tab.number_of_times_each_philosopher_must_eat);
	// ------------------------------------------------------------------------

	// CREATE semaphore
	sem_unlink("fork_availability");
	tab.fork_availability = sem_open("fork_availability", O_CREAT, 0644, tab.number_of_philosophers / 2);
	tab.n_times_eaten = malloc(sizeof(int) * tab.number_of_philosophers);
	int i = -1;
	while (++i < tab.number_of_philosophers)
		tab.n_times_eaten[i] = 0;
	// GET start time
	if ((tab.start_time = get_current_time(&tab)) == -1)
		return (0);
	// MALLOC space for t_philosopher struct variables
	tab.time_last_meal = malloc(sizeof(long long) * tab.number_of_philosophers);
	tab.phi_pid = malloc(sizeof(int) * tab.number_of_philosophers);
	
	// CREATE child processes / start philosophers
	i = -1;
	while (++i < tab.number_of_philosophers)
	{
		tab.phi_n = i;
		pid_t fork_ret = fork();
		if (fork_ret == 0)
		{
			phi_f(&tab);
		}
		else if (fork_ret > 0)
		{
			// TESTING --------------------------------------------------------
			// printf("pid of child %d is %d\n", i, (int)fork_ret);
			// ----------------------------------------------------------------
			tab.phi_pid[i] = fork_ret;
		}
		usleep(5000);
	}

	// WAIT for a process/philosopher to die or all to fatten up
	int philosophers_left = tab.number_of_philosophers;
	int exit_status;
	pid_t pid;
	while (philosophers_left--)
	{
		pid = wait(&exit_status);
		// printf("philosopher with id %d exited with status %d\n", (int)pid, WEXITSTATUS(exit_status));
		// while
		if (WEXITSTATUS(exit_status) == EXIT_DEATH)
		{
			while (--tab.number_of_philosophers > -1)
			{
				// printf("kill'ing philosopher with pid %d\n", tab.phi_pid[tab.number_of_philosophers]);
				kill(tab.phi_pid[tab.number_of_philosophers], SIGKILL);

			}
			// printf("all philosophers kill'ed\n");
			break;
		}
		// As soon as wait() returns with a pid, we know a philospopher has exited, either because he's fat or because he's dead. If he's fat, we let the other philosopher's continue; if he's dead, we kill the other philosophers. In order to kill the other philosophers we have to 
	}
	// printf("all philosophers are done\n");

	// sem_unlink("fork_availability");

	return (1);
}
