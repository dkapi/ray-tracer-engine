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
    camera.aspect_ratio      = 16.0 / 9.0;
    camera.image_width       = 1440;  
    camera.samples_per_pixel = 500; 
    camera.max_depth         = 50;
    camera.vfov              = 40;  
    camera.lookfrom          = vec3_create_values(-10.0, 4.0, 4.0); 
    camera.lookat            = vec3_create_values(0.0, 0.0, 0.0); 
    camera.vup               = vec3_create_values(0.0, 1.0, 0.0); 
    camera.defocus_angle     = 0.0; 
    camera.focus_dist        = 1.0;
    camera_initialize(&camera);

    // image output
    FILE *img = fopen("output.ppm", "wb");
    
    // raster
    pixel_t** raster = raster_init(camera.image_width, camera.image_height);

    // world
    hittable_list* world = hittable_list_create();

    // Load an image texture
    texture_t* image_tex = (texture_t*)create_image_texture("textures/texture_images/Red_brick_wall_texture.jpg");
    color center_color = vec3_create_values(0.4, 0.2, 0.1);
    material_t *material2 = (material_t *)create_lambertian_color(&center_color);

    // Create a Lambertian material using the image texture
    material_t* mat_tex = (material_t*)create_lambertian_texture(image_tex);

    mesh_t* mesh = load_obj("meshes/teapot.obj", material2);

    // Add mesh triangles to the world
    add_mesh_to_world(mesh, world);

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
    free_mesh(mesh);


    return 0;
}
