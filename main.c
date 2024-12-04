#include <time.h>
#include "engine.h"

int main(int argc, char* argv[]) {
    srand(time(NULL));

    // manually set the number of threads, else let OpenMP decide
    if (argc > 1) {
        int num_threads = atoi(argv[1]);
        if (num_threads > 0) {
            omp_set_num_threads(num_threads);
        }
    }

    // Camera setup
    camera_t camera = {0};
    camera.aspect_ratio      = 1.0;
    camera.image_width       = 600;  
    camera.samples_per_pixel = 200; 
    camera.max_depth         = 50;
    camera.vfov              = 40;  
    camera.lookfrom          = (point3){278, 278, -800};
    camera.lookat            = (point3){278, 278, 0};
    camera.vup               = (point3){0, 1, 0};
    camera.defocus_angle     = 0.0; 
    camera.focus_dist        = 1.0; // Do not set to zero
    camera.background        = (color){0, 0, 0};
    camera_initialize(&camera);

    // image output
    FILE *img = fopen("output.ppm", "wb");
    
    // raster
    pixel_t** raster = raster_init(camera.image_width, camera.image_height);

    // world
    hittable_list* world = hittable_list_create();

    // materials
    material_t* red   = (material_t*)create_lambertian_color(&(color){.65, .05, .05});
    material_t* white = (material_t*)create_lambertian_color(&(color){.73, .73, .73});
    material_t* green = (material_t*)create_lambertian_color(&(color){.12, .45, .15});
    material_t* light = (material_t*)create_diffuse_light_color(&(color){7, 7, 7});

    // cornell box walls
    hittable_list_add(world, (hittable*)quad_create(&(point3){555, 0, 0}, &(vec3){0, 555, 0}, &(vec3){0, 0, 555}, green)); // Right wall
    hittable_list_add(world, (hittable*)quad_create(&(point3){0, 0, 0}, &(vec3){0, 555, 0}, &(vec3){0, 0, 555}, red));   // Left wall
    hittable_list_add(world, (hittable*)quad_create(&(point3){114, 554, 127}, &(vec3){330, 0, 0}, &(vec3){0, 0, 305}, light)); // Light
    hittable_list_add(world, (hittable*)quad_create(&(point3){0, 0, 0}, &(vec3){555, 0, 0}, &(vec3){0, 0, 555}, white)); // Floor
    hittable_list_add(world, (hittable*)quad_create(&(point3){555, 555, 555}, &(vec3){-555, 0, 0}, &(vec3){0, 0, -555}, white)); // Ceiling
    hittable_list_add(world, (hittable*)quad_create(&(point3){0, 0, 555}, &(vec3){555, 0, 0}, &(vec3){0, 555, 0}, white)); // Back wall

    // material_t* fog_texture = (material_t*)create_solid_color(0.8, 0.8, 0.8);
    // hittable* fog_boundary = (hittable*)sphere_create(&(point3){278, 278, 278}, 150, NULL);
    // hittable* fog = (hittable*)volume_create(fog_boundary, 0.01, (texture_t*)fog_texture);
    // hittable_list_add(world, fog);

    // First box
    point3 box1_min = (point3){0, 0, 0};
    point3 box1_max = (point3){165, 330, 165};
    hittable* box1 = create_box(&box1_min, &box1_max, white);
    hittable* rotated_box1 = (hittable*)rotate_object_y(box1, 15);
    hittable* translated_box1 = (hittable*)translate_object(rotated_box1, &(vec3){265, 0, 295});
    // hittable_list_add(world, translated_box1);

    // Second box
    point3 box2_min = (point3){0, 0, 0};
    point3 box2_max = (point3){165, 165, 165};
    hittable* box2 = create_box(&box2_min, &box2_max, white);
    hittable* rotated_box2 = (hittable*)rotate_object_y(box2, -18);
    hittable* translated_box2 = (hittable*)translate_object(rotated_box2, &(vec3){130, 0, 65});
    // hittable_list_add(world, translated_box2);

    hittable* volume1 = (hittable*)volume_create(translated_box1, 0.01, (texture_t*)create_solid_color(0, 0, 0));
    hittable* volume2 = (hittable*)volume_create(translated_box2, 0.01, (texture_t*)create_solid_color(1, 1, 1));

    hittable_list_add(world, volume1);
    hittable_list_add(world, volume2);


    // create BVH from hittable list
    size_t object_count = darray_size(world->objects);
    hittable **objects = (hittable **)world->objects->data;

    bvh_node_t* bvh_world = bvh_node_init(objects, 0, object_count);
    if (!bvh_world) {
        fprintf(stderr, "Error: BVH creation failed\n");
        hittable_list_destroy(world);
        fclose(img);
        return 1;
    }

    // render loop
    double start_time = omp_get_wtime();

    render(&camera, bvh_world, raster);
    raster_to_ppm(raster, camera.image_width, camera.image_height, img);

    double end_time = omp_get_wtime();

    double elapsed_time = end_time - start_time;
    int minutes = (int)(elapsed_time / 60);
    int seconds = (int)elapsed_time % 60;
    printf("\nRender completed in %d minutes and %d seconds.\n", minutes, seconds);

    // cleanup
    fclose(img);
    hittable_list_destroy(world);
    bvh_node_free(bvh_world);


    return 0;
}
