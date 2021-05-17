# Philosophers
## ***WORK IN PROGRESS***

TO-DO:
- p1, p2: error checking
- have ./philo_* return 1 on wrap_up error
- p1: remove monitor_philosophers?

In the subject pdf it is written that "Each time a philosopher has finished eating, he will drop his forks and start sleeping." So first he drops his forks, and only after does he fall asleep. Therefore mutex_lock() or sem_post() should be called before printing that the philosopher is sleeping.

- problem (2): Invalid free: Because threads can call free on a variable that is shared between the threads, this makes it possible for multiple threads to do so at the same time.
	- solution: Only call free from the main process. return_error() becomes set_error_code() + exit_error()

- problem (2): Status messages are still being printed, even after a philosopher has died.
	- solution: Change starving(), so that the death of a philosopher is processed quicker.

- problem (2): Multiple "died" status messages.
	- solution: Protect the printing of that message with a semaphore (on which sem_post() isn't called).

- problem (2): tab->n_times_eaten is being read/written to after it has been freed.
	- solution: set it to NULL after freeing, and before reading/writing, check if it's NULL.

- problem (2): A philosopher can surpass time_to_die but not drop dead because of sem_wait.
	- solution: Create a thread (grim_reaper) within the threads (philosophers) that checks if a philosopher should die and causing that to happen if so.

TESTING
bad input
(no args)
5 800 200 (too few args)
5 800 200 200 3 3 (too many args)
5 800 200 200 t (non-number args)
5 800 200 200 3 t (non-number args)

regular (on guacamole)
5 800 200 200 (infinite)
5 800 200 200 3 (all fat at ± 1600 - 1610)
4 410 200 200 (infinite)
4 410 200 200 3 (all fat at ± 1200 - 1205)
4 310 200 100 (death at ± 310)
4 310 200 200 (death at ± 310 - interrupts sleeping)
4 200 210 100 (death at ± 200 - interrupts eating)
5 610 200 60 (infinite - test line skipping)

limits (on guacamole)
200 410 200 200 (infinite)
200 200 200 200 (death at ± 200)
200 220 100 100 (infinite)
200 210 100 100 (infinite)
4 60 60 60 (death at ± 60)
200 60 60 60 (death at ± 60)
4 120 60 60 (death at ± 120)
4 125 60 60 (infinite)

philo_three
check if no philo_three processes remain after exiting the program

## Project Description

***"Summary:  In this project, you will learn the basics of threading a process and how to work on the same memory space. You will learn how to make threads. You will discover the mutex, semaphore and shared memory."***

What it comes down to (in my implementation anyway) is a program with the main objective of running a loop that constitutes the philosophers' routines: think, eat, sleep, repeat. The loop ends when all the philosophers have eaten a certain number of times or if one starves and dies.

Each philosopher has their own separate routine, either in a thread or a child process, but they are related to each other by the limitation on how many philosophers can eat at the same time. Supposedly they're all sitting around a round table with food on it, but only a certain number of forks and the need to possess two forks to be able to eat.

This concept is implemented in three different ways, which are described below:

### . . . **philo_one: threads and mutex**

Each fork is protected by a mutex lock. The mutex is locked only while the availability of the fork is checked and changed. While the fork is being used, the mutex isn't locked, so that other threads can check on the availability of the fork and continue on their way if it's not, instead of having to wait right there for it to become available again.

Keeping the mutex locked while the fork is in use should result in much less locking and unlocking and with that, shorter and code. So I'll probably do it like that in the future.

Preventing messages from being printed post death etc.
Every message printed to stdout requires the possession of the same mutex lock: the print_lock. 
And before printing anything, exit_code is checked: if it's not 0, the message isn't printed. 
When the first philosopher dies or the last philosopher has eaten enough times, this is printed. While in possession of the lock, the exit_code is also set to a non-0 value. So any successive thread that accesses the print_lock to print something doesn't pass the exit_code check and therefore doesn't get to print anything. Failing the exit_code check also causes the thread to exit.
This makes it so that the message that reports on the death of a philosopher or the attaining of fatness of the last philosopher is always the last message that is printed before all the threads and the main process exit.

Preventing philosophers from eating when they should be dying
- philosopher waits for forks (mutex locks)
- philosopher gets forks and can start eating
- philosopher waits for print_lock to print message that it is eating
During any of the aforementioned stages a philosopher's time_last_meal could expire (meaning it should die), potentially also putting grimreaper into motion. But if it's liveliness isn't checked (accurately), it could still manage to print that it's eating. And if grimreaper was triggered, the message of it eating would be quickly followed up by a message of its death.
To prevent zombie philosophers and the mix up of messages, the liveliness of a philosopher is the final check before printing that it is eating. And only if it passes this check time_last_meal is updated.
This makes it so that the philosopher dies even if it has technically acquired a pair of forks and is about to dig in.


### . . . **philo_two: threads and semaphore**

We use a semaphore to limit the number of threads or processes that are able to access certain parts of the code.

The part of the code it limits off for us is the part in which philosophers eat.

The number of threads or processes allowed, is set to the number of forks (which is equal to the number of philosophers) divided by two (because a philosopher needs two forks to eat).

### . . . **philo_three: processes and semaphore**

This one works very similarly to philo_two, but instead of the philosophers being represented by threads, they are processes.

## How to Run the Program

1. cd into philo_one, philo_two or philo_three.
2. run `make` to compile
3. execute the philo_one, philo_two or philo_three executable with the following arguments given as numbers:
	- number of philosophers
	- how much time in milliseconds can elapse since the start of a philosopher's last meal before the philosopher dies
	- time in milliseconds it takes to eat
	- time in milliseconds it takes to sleep
	- optional: number of times each philosopher must eat for the program to quit. If this argument is not supplied the program runs until a philosopher dies
	
	e.g.: `./philo_one 3 16000 3000 8000 2`

## Resources
- [video: How to create and join threads in C (pthreads).](https://www.youtube.com/watch?v=uA8X5zNOGw8&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM)
- [video: How to pass arguments to and get results from threads. (pthread_create, pthread_join)](https://www.youtube.com/watch?v=It0OFCbbTJE&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM&index=2)
- [video: Safety and Speed Issues with Threads. (pthreads, mutex, locks)](https://www.youtube.com/watch?v=9axu8CUvOKY&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM&index=3)
- [article: How to use POSIX semaphores in C language](https://www.geeksforgeeks.org/use-posix-semaphores-c/#:~:text=To%20lock%20a%20semaphore%20or,int%20sem_post(sem_t%20*sem)%3B)
- [stackoverflow: passing struct to pthread as an argument](https://stackoverflow.com/questions/20196121/passing-struct-to-pthread-as-an-argument)
- [stackoverflow: Share POSIX semaphore among multiple processes](https://stackoverflow.com/questions/32205396/share-posix-semaphore-among-multiple-processes)
- [article: Semaphore between processes example in C](http://www.vishalchovatiya.com/semaphore-between-processes-example-in-c/)
- [stackoverflow: Multiple child process](https://stackoverflow.com/questions/876605/multiple-child-process)

## Additional Notes

### Time Lag
With the arguments `3 30 10 10` you could think that nobody has to die, because if each philosopher takes 10ms to eat, all three could eat within 30ms. But actually the first philosopher to eat will be the first to die, at 30ms. This is because:
1. Even if he would be able to eat at that point, the `time_to_die` has also elapsed, and in this harsh and cruel simulation environment we check if he's dead before we check if he's able to eat.
2. Because running the simulation also takes time. For example: a little time will elapse between checking whether a philosopher can eat and actually starting the meal, and between deciding that a philosopher is done eating and getting him into bed. 
So even if, theoretically, all philosophers have enough time to eat, the code still needs to be run and a slow pc or miniscule hiccup can easily spoil the fun.
On my old-ass MacBook it seems that an extra ±5ms per philosopher is needed as a buffer on `time_to_die` to get from 'theoretically survivable' to 'practically survivable'.
