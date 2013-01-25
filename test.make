all:  
	gcc service/threads/condition.c service/threads/thread.c service/threads/mutex.c test.c -g -o test -I ccs/include -I. -L./service/util/.libs -lserviceutil -lpthread
