#include "engine.h"

int main() {
    // camera
    camera_t camera = {0};

    camera.aspect_ratio     = (16.0 / 9.0);
    camera.image_width      = 2556;
    camera.samples_per_pixel= 500;
    camera.max_depth        = 50;
    camera.vfov             = 15;
    camera.lookfrom         = vec3_create_values(2, 1.5, 5); 
    camera.lookat           = vec3_create_values(0, .9, 0);  
    camera.vup              = vec3_create_values(0, 1, 0);
    camera.defocus_angle    = 0.8;
    camera.focus_dist       = 6.0; // focus dist is from camera origin(lookfrom) to whatever location is camera origin + focus dist
    camera_initialize(&camera);

    // Open file to write PPM image & write header
    FILE *img = fopen("output.ppm", "wb");

    // Create hittable list (world)
    hittable_list *world = hittable_list_create();

    color reflective_color = vec3_create_values(1.0, 1.0, 1.0); // White color for reflective glass
    material_t *material_reflective_glass = (material_t *)create_metal(&reflective_color, 0.0); // Fully reflective

    color copper_color = vec3_create_values(0.72, 0.45, 0.20);
    material_t *material_copper = (material_t *)create_metal(&copper_color, 0.3);

    material_t *material_glass = (material_t *)create_dielectric(1.3);
    color ground_color = vec3_create_values(0.2, 0.2, 0.4);
    material_t *ground_material = (material_t *)create_metal(&ground_color, 0.15);

    point3 center = vec3_create_values(0, .8, 0);
    sphere* s1 = sphere_create(&center, .08, material_copper);
    point3 ground_center = vec3_create_values(0, -1000, 0);
    sphere *ground_sphere = sphere_create(&ground_center, 1000.0, ground_material);
    
    point3 c2 = vec3_create_values(0, .8, 0);
    sphere* s2 = sphere_create(&c2, .25, material_glass);
    hittable_list_add(world, (hittable *)s2);
    hittable_list_add(world, (hittable *)ground_sphere);

    hittable_list_add(world, (hittable*)s1);

    point3 far = vec3_create_values(-1, 1.5, -10);
    sphere* s_far = sphere_create(&far, 2, material_copper);
    hittable_list_add(world, (hittable*)s_far);


    // Triangle behind the sphere
    point3 pa = vec3_create_values(-0.3, 0.5, -1.0);
    point3 pb = vec3_create_values(0.7, 0.5, -1.0);
    point3 pc = vec3_create_values(.2, 1.0, -1.0);

    triangle_t* tri = create_triangle(&pa, &pb, &pc, material_reflective_glass);
    hittable_list_add(world, (hittable*)tri);



    // Render loop
    render(&camera, world, img);

    // Cleanup
    fclose(img);
    hittable_list_destroy(world);

    return 0;
}