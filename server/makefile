CC=$(CROSS_COMPILE)gcc

CFLAGS+=-g -Wall -Werror 
exe=aesdsocket
obj=aesdsocket.o

all: $(exe)

$(exe): $(obj)
	$(CC) -o $(exe) $(obj) $(lib)

aesdsocket.o: aesdsocket.c
	$(CC) $(CFLAGS) -c aesdsocket.c $(lib)

clean:
	rm -f $(exe) $(obj)
