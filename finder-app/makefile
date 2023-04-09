CC=$(CROSS_COMPILE)gcc

CFLAGS=-g -Wall -ansi
exe=writer
obj=writer.o

default: $(exe)

$(exe): $(obj)
	$(CC) -o $(exe) $(obj)

writer.o: writer.c
	$(CC) $(CFLAGS) -c writer.c

clean:
	rm -f $(exe) $(obj)
