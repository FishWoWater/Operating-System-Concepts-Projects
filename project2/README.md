## Project2: Unix-shell programming & LKM for task information
### Background knowledge
&emsp;&emsp;In this project we will use some C functions in Linux. First I introduce some of them as belows:
``` C
#include <unistd.h>
#include <signal.h>
/* copy the file descriptor. This function will find a minimum
avaiable file descriptors and sets its pointer as the oldfd(i.e.
the two fd will point to the same file) */
int dup(int oldfd);
/* just a little different from dup(), dup2() can specify the newfd
we want to use. It the newfd is already opened, this function will first close it and reopen */
int dup2(int oldfd, int newfd);

/*
execvp(const char *file, const char *argv[]);
...
/* when creating child processes, if the parent process
  does not wait until its child exits, the child process will possibly become a zombie. In this case we can use the following
  function to ask init process to release the resources of the child process. For more details refer to
  https://www.geeksforgeeks.org/zombie-processes-prevention/ */
signal(SIGCHID, SIG_IGNORE);
```

### Part A: Unix shell programming
> source code is available in project2/simulator.c

#### Step1: Main logic and analysis
&emsp;&emsp;Use a while loop(parent process) to read the input and then create a seperate child process the handle this input
``` C
char *input = malloc(sizeof(char) * MAX_LINE);

while(should_run){
  printf("osh>");
  fflush(stdout);

  fgets(input, MAX_LINE, stdin);
  /* logic to handle the input */
}
```
#### Step2: Create a child process to
