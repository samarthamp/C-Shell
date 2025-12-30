# Compiler and Flags
CC = gcc
CFLAGS = -Wall -g

# Target Executable
TARGET = myshell

# Object Files (Added command-2.o)
OBJS = main.o prompt-1.o command-2.o

# Build Rule
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Dependencies
main.o: main.c header.h prompt-1.h command-2.h
	$(CC) $(CFLAGS) -c main.c

prompt-1.o: prompt-1.c header.h prompt-1.h
	$(CC) $(CFLAGS) -c prompt-1.c

command-2.o: command-2.c header.h command-2.h
	$(CC) $(CFLAGS) -c command-2.c

clean:
	rm -f *.o $(TARGET)