CC = gcc
CFLAGS = -O2 -Wall -Wno-scalar-storage-order
SRCDIR = src
BINDIR = bin

# List of source files
SRCS = $(wildcard $(SRCDIR)/*.c)

# List of object files
OBJS = $(SRCS:$(SRCDIR)/%.c=$(BINDIR)/%.o)

# Name of the executable
TARGET = build/ilda-analyzer

# Default target
all: $(TARGET)

# Rule for compiling object files
$(BINDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for linking the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Clean target
clean:
	rm -f $(OBJS) $(TARGET)

# PHONY targets
.PHONY: all clean
