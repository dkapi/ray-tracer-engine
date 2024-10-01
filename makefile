# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall

# Extra flags
EXTRA_FLAGS = -lm 

# Source files
SRC = main.c vec3.c ray.c

# Object files
OBJ = $(SRC:.c=.o)

# Output executable
TARGET = main

# Default target
all: $(TARGET)

# Linking executable with extra flags
$(TARGET): $(OBJ)
	@echo "Linking $@ with object files: $^"
    #@ prefixed to call will supress output
	@$(CC) $(OBJ) -o $@ $(EXTRA_FLAGS)
	@echo "Executable created: $@"

# Compile source files to object files
%.o: %.c
	@echo "Compiling $< to $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove object files and executable
clean:
	@echo "Cleaning up..."
	@rm -f $(OBJ) $(TARGET) *.ppm
	@echo "Cleanup complete."

# Phony targets
.PHONY: all clean
