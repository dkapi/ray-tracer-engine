#include "engine.h"


int main() {


    // camera
    camera_t camera = {0};

    camera.aspect_ratio = (16.0 / 9.0);
    camera.image_width = 2556;
    camera.focal_length = 1; // prolly make 1 default
    camera.samples_per_pixel = 500; // samples 10 is pretty good as well and quickeer, dont go higher than 100 i think
    camera_initialize(&camera);


    // Open file to write PPM image & write header
    FILE *img = fopen("output.ppm", "wb");


    // World (hittable list)
    hittable_list *world = hittable_list_create();

    // Add first sphere
    point3 center1 = vec3_create_values(1.5, 0.5, -2);
    sphere *s1 = sphere_create(&center1, 0.5);
    hittable_list_add(world, (hittable *)s1);

    // Add additional spheres around the scene
    point3 center2 = vec3_create_values(-2.0, 0.5, -4.0);
    sphere *s2 = sphere_create(&center2, 0.7);
    hittable_list_add(world, (hittable *)s2);

    point3 center3 = vec3_create_values(0.0, -0.5, -3.5);
    sphere *s3 = sphere_create(&center3, 1.0);
    hittable_list_add(world, (hittable *)s3);

    point3 center4 = vec3_create_values(3.0, -1.0, -6.0);
    sphere *s4 = sphere_create(&center4, 1.5);
    hittable_list_add(world, (hittable *)s4);

    // Add first triangle
    point3 tri_a1 = vec3_create_values(-1.0, 0.0, -5.0);
    point3 tri_b1 = vec3_create_values(1.0, 0.0, -5.0);
    point3 tri_c1 = vec3_create_values(0.0, 1.0, -5.0);
    triangle_t *t1 = create_triangle(&tri_a1, &tri_b1, &tri_c1);
    hittable_list_add(world, (hittable *)t1);

    // Add additional triangles around the scene
    point3 tri_a2 = vec3_create_values(2.0, -1.0, -6.0);
    point3 tri_b2 = vec3_create_values(3.5, -1.0, -6.0);
    point3 tri_c2 = vec3_create_values(2.75, 1.0, -6.0);
    triangle_t *t2 = create_triangle(&tri_a2, &tri_b2, &tri_c2);
    hittable_list_add(world, (hittable *)t2);

    point3 tri_a3 = vec3_create_values(-3.0, -1.5, -7.0);
    point3 tri_b3 = vec3_create_values(-1.0, -1.5, -7.0);
    point3 tri_c3 = vec3_create_values(-2.0, 0.5, -7.0);
    triangle_t *t3 = create_triangle(&tri_a3, &tri_b3, &tri_c3);
    hittable_list_add(world, (hittable *)t3);

    // render loop
    render(&camera, world, img);


    // Cleanup
    fclose(img);
    hittable_list_destroy(world);
    free(t1);
    // free(s1);
    // free(s2);

    return 0;
}
