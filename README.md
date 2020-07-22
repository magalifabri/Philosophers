# Philosophers - WORK IN PROGRESS

## Resources
- [video: How to create and join threads in C (pthreads).](https://www.youtube.com/watch?v=uA8X5zNOGw8&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM)
- [video: How to pass arguments to and get results from threads. (pthread_create, pthread_join)](https://www.youtube.com/watch?v=It0OFCbbTJE&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM&index=2)
- [video: Safety and Speed Issues with Threads. (pthreads, mutex, locks)](https://www.youtube.com/watch?v=9axu8CUvOKY&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM&index=3)
- [How to use POSIX semaphores in C language](https://www.geeksforgeeks.org/use-posix-semaphores-c/#:~:text=To%20lock%20a%20semaphore%20or,int%20sem_post(sem_t%20*sem)%3B)
- 

- [stackoverflow: passing struct to pthread as an argument](https://stackoverflow.com/questions/20196121/passing-struct-to-pthread-as-an-argument)
- [Share POSIX semaphore among multiple processes](https://stackoverflow.com/questions/32205396/share-posix-semaphore-among-multiple-processes)
- [Semaphore between processes example in C](http://www.vishalchovatiya.com/semaphore-between-processes-example-in-c/)
- [Multiple child process](https://stackoverflow.com/questions/876605/multiple-child-process)

## Tests

- 3 15000 5000 8000: 3rd should die after ±15s
- 3 15000 3000 8000 3: they're all done after ±39s

## study notes
- **Semaphores**: A semaphore is a pointer to memory that can be either accessed either only from within the same process or across different processes. 
`sem_t *sem_open(const char *name, int oflag, mode_t mode, unsigned int value);`
`sem_t *`: returns a "semaphore descriptor", a bit like a file descriptor, which is a pointer of type sem_t that points to a location in memory.
`const char *name`: a name chosen for the semaphore, which is used by, e.g., sem_unlink() to refer to the semaphore to manipulate it
`oflag`: when creating a semaphore with sem_open() you'll put O_CREAT here
`mode_t mode`: *"specifies the permissions for the semaphore as described in chmod(2)"*, e.g. 644
`unsigned int value`: the number of threads or processes that can use the semaphore at the same time