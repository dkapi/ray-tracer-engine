#include "engine.h"


int main() {


    // camera
    camera_t camera = {0};

    camera.aspect_ratio = (16.0 / 9.0);
    camera.image_width = 1024;
    camera.focal_length = 1.0; // prolly make 1 default
    camera.samples_per_pixel = 10; // samples 10 is pretty good as well and quickeer, dont go higher than 100 i think
    camera_initialize(&camera);


    // Open file to write PPM image & write header
    FILE *img = fopen("output.ppm", "wb");


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
    render(&camera, world, img);


    // Cleanup
    fclose(img);
    hittable_list_destroy(world);
    free(s1);
    free(s2);

    return 0;
}
