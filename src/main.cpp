#include <iostream>

#include "painter.h"

using namespace render;

int main(int argc, char** argv) {
    
    render::Painter painter(800, 600, "result.ppm");
    painter.init_img();
    painter.cleanup();

}