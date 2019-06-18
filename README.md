# Computer-Graphics-From-Scratch
Simple raytracer made to learn the basiscs of computer graphics

## Exemplary Rendering scene
```bash
sphere_t spheres[SPHERES] = {sphere_t{coords_t{0, -1, 3}, 1, rgb_t{255, 0, 0}, 500, 0.2},
                             sphere_t{coords_t{2, 0, 4}, 1, rgb_t{0, 0, 255}, 500, 0.3},
                             sphere_t{coords_t{-3, 0, 4}, 2, rgb_t{0, 255, 0}, 10, 0.4},
                             sphere_t{coords_t{0, -5001, 0}, 5000, rgb_t{70, 70, 70}, 50, 0.1},
                             sphere_t{coords_t{80, 80, 250}, 20, rgb_t{80, 89, 150}, 700, 0.4}};
```
![alt text](result_image.bmp)

## Assumptions
* Recursive ray-tracing with maximum depth equal 3
* Only white color light for now
* No atmosphere (light does not loose intensity over distance)
* Diffuse and specular reflections
* Point, directional and ambient light sources

## Running the rederer on a sample scene
```bash
mkdir build
cd build
cmake ..
make
./renderer
```