.PHONY:clean
CC=g++
CFLAGS=-Wall -g
BIN=fac
OBJS=Condition.o Queue.o Buffer.o ProduceThread.o ConsumeThread.o Factory.o test_main.o
$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
%.o:%.cc
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)
