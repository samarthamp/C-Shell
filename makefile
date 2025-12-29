# Compiler and Flags
CC = gcc
CFLAGS = -Wall -g

# Target Executable
TARGET = myshell

# Object Files
OBJS = main.o prompt-1.o

# Build Rule
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile main.c
main.o: main.c header.h prompt-1.h
	$(CC) $(CFLAGS) -c main.c

# Compile prompt.c
prompt.o: prompt-1.c header.h prompt-1.h
	$(CC) $(CFLAGS) -c prompt-1.c

# Clean Rule
clean:
	rm -f *.o $(TARGET)