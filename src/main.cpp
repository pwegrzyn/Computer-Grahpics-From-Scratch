#include <iostream>
#include <math.h>
#include <algorithm>

#include "painter.hpp"
#include "bitmap_image.hpp"

using namespace render;

#define EPSILON 0.001
#define INF (1 << 30)
#define CANVAS_WIDTH 600
#define CANVAS_HEIGHT 600
#define VIEWPORT_WIDTH 1
#define VIEWPORT_HEIGHT 1
#define VIEWPORT_DIST 1
#define SPHERES 5
#define LIGHTS 3
#define BACKGROUND_COLOR rgb_t {0, 0, 0}

// COORDS
typedef struct coords_tag
{
    double x;
    double y;
    double z;
} coords_t;

// SCENE OBJECTS
typedef struct sphere_tag
{
    coords_t center;
    double radius;
    rgb_t color;
    int specular;
    double reflective;
} sphere_t;

typedef struct sphere_intersect_tag
{
    double t_1;
    double t_2;
} sphere_intersect_t;

typedef struct closest_intersection_tag
{
    int closest_sphere;
    double closest_t_val;
} closest_intersection_result;

// LIGHT SOURCES
typedef enum light_source_type_tag
{
    AMBIENT,
    POINT,
    DIRECTIONAL
} light_source_type_t;

typedef struct light_tag
{
    light_source_type_t type;
    double intensity;
    coords_t loc; // direction vector or position point
} light_t;

// -------------------- Add any spheres you want to the scene -----------------
static sphere_t spheres[SPHERES] = {sphere_t{coords_t{0, -1, 3}, 1, rgb_t{255, 0, 0}, 500, 0.2},
                             sphere_t{coords_t{2, 0, 4}, 1, rgb_t{0, 0, 255}, 500, 0.3},
                             sphere_t{coords_t{-3, 0, 4}, 2, rgb_t{0, 255, 0}, 10, 0.4},
                             sphere_t{coords_t{0, -5001, 0}, 5000, rgb_t{120, 120, 0}, 50, 0.1},
                             sphere_t{coords_t{80, 80, 250}, 20, rgb_t{80, 89, 150}, 700, 0.4}};

// -------------------- Add any light sources you want to the scene ----------
static light_t lights[LIGHTS] = {light_t{AMBIENT, 0.2, coords_t {0,0,0}},
                          light_t{POINT, 0.6, coords_t {2,1,0}},
                          light_t{DIRECTIONAL, 0.2, coords_t{1,4,4}}};

static inline double canvas_to_viewport_x(int x) 
{
    return x * ((double)VIEWPORT_WIDTH / CANVAS_WIDTH);
}

static inline double canvas_to_viewport_y(int y) 
{
    return y * ((double)VIEWPORT_HEIGHT / CANVAS_HEIGHT);
}

static inline double canvas_to_viewport_z() 
{
    return VIEWPORT_DIST;
}

static void put_pixel(bitmap_image* image, int x, int y, rgb_t color)
{
    int x_prim = (int)(((double)CANVAS_WIDTH / 2) + x);
    int y_prim = (int)(((double)CANVAS_HEIGHT / 2) - y - 1);
    if (x_prim < 0 || x_prim >= CANVAS_HEIGHT || y_prim < 0 || y_prim >= CANVAS_HEIGHT)
    {
        return;
    }
    image->set_pixel(x_prim, y_prim, color);
}

static double dot(coords_t a, coords_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static double length(coords_t vector)
{
    return sqrt(dot(vector, vector));
}

static void clamp_color(rgb_t* current_color, int new_red, int new_green, int new_blue)
{
    current_color->red = std::min(255, std::max(0, new_red));
    current_color->green = std::min(255, std::max(0, new_green));
    current_color->blue = std::min(255, std::max(0, new_blue));
}

static sphere_intersect_t* intersect_ray_sphere(coords_t origin, coords_t transformed, sphere_t sphere)
{
    coords_t center = sphere.center;
    double radius = sphere.radius;
    sphere_intersect_t* result = new sphere_intersect_t;
    coords_t oc_vector;

    oc_vector.x = origin.x - center.x;
    oc_vector.y = origin.y - center.y;
    oc_vector.z = origin.z - center.z;
    double k1 = dot(transformed, transformed);
    double k2 = 2 * dot(oc_vector, transformed);
    double k3 = dot(oc_vector, oc_vector) - radius * radius;

    double discriminant = k2 * k2 - 4 * k1 * k3;
    if (discriminant < 0)
    {
        result->t_1 = INF;
        result->t_2 = INF;
        return result;
    }

    result->t_1 = (-k2 + sqrt(discriminant)) / (2 * k1);
    result->t_2 = (-k2 - sqrt(discriminant)) / (2 * k1);
    return result;
}

static closest_intersection_result* closest_intersection(coords_t origin, coords_t transformed, double t_min, double t_max)
{
    closest_intersection_result* result = new closest_intersection_result;
    result->closest_sphere = -1;
    result->closest_t_val = INF;
    for (int i = 0; i < SPHERES; i++)
    {
        sphere_intersect_t* intersect = intersect_ray_sphere(origin, transformed, spheres[i]);
        if (intersect->t_1 > t_min && intersect->t_1 < t_max && intersect->t_1 < result->closest_t_val)
        {
            result->closest_t_val = intersect->t_1;
            result->closest_sphere = i;
        }
        if (intersect->t_2 > t_min && intersect->t_2 < t_max && intersect->t_2 < result->closest_t_val)
        {
            result->closest_t_val = intersect->t_2;
            result->closest_sphere = i;
        }
        delete intersect;
    }
    return result;
}

static double compute_lighting(coords_t P, coords_t N, coords_t V, int s)
{
    double intensity = 0.0;
    for (int i = 0; i < LIGHTS; i++)
    {
        if (lights[i].type == AMBIENT)
        {
            intensity += lights[i].intensity;
        } 
        else
        {
            coords_t L;
            double t_max;
            if (lights[i].type == POINT)
            {
                L.x = lights[i].loc.x - P.x;
                L.y = lights[i].loc.y - P.y;
                L.z = lights[i].loc.z - P.z;
                t_max = 1.0;
            } 
            else
            {
                L.x = lights[i].loc.x;
                L.y = lights[i].loc.y;
                L.z = lights[i].loc.z;
                t_max = INF;
            }
            // Check for possible shadows
            closest_intersection_result* blocker = closest_intersection(P, L, EPSILON, t_max);
            if (blocker->closest_sphere != -1)
            {
                delete blocker;
                continue;
            }
            delete blocker;
            // Diffusion reflections
            double n_dot_l = dot(N, L);
            if (n_dot_l > 0)
            {
                intensity += lights[i].intensity * n_dot_l / (length(N) * length(L));
            }
            // Specular reflections
            if (s != -1)
            {
                coords_t R;
                R.x = 2 * N.x * n_dot_l - L.x;
                R.y = 2 * N.y * n_dot_l - L.y;
                R.z = 2 * N.z * n_dot_l - L.z;
                double r_dot_v = dot(R, V);
                if (r_dot_v > 0)
                {
                    intensity += lights[i].intensity * pow(r_dot_v / (length(R) * length(V)), s);
                }
            }
        }
    }
    return intensity;
}

static rgb_t trace_ray(coords_t origin, coords_t transformed, double t_min, double t_max, int depth)
{
    closest_intersection_result* result = closest_intersection(origin, transformed, t_min, t_max);
    int closest_sphere = result->closest_sphere;
    double closest_t_val = result->closest_t_val;
    delete result;
    if (closest_sphere == -1) 
    {
        return BACKGROUND_COLOR;
    }
    //return spheres[closest_sphere].color;
    coords_t P, N;
    // Compute the intersection
    P.x = origin.x + closest_t_val * transformed.x;
    P.y = origin.y + closest_t_val * transformed.y;
    P.z = origin.z + closest_t_val * transformed.z;
    // Compute sphere normal at the intersection
    N.x = P.x - spheres[closest_sphere].center.x;
    N.y = P.y - spheres[closest_sphere].center.y;
    N.z = P.z - spheres[closest_sphere].center.z;
    // Normalize
    double n_len = length(N);
    N.x /= n_len;
    N.y /= n_len;
    N.z /= n_len;
    // Scale the light by the computed intensity
    rgb_t result_color;
    coords_t V;
    // Thew view vector is simply minus the transformed init vector
    V.x = -transformed.x;
    V.y = -transformed.y;
    V.z = -transformed.z;
    double computed_intensity = compute_lighting(P, N, V, spheres[closest_sphere].specular);
    clamp_color(&result_color, spheres[closest_sphere].color.red * computed_intensity, 
        spheres[closest_sphere].color.green * computed_intensity, 
        spheres[closest_sphere].color.blue * computed_intensity);
    
    // Recursive ray-tracing modification
    double r = spheres[closest_sphere].reflective;
    if (depth <= 0 || r <= 0)
    {
        return result_color;
    }
    coords_t R;
    double v_n_dot = dot(N, V);
    R.x = 2 * v_n_dot * N.x - V.x;
    R.y = 2 * v_n_dot * N.y - V.y;
    R.z = 2 * v_n_dot * N.z - V.z;
    rgb_t reflected_color = trace_ray(P, R, EPSILON, INF, depth - 1);
    clamp_color(&reflected_color, result_color.red * (1 - r) + reflected_color.red * r, 
        result_color.green * (1 - r) + reflected_color.green * r, 
        result_color.blue * (1 - r) + reflected_color.blue * r);
    return reflected_color;
}

int main(int argc, char **argv)
{

    bitmap_image image(CANVAS_WIDTH, CANVAS_HEIGHT);

    int ray_tracing_recursion_limit = 3;
    // Change the postion of the camera
    coords_t camera {0, 0, -2};
    coords_t transformed;
    for (int x = - CANVAS_WIDTH / 2; x < CANVAS_WIDTH / 2; x++)
    {
        for (int y = - CANVAS_HEIGHT / 2; y < CANVAS_HEIGHT / 2; y++)
        {
            transformed.x = canvas_to_viewport_x(x);
            transformed.y = canvas_to_viewport_y(y);
            transformed.z = canvas_to_viewport_z();
            // Multiply a rotation matrix by the transformed vector to obtain the desired rotation
            rgb_t color = trace_ray(camera, transformed, 1, INF, ray_tracing_recursion_limit);
            put_pixel(&image, x, y, color);
        }
    }

    image.save_image("result_image.bmp");
}