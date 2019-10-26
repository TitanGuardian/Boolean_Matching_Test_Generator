CC=g++
CFLAGS=-c -Wall

all: balance.o Test.o rand64.o
	$(CC) ./src/main.cpp rand64.o Test.o balance.o ../abc/libabc.a -lboost_program_options -lpthread -lm -ldl -lreadline -o gentest -std=c++11	
Test.o:
	$(CC) $(CFLAGS) ./src/Test.cpp -std=c++11
balance.o:	
	gcc ./src/abcbalance.c -Wall -g -c -o balance.o 
rand64.o:
	gcc ./src/rand64.c -Wall -g -c -o rand64.o
clean:
	rm -rf *.o gentest
	rm -rf ./out/*
