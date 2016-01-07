CFLAGS=-O2

test:
	gcc $(CFLAGS) test_sjf.c -lpthread -o test_sjf

install:
	cp /usr/src/linux-2.6.32.60/kernel/sched_weighted_rr.c ./sched_weighted_rr-backup.c

clean:
	rm test_weighted_rr

