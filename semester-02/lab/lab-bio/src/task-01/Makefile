TARGET = main
CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread
SRCS = main.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
clean:
	rm -f $(OBJS) $(TARGET)

re: clean all
