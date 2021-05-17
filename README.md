# Philosophers
## ***WORK IN PROGRESS***

## Project Description

***"Summary:  In this project, you will learn the basics of threading a process and how to work on the same memory space. You will learn how to make threads. You will discover the mutex, semaphore and shared memory."***

What it comes down to (in my implementation anyway) is a program with the main objective of running a loop that constitutes the philosophers' routines: think, eat, sleep, repeat. The loop ends when all the philosophers have eaten a certain number of times or if one starves and dies.

Each philosopher has their own separate routine, either in a thread or a child process, but they are related to each other by the limitation on how many philosophers can eat at the same time. Supposedly they're all sitting around a round table with food on it, but only a certain number of forks and the need to possess two forks to be able to eat.

This concept is implemented in three different ways:
1. the philosophers are threads and each fork is protected by a mutex lock
2. the philosophers are threads and the total sum of forks is protected with one semaphore
3. the philosophers are processes and the total sum of forks is protected with one semaphore

## How to Use
1. cd into philo_one, philo_two or philo_three.
2. run `make` to compile
3. execute the philo_one, philo_two or philo_three executable with the following arguments given as numbers:
	- number of philosophers
	- how much time in milliseconds can elapse since the start of a philosopher's last meal before the philosopher dies
	- time in milliseconds it takes to eat
	- time in milliseconds it takes to sleep
	- optional: number of times each philosopher must eat for the program to quit. If this argument is not supplied the program runs until a philosopher dies
	
	e.g.: `./philo_one 5 800 200 200 3`

## Additional Notes

### Reporting on a philosophers death when it's waiting on a semaphore or a mutex lock
When a philosopher thread is waiting for a semaphore or mutex lock to become available, it can't do anything else in the meantime. Thus it also can't check if the philosopher has died and report on its death in a timely manner. This task is outsourced to a separate thread: grimreaper. This thread continually checks if the philosopher is still alive and reports on their death when required.

### Preventing messages from being printed post death etc.
Every message printed to stdout requires the possession of the same mutex lock: the print_lock. 
And before printing anything, exit_code is checked: if it's not 0, the message isn't printed. 
When the first philosopher dies or the last philosopher has eaten enough times, this is printed. While in possession of the lock, the exit_code is also set to a non-0 value. So any successive thread that accesses the print_lock to print something doesn't pass the exit_code check and therefore doesn't get to print anything. Failing the exit_code check also causes the thread to exit.
This makes it so that the message that reports on the death of a philosopher or the attaining of fatness of the last philosopher is always the last message that is printed before all the threads and the main process exit.

### Preventing philosophers from eating when they should be dying
During any of the bellow mentioned stages a philosopher's time_last_meal could expire (meaning it should die), potentially also putting grimreaper into motion.

- philosopher waits for forks (mutex locks)
- philosopher gets forks and can start eating
- philosopher waits for print_lock to print message that it is eating

But if it's liveliness isn't checked (accurately), it could still manage to print that it's eating. And if grimreaper was triggered, the message of it eating would be quickly followed up by a message of its death.
To prevent zombie philosophers and the mix up of messages, the liveliness of a philosopher is the final check before printing that it is eating. And only if it passes this check is time_last_meal updated.
This makes it so that the philosopher dies even if it has technically acquired a pair of forks and is about to dig in.

### Time Lag
With the arguments `3 30 10 10` you could think that nobody has to die, because if each philosopher takes 10ms to eat, all three could eat within 30ms. But actually the first philosopher to eat will be the first to die, at 30ms. This is because:
1. Even if he would be able to eat at that point, the `time_to_die` has also elapsed, and in this harsh and cruel simulation environment we check if he's dead before we check if he's able to eat.
2. Because running the simulation also takes time. For example: a little time will elapse between checking whether a philosopher can eat and actually starting the meal, and between deciding that a philosopher is done eating and getting him into bed. 
So even if, theoretically, all philosophers have enough time to eat, the code still needs to be run and a slow pc or miniscule hiccup can easily spoil the fun.
On my old-ass MacBook it seems that an extra ±5ms per philosopher is needed as a buffer on `time_to_die` to get from 'theoretically survivable' to 'practically survivable'.

## Resources
- [video: How to create and join threads in C (pthreads).](https://www.youtube.com/watch?v=uA8X5zNOGw8&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM)
- [video: How to pass arguments to and get results from threads. (pthread_create, pthread_join)](https://www.youtube.com/watch?v=It0OFCbbTJE&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM&index=2)
- [video: Safety and Speed Issues with Threads. (pthreads, mutex, locks)](https://www.youtube.com/watch?v=9axu8CUvOKY&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM&index=3)
- [article: How to use POSIX semaphores in C language](https://www.geeksforgeeks.org/use-posix-semaphores-c/#:~:text=To%20lock%20a%20semaphore%20or,int%20sem_post(sem_t%20*sem)%3B)
- [stackoverflow: passing struct to pthread as an argument](https://stackoverflow.com/questions/20196121/passing-struct-to-pthread-as-an-argument)
- [stackoverflow: Share POSIX semaphore among multiple processes](https://stackoverflow.com/questions/32205396/share-posix-semaphore-among-multiple-processes)
- [article: Semaphore between processes example in C](http://www.vishalchovatiya.com/semaphore-between-processes-example-in-c/)
- [stackoverflow: Multiple child process](https://stackoverflow.com/questions/876605/multiple-child-process)
