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
    camera.samples_per_pixel = 500; 
    camera.max_depth         = 50;
    camera.vfov              = 40;  
    camera.lookfrom          = (point3){0 , 0, -10};
    camera.lookat            = (point3){0 , 0, 0};
    camera.vup               = (point3){0 , 1, 0};
    camera.defocus_angle     = 0.0; 
    camera.focus_dist        = 1.0; // Do not set to zero
    camera.background        = (color){0, 0, 0};
    camera_initialize(&camera);

    // image output
    FILE *img = fopen("output.ppm", "wb");
    
    // raster
    pixel_t** raster = raster_init(camera.image_width, camera.image_height);

    //cubemap
    const char* cubemap_path[6] = {
        "textures/cubemaps/space_skymap/right.png",  // right
        "textures/cubemaps/space_skymap/left.png",   // left
        "textures/cubemaps/space_skymap/top.png",    // top
        "textures/cubemaps/space_skymap/bottom.png", // bottom
        "textures/cubemaps/space_skymap/back.png",  // back
        "textures/cubemaps/space_skymap/front.png"   // front
    };
    cubemap_t* cubemap = create_cubemap(cubemap_path);

    // world
    hittable_list* world = hittable_list_create();


    material_t* mars_tex = (material_t*)create_lambertian_texture(
        (texture_t*)create_image_texture("textures/texture_images/planet_textures/2k_mars.jpg"));
    material_t* earth_texture = (material_t*)create_lambertian_texture(
        (texture_t*)create_image_texture("textures/texture_images/earthmap.jpg"));
    material_t* reflective_material = (material_t*)create_metal(&(color){0.8, 0.8, 0.8}, 0.0); // Perfect mirror
    hittable* reflective_sphere = (hittable*)sphere_create(&(point3){0, 0, 0}, 1.0, reflective_material);
    hittable* reflective_sphere2 = (hittable*)sphere_create(&(point3){2, 0, 0}, 1.0, reflective_material);
    hittable* mars = (hittable*)sphere_create(&(point3){0,0,0}, 3.0, mars_tex);
    hittable_list_add(world, reflective_sphere);


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

    render(&camera, bvh_world, raster, cubemap);
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
