CC = gcc
CFLAGS = -std=c99 -Wall -Werror
DEBUG_FLAGS = -g
LIBS = -ledit -lm
TARGET = zlisp
SRCS = main.c lib/mpc.c lib/types.c lib/builtin.c lib/parser.c
OBJS = $(SRCS:.c=.o)

EMCC = emcc
EMCC_FLAGS = --preload-file std.zsp --preload-file hello.zsp -std=c99 -Wall -Werror \
			-sEXPORTED_FUNCTIONS=_main,_execute,_cleanup -sEXPORTED_RUNTIME_METHODS=cwrap -sMODULARIZE -sEXPORT_ES6=1
EMCC_LIBS = -lm
EMCC_TARGET = zlisp/main.js

.PHONY: all debug clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

wasm:
	mkdir -p static/zlisp
	$(EMCC) $(EMCC_FLAGS) -o static/$(EMCC_TARGET) $(SRCS) $(EMCC_LIBS)

clean:
	rm -rf $(OBJS) $(TARGET) static
