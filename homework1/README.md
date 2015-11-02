## README ##

### Ubuntu vim env ###

path : `etc/vim/vimrc`

```
set tabstop=4
set nu
```

### Program ###

#### Compiler & Run ####

```
$ gcc sched_test.cpp -lpthread
$ sudo ./a.out
```

if you call `sched.h`, need root privilege to execute it.

#### Pthread ####

```
#define _GNU_SOURCE	// for type `cpu_set_t` before #include <sched.h>
#include <pthread.h>	// POSTFIX pthread_*
#include <unistd.h>	// UNIX standard library, ex. gettid()i
#include <sched.h>	// POSTIFX sched_*
#include <errno.h>	// printf("%s", strerror(errno)); about error msg
```
