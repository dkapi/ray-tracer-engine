# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -ggdb -fopenmp -o2 -Iinclude

# Extra flags
EXTRA_FLAGS = -fopenmp -lm 

# Source files
SRC = main.c \
	  vec3.c \
	  ray.c \
	  hittable.c \
	  sphere.c \
	  triangle.c \
	  quad.c \
	  volumes.c \
	  object_instancing.c \
  	  darray/d_array.c \
	  interval.c \
	  camera.c \
	  color.c \
	  material.c \
	  aabb.c \
	  bvh.c \
	  raster.c \
	  textures/texture.c \
	  textures/perlin.c \
	  triangle_mesh.c \
	  cube_map.c \
	  hdr.c 

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
