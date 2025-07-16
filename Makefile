# Define the C compiler
CC = gcc -std=c17

# Default compiler flags (with sanitizers for development)
# -std=c17: Use C17 standard
# -Wshadow: Warn about shadowed variables
# -Wall: Enable all common warnings
# -Wno-unused-result: Disable warning for unused function results
# -g: Include debugging information
# -D_GLIBC_DEBUG: Define _GLIBC_DEBUG for glibc debugging features
# -fsanitize=address: Enable AddressSanitizer for memory error detection
# -fsanitize=undefined: Enable UndefinedBehaviorSanitizer for undefined behavior detection
# -fno-omit-frame-pointer: Required for AddressSanitizer stack traces

CFLAGS_DEFAULT = -Wshadow -Wall -Wno-unused-result -pedantic -fanalyzer

CFLAGS_DEBUG = $(CFLAGS_DEFAULT) -g -D_GLIBC_DEBUG -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer

CFLAGS_FAST = $(CFLAGS_DEFAULT) -O2

# Define the executable name
TARGET = main.out
FAST_TARGET = fast_$(TARGET)

# Define all source files in a variable
SRCS = \
	main.c \
	server/socket.c \
	server/connection.c \
	threadpool/threadpool.c \
	threadpool/worker.c \
	queue/queue_task_t.c

# Default target: build the executable with sanitizers
all: $(TARGET)

# Target for a fast build (with O2 optimization)
fast: $(FAST_TARGET)

# Rule to build the executable with default flags (sanitizers)
# Uses the SRCS variable for dependencies
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS_DEBUG) -o $(TARGET) $^

# Rule to build the executable with fast flags (O2 optimization)
# Also uses the SRCS variable for dependencies
$(FAST_TARGET): $(SRCS)
	$(CC) $(CFLAGS_FAST) -o $(FAST_TARGET) $^

# Clean rule: remove the executables
clean:
	rm -f $(TARGET) $(FAST_TARGET)

.PHONY: all fast clean
