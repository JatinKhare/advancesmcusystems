CC = gcc
CFLAGS  = -g -Wall

test1_OBJS:= test1.o
test2_OBJS:= test2.o

all: test1 test2

test1:  $(test1_OBJS)
	$(CC) $(CFLAGS) -o$@ $^ 

test2:  $(test2_OBJS)
	$(CC) $(CFLAGS) -o$@ $^ 

clean: 
	$(RM) count *.o *~ test1 test23

.PHONY: all clean
