# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -ggdb

# Extra flags
EXTRA_FLAGS = -lm 

# Source files
SRC = main.c vec3.c ray.c hittable.c sphere.c triangle.c darray/d_array.c interval.c camera.c color.c material.c aabb.c bvh.c

# Object files
OBJ = $(SRC:.c=.o)

# Output executable
TARGET = engine

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
