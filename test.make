all:  
	gcc test.c -g -o test -I service/include -I. -L./install/lib -lservice && LD_LIBRARY_PATH=./install/lib:$LD_LIBRARY_PATH ./test
