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
    camera.focus_dist        = 1.0; // do not set to zero :D
    camera.background        = (color){0,0,0};
    camera_initialize(&camera);

    // image output
    FILE *img = fopen("output.ppm", "wb");
    
    // raster
    pixel_t** raster = raster_init(camera.image_width, camera.image_height);

    // world
    hittable_list* world = hittable_list_create();

    material_t* red   = (material_t*)create_lambertian_color(&(color){.65,.05,.05});
    material_t* white = (material_t*)create_lambertian_color(&(color){.73,.73,.73});
    material_t* green = (material_t*)create_lambertian_color(&(color){.12,.45,.15});
    material_t* light = (material_t*)create_diffuse_light_color(&(color){15,15,15});

    quads_t* q1 = quad_create(&(point3){555,0,0}, &(vec3){0,555,0}, &(vec3){0,0,555}, green);
    hittable_list_add(world, (hittable*)q1);
    quads_t* q2 = quad_create(&(point3){0,0,0}, &(vec3){0,555,0}, &(vec3){0,0,555}, red);
    hittable_list_add(world, (hittable*)q2);
    quads_t* q3 = quad_create(&(point3){343,554,332}, &(vec3){-130,0,0}, &(vec3){0,0,-105}, light);
    hittable_list_add(world, (hittable*)q3);
    quads_t* q4 = quad_create(&(point3){0,0,0}, &(vec3){555,0,0}, &(vec3){0,0,555}, white);
    hittable_list_add(world, (hittable*)q4);
    quads_t* q5 = quad_create(&(point3){555,555,555}, &(vec3){-555,0,0}, &(vec3){0,0,-555}, white);
    hittable_list_add(world, (hittable*)q5);
    quads_t* q6 = quad_create(&(point3){0,0,555}, &(vec3){555,0,0}, &(vec3){0,555,0}, white);
    hittable_list_add(world, (hittable*)q6);

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
