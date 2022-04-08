CC = gcc
CFLAGS  = -g -Wall

test1_OBJS:= test1.o

all: test1

test1:  $(test1_OBJS)
	$(CC) $(CFLAGS) -o$@ $^ 

clean: 
	$(RM) count *.o *~ test1

.PHONY: all clean
