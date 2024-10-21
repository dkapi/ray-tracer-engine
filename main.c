#include "engine.h"

int main() {

    // camera
    camera_t camera = {0};

    camera.aspect_ratio     = (16.0 / 9.0);
    camera.image_width      = 2560;
    camera.samples_per_pixel= 500; // samples 10 is pretty good as well and quickeer, dont go higher than 100 i think
    camera.max_depth        = 50;
    camera.defocus_angle    = 0.6;
    camera.focus_dist       = 10.0;
    camera.vfov             = 20;
    camera.lookfrom         = vec3_create_values(13,2,3);
    camera.lookat           = vec3_create_values(0,0,0);
    camera.vup              = vec3_create_values(0,1,0);
    camera_initialize(&camera);


    // Open file to write PPM image & write header
    FILE *img = fopen("output.ppm", "wb");

// Create hittable list (world)
    hittable_list *world = hittable_list_create();

    // Ground material and sphere
    color ground_color = vec3_create_values(0.5, 0.5, 0.5);
    material_t *ground_material = (material_t *)create_lambertian(&ground_color);
    point3 ground_center = vec3_create_values(0, -1000, 0);
    sphere *ground_sphere = sphere_create(&ground_center, 1000.0, ground_material);
    hittable_list_add(world, (hittable *)ground_sphere);

    // Generate small spheres randomly in the scene
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            double choose_mat = random_double();
            point3 center = vec3_create_values(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            vec3 create = vec3_create_values(4, 0.2, 0);
            vec3 temp_sub = vec3_subtract(&center, &create);
            if (vec3_length(&temp_sub) > 0.9) {
                material_t *sphere_material;

                if (choose_mat < 0.8) {
                    // Diffuse material
                    vec3 random1 = vec3_random();
                    vec3 random2 = vec3_random();
                    color albedo = vec3_multiply(&random1, &random2);
                    sphere_material = (material_t *)create_lambertian(&albedo);
                } else if (choose_mat < 0.95) {
                    // Metal material
                    color albedo = vec3_random_values(0.5, 1.0);
                    double fuzz = random_double_with_params(0, 0.5);
                    sphere_material = (material_t *)create_metal(&albedo, fuzz);
                } else {
                    // Glass material
                    sphere_material = (material_t *)create_dielectric(1.5);
                }

                sphere *small_sphere = sphere_create(&center, 0.2, sphere_material);
                hittable_list_add(world, (hittable *)small_sphere);
            }
        }
    }

    // Add three large spheres with different materials
    material_t *material1 = (material_t *)create_dielectric(1.5);
    point3 center1 = vec3_create_values(0, 1, 0);
    sphere *sphere1 = sphere_create(&center1, 1.0, material1);
    hittable_list_add(world, (hittable *)sphere1);

    color color2 = vec3_create_values(0.4, 0.2, 0.1);
    material_t *material2 = (material_t *)create_lambertian(&color2);
    point3 center2 = vec3_create_values(-4, 1, 0);
    sphere *sphere2 = sphere_create(&center2, 1.0, material2);
    hittable_list_add(world, (hittable *)sphere2);

    color color3 = vec3_create_values(0.7, 0.6, 0.5);
    material_t *material3 = (material_t *)create_metal(&color3, 0.0);
    point3 center3 = vec3_create_values(4, 1, 0);
    sphere *sphere3 = sphere_create(&center3, 1.0, material3);
    hittable_list_add(world, (hittable *)sphere3);

    // Render loop
    render(&camera, world, img);

    // Cleanup
    fclose(img);
    hittable_list_destroy(world);

    return 0;
}
