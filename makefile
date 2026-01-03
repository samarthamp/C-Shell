CC = gcc
CFLAGS = -Wall -g

# Added myshrc-9.o
OBJS = main.o prompt-1.o command-2.o hop-3.o reveal-4.o log-5.o proclore-7.o seek-8.o myshrc-9.o

TARGET = myshell

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c header.h prompt-1.h command-2.h log-5.h myshrc-9.h
	$(CC) $(CFLAGS) -c main.c

prompt-1.o: prompt-1.c header.h prompt-1.h
	$(CC) $(CFLAGS) -c prompt-1.c

command-2.o: command-2.c header.h command-2.h hop-3.h reveal-4.h log-5.h proclore-7.h seek-8.h myshrc-9.h
	$(CC) $(CFLAGS) -c command-2.c

hop-3.o: hop-3.c header.h hop-3.h
	$(CC) $(CFLAGS) -c hop-3.c

reveal-4.o: reveal-4.c header.h reveal-4.h
	$(CC) $(CFLAGS) -c reveal-4.c

log-5.o: log-5.c header.h log-5.h command-2.h
	$(CC) $(CFLAGS) -c log-5.c

proclore-7.o: proclore-7.c header.h proclore-7.h
	$(CC) $(CFLAGS) -c proclore-7.c

seek-8.o: seek-8.c header.h seek-8.h
	$(CC) $(CFLAGS) -c seek-8.c

myshrc-9.o: myshrc-9.c header.h myshrc-9.h command-2.h
	$(CC) $(CFLAGS) -c myshrc-9.c

clean:
	rm -f *.o $(TARGET)