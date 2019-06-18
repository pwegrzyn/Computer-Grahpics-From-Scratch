#include <iostream>

#include "painter.hpp"
#include "bitmap_image.hpp"

using namespace render;

int main(int argc, char** argv) {
    
    Painter painter(800, 600, "result.ppm");
    painter.init_img();
    painter.cleanup();

}