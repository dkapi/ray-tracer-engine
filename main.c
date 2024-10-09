#include "engine.h"


int main() {

    // Open file to write PPM image & write header
    FILE *img = fopen("output.ppm", "w");
    fprintf(img, "P3\n%d %d\n255\n", (int)I_WIDTH, (int)I_HEIGHT);

    // camera
    camera_t* camera = camera_initialize();


    // figure out how to call this in a const param? maybe u cant? not sure
    point3 center1 = vec3_create_values(0, 0, -1);
    point3 center2 = vec3_create_values(0, -100.5, -1);
    sphere *s1 = sphere_create(&center1, 0.5);
    sphere *s2 = sphere_create(&center2, 100);

    // world
    hittable_list *world = hittable_list_create();    
    hittable_list_add(world, (hittable *)s1);
    hittable_list_add(world, (hittable *)s2);

    // render loop
    render(camera, world, img);


    // Cleanup
    fclose(img);
    hittable_list_destroy(world);
    free(s1);
    free(s2);

    return 0;
}
