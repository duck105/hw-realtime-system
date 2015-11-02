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


使用函數庫

```
#define _GNU_SOURCE	// for type `cpu_set_t` before #include <sched.h>
#include <pthread.h>	// POSTFIX pthread_*
#include <unistd.h>	// UNIX standard library, ex. gettid()i
#include <sched.h>	// POSTIFX sched_*
#include <errno.h>	// printf("%s", strerror(errno)); about error msg
```

在 pthread create 後，可以特別設定說要在哪一個 CPU 上面操作。如果主機是 multicore，會造成實驗不方便，方便起見可以固定在 CPU 0 上面測試。

```
cpu_set_t mask;
CPU_ZERO(&mask);
CPU_SET(0, &mask);
sched_setaffinity(0, sizeof(mask), &mask);
```

在 pthread create 後，可以設定他的兄弟姊妹們都使用哪一種 scheduler 上面跑。假設都在 `SCHED_FIFO` 上面跑，執行這一行需要 root 權限，其中一個原因是在眾多的 scheduler 中，`SCHED_FIFO` 上的 pthread 會先跑，假如現在使用 `SCHED_BATCH` 則不用使用 root 權限即可執行，檢查回傳的結果，確定是否正確執行，即使設定錯誤，程序仍按照預設的 scheduler 繼續跑。

特別注意到，呼叫 `sched_setscheduler()` 中的 param 使用時，預設 `param.sched_priority = 0`，但是 `SCHED_FIFO` 提供的 priority 介於 1 到 99 之間，如果設定為 0 則會失敗，並且在 `strerror(errno)` 會顯示參數錯誤。

```
struct sched_param param;
printf("Thread %d sched_setscheduler()\n", mid);
param.sched_priority = sched_get_priority_max(SCHED_FIFO);
s = sched_setscheduler(0, SCHED_FIFO, &param);
printf("Thread %d sched after %s\n", mid, strerror(errno));
```

由於一次 create 數個 pthread 出來，他們被創建後才能進行 `sched_setscheduler()`，所以他們之間會發生突然間的交錯，因為有些在 `sched_setscheduler()` 之前可能被 context switch 到另一個 pthread X 上，導致另一個 pthread X 比較晚被 create 並順利地跑完 setcheduler 跑到要印出的片段，隨後才被其他的 pthread context switch 過去。


