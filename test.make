all:  
	gcc test.c -g -o test -I ccs/include -I. -L./install/lib -lservice && LD_LIBRARY_PATH=./install/lib:$LD_LIBRARY_PATH ./test
