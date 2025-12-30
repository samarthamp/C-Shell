CC = gcc
CFLAGS = -Wall -g

# Added reveal.o
OBJS = main.o prompt-1.o command-2.o hop-3.o reveal-4.o

TARGET = myshell

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c header.h prompt-1.h command-2.h
	$(CC) $(CFLAGS) -c main.c

prompt-1.o: prompt-1.c header.h prompt-1.h
	$(CC) $(CFLAGS) -c prompt-1.c

command-2.o: command-2.c header.h command-2.h hop-3.h reveal-4.h
	$(CC) $(CFLAGS) -c command-2.c

hop-3.o: hop-3.c header.h hop-3.h
	$(CC) $(CFLAGS) -c hop-3.c

reveal-4.o: reveal-4.c header.h reveal-4.h
	$(CC) $(CFLAGS) -c reveal-4.c

clean:
	rm -f *.o $(TARGET)