CC = gcc
CFLAGS  = -g -Wall

test1_OBJS:= test1.o
test2_OBJS:= test2.o
test3_OBJS:= test3.o

all: test1 test2 test3

test1:  $(test1_OBJS)
	$(CC) $(CFLAGS) -o$@ $^ 

test2:  $(test2_OBJS)
	$(CC) $(CFLAGS) -o$@ $^ 

test3:  $(test3_OBJS)
	$(CC) $(CFLAGS) -o$@ $^ 

clean: 
	$(RM) count *.o *~

.PHONY: all clean
