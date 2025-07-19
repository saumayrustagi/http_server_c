# --- COMPILER ---
CC = clang -std=c17

CFLAGS_DEFAULT = -Wshadow -Wall -Wno-unused-result -pedantic

CFLAGS_DEBUG = $(CFLAGS_DEFAULT) -g -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer
CFLAGS_FAST = $(CFLAGS_DEFAULT) -O2

# --- FILES ---
TARGET = main.out
FAST_TARGET = fast_$(TARGET)

# Define all source files in a variable
SRCS = \
	main.c \
	server/socket.c \
	server/connection.c \
	threadpool/threadpool.c \
	threadpool/worker.c \
	queue/queue_task_t.c \
	arena/arena.c

DEBUG_OBJS_DIR = target/debug/
FAST_OBJS_DIR = target/release/

DEBUG_OBJS = $(patsubst %.c,$(DEBUG_OBJS_DIR)%.o,$(SRCS))
FAST_OBJS = $(patsubst %.c,$(FAST_OBJS_DIR)%.o,$(SRCS))

# --- ARGS ---
$(DEBUG_OBJS): CFLAGS = $(CFLAGS_DEBUG)
$(FAST_OBJS): CFLAGS = $(CFLAGS_FAST)

$(TARGET): CFLAGS = $(CFLAGS_DEBUG)
$(FAST_TARGET): CFLAGS = $(CFLAGS_FAST)

# --- COMMANDS ---
$(DEBUG_OBJS_DIR)%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(FAST_OBJS_DIR)%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(DEBUG_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(FAST_TARGET): $(FAST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: all fast clean

all: $(TARGET)

fast: $(FAST_TARGET)

clean:
	rm -rf $(TARGET) $(FAST_TARGET) $(DEBUG_OBJS_DIR) $(FAST_OBJS_DIR)
