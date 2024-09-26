#include <vec3.h>
#include <color.h>

#include <stdint.h>

#define X 256
#define Y 256

#define R 0
#define G 1
#define B 2

int main(int argc, char *argv[])
{
    FILE *img;
    int x, y;


    img = fopen("image.ppm", "w");
    fprintf(img, "P3\n%d\n%d\n255\n", X, Y);

    for (y = 0; y < Y; y++) {
        for(x = 0; x < X; x++) {
            vec3 pixel_colors = vec3_create_values((double) x / X, (double) y / Y, 0);
            write_color(img, pixel_colors);
        }
    }
    
    return 0;

}