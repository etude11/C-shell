# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# List of all .c files in the current directory
SRCS = $(wildcard *.c)

# Output executable
OUTPUT = a

# Rule to compile and link all .c files into the executable
all: $(OUTPUT)

$(OUTPUT): $(SRCS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRCS)

# Clean rule to remove the compiled executable
clean:
	rm -f $(OUTPUT)
