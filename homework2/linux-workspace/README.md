## README ##

Real-time System Homework modify ***Linux-2.6.30.6*** version, focus on

```plain
.
├── README.md
├── arch
│   └── x86
│       ├── include
│       │   └── asm
│       │       └── unistd_32.h
│       └── kernel
│           └── syscall_table_32.S
├── include
│   └── linux
│       ├── sched.h
│       └── syscalls.h
└── kernel
    ├── sched.c
    ├── sched_fair.c
    ├── sched_rt.c
    └── sched_weighted_rr.c
```

在這些 file 中搜尋 `RTS Proj2: weighted_rr`` 相關的關鍵字即可找到需要實作部分。

### 實作 weight_rr ###

in `include/linux/sched.h`

```c
struct sched_rt_entity {
	...
	//+ RTS Proj2: weighted_rr
	struct list_head weighted_rr_list_item;
	...
};
...
struct task_struct {
	...
	//+ RTS Proj2: weighted_rr
	unsigned int task_time_slice;
	//+ RTS Proj2: weighted_rr_prio
	unsigned int weighted_time_slice;
	...
};
...
//+ RTS Proj2: weighted_rr
extern int weighted_rr_time_slice;
```

in `kernel/sched.c`

```c
//+ RTS Proj2: weighted_rr
static inline int weighted_rr_policy(int policy)
{
	if(unlikely(policy == SCHED_WEIGHTED_RR))
		return 1;
	return 0;
}

//+ RTS Proj2: weighted_rr
static inline int task_has_weighted_rr_policy(struct task_struct *p)
{
	return weighted_rr_policy(p->policy);
}
...
//+ RTS Proj2: weighted_rr
struct weighted_rr_rq {
	struct list_head queue;
	unsigned long nr_running;
	struct list_head *weighted_rr_load_balance_head, *weighted_rr_load_balance_curr;
};
...
struct rq {
	...
	//+ RTS Proj2: weighted_rr
	struct weighted_rr_rq weighted_rr;
	...
};
...
```

### Material ###

* [Linux Kernel: 強大又好用的 list_head 結構](http://blog.xuite.net/uhonda0618/linuxkernelcode/20863635-Linux+Kernel%3A+%E5%BC%B7%E5%A4%A7%E5%8F%88%E5%A5%BD%E7%94%A8%E7%9A%84list_head%E7%B5%90%E6%A7%8B)

### Sample Output ###

#### Weight RR ####

```
Welcome to Buildroot
buildroot login: root
# ls
run.sh            test_sjf          test_weighted_rr
# ./run.sh
sched_policy: 6, quantum: 1, num_threads: 5, buffer_size: 5000000
a quantum = 1
b quantum = 2
c quantum = 4
d quantum = 8
e quantum = 16
abababcacabcdabcdeabcdeabcdeabcdabcdabcabcabcabcabcabcabcababababababababa
```

#### SJF ####

```
# ./run.sh
sched_policy: 6, quantum: 10, num_threads: 5, buffer_size: 5000000
a exeTime = 29
b exeTime = 26
c exeTime = 27
d exeTime = 23
e exeTime = 21
edbca
```