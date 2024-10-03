CC = gcc
CFLAGS = -std=c99 -Wall -Werror
DEBUG_FLAGS = -g
LIBS = -ledit -lm
TARGET = zlisp
SRCS = main.c lib/mpc.c lib/types.c lib/builtin.c lib/parser.c
OBJS = $(SRCS:.c=.o)

.PHONY: all debug clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
