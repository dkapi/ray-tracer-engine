#include <time.h>
#include "engine.h"


void generate_varied_terrain_with_boxes(perlin_t* perlin, hittable_list* scene, int width, int depth, int min_height, int max_height, double scale, material_t* grass_top_mat, material_t* grass_side_mat, material_t* dirt_mat, material_t* stone_mat) {
    for (int x = 0; x < width; x++) {
        for (int z = 0; z < depth; z++) {
            // Generate height using Perlin noise
            point3 p = {x * scale, 0, z * scale};
            double noise_value = perlin_noise(perlin, &p);
            int height = min_height + (int)((noise_value + 1.0) * 0.5 * (max_height - min_height));

            // Create blocks for this column
            for (int y = 0; y < height; y++) {
                point3 min = {x, y, z};
                point3 max = {x + 1, y + 1, z + 1}; // Block size is 1x1x1

                if (y == height - 1) {
                    // Grass block with textured top and sides
                    hittable_list_add(scene, create_textured_box(&min, &max, grass_top_mat, grass_side_mat, dirt_mat));
                } else if (y >= height - 4) {
                    // Subsurface dirt block
                    hittable_list_add(scene, create_box(&min, &max, dirt_mat));
                } else {
                    // Underground stone block
                    hittable_list_add(scene, create_box(&min, &max, stone_mat));
                }
            }
        }
    }
}


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
    camera.lookfrom          = (point3){25 , 20, -40}; // Adjusted for terrain view
    camera.lookat            = (point3){25 , 5, 25};   // Center the camera on the terrain
    camera.vup               = (point3){0 , 1, 0};
    camera.defocus_angle     = 0.0; 
    camera.focus_dist        = 40.0; // Set to match terrain scale
    camera.background        = (color){0.4, 0.4, 0.4};
    camera_initialize(&camera);

    // Image output
    FILE *img = fopen("output.ppm", "wb");
    
    // Raster
    pixel_t** raster = raster_init(camera.image_width, camera.image_height);

    // Cubemap
    const char* cubemap_path[6] = {
        "textures/cubemaps/space_skymap/right.png",  // right
        "textures/cubemaps/space_skymap/left.png",   // left
        "textures/cubemaps/space_skymap/top.png",    // top
        "textures/cubemaps/space_skymap/bottom.png", // bottom
        "textures/cubemaps/space_skymap/back.png",  // back
        "textures/cubemaps/space_skymap/front.png"   // front
    };
    cubemap_t* cubemap = create_cubemap(cubemap_path);

    // World setup
    hittable_list* world = hittable_list_create();

    // Materials for terrain
    material_t* grass_mat = (material_t*)create_lambertian_color(&(color){0.2,0.8,0.2}); // Green grass
    material_t* dirt_mat  = (material_t*)create_lambertian_color(&(color){0.5, 0.4, 0.3});  // Brown dirt
    material_t* stone_mat = (material_t*)create_lambertian_color(&(color){0.5, 0.5, 0.5}); // Gray stone

    texture_t* grass_top =    (texture_t*)create_image_texture("textures/texture_images/minecraft_textures/grass_top.jpg");
    texture_t* grass_side =   (texture_t*)create_image_texture("textures/texture_images/minecraft_textures/grass_side.jpg");
    texture_t* grass_bottom = (texture_t*)create_image_texture("textures/texture_images/minecraft_textures/dirt.jpg");

    material_t* g_top = (material_t*)create_lambertian_texture(grass_top);
    material_t* g_side = (material_t*)create_lambertian_texture(grass_side);
    material_t* g_bottom = (material_t*)create_lambertian_texture(grass_bottom);

    // Procedural terrain generation
    perlin_t* perlin = perlin_create();
    int terrain_width = 50, terrain_depth = 50;
    int min_height = 5, max_height = 20;
    double scale = 0.1; // Controls terrain feature size

    generate_varied_terrain_with_boxes(perlin, world, terrain_width, terrain_depth, min_height, max_height, scale, g_top, g_side, g_bottom, stone_mat);

    // Optional: Add other objects (e.g., spheres or reflective objects)
    material_t* reflective_material = (material_t*)create_metal(&(color){0.8, 0.8, 0.8}, 0.0); // Perfect mirror
    hittable* reflective_sphere = (hittable*)sphere_create(&(point3){5, 15, 5}, 3.0, reflective_material);
    hittable_list_add(world, reflective_sphere);

    // Create BVH from hittable list
    size_t object_count = darray_size(world->objects);
    hittable **objects = (hittable **)world->objects->data;

    bvh_node_t* bvh_world = bvh_node_init(objects, 0, object_count);
    if (!bvh_world) {
        fprintf(stderr, "Error: BVH creation failed\n");
        hittable_list_destroy(world);
        fclose(img);
        return 1;
    }

    // Render loop
    double start_time = omp_get_wtime();

    render(&camera, bvh_world, raster, NULL, NULL); // Removed HDR texture for simplicity
    raster_to_ppm(raster, camera.image_width, camera.image_height, img);

    double end_time = omp_get_wtime();

    double elapsed_time = end_time - start_time;
    int minutes = (int)(elapsed_time / 60);
    int seconds = (int)elapsed_time % 60;
    printf("\nRender completed in %d minutes and %d seconds.\n", minutes, seconds);

    // Cleanup
    fclose(img);
    hittable_list_destroy(world);
    bvh_node_free(bvh_world);
    perlin_free(perlin);
    free(grass_mat);
    free(dirt_mat);
    free(stone_mat);
    free(reflective_material);

    return 0;
}