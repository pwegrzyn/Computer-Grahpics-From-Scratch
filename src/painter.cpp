#include <string>

#include "painter.h"

using namespace render;

Painter::Painter(int width, int height, std::string file_name) 
{
    width = width;
    height = height;
    if(hasEnding(file_name, std::string(".ppm"))) {
        img = new std::ofstream(file_name);
    } else {
        img = new std::ofstream(file_name + ".ppm");
    }
}

void Painter::init_img() 
{
    *img << "P3" << std::endl;
    *img << width << " " << height << std::endl;
    *img << "255" << std::endl;
}

void Painter::draw_pixel(int x, int y, int r, int g, int b) 
{
    *img << r << " " << g << " " << b << std::endl;
}

void Painter::cleanup() 
{
    delete img;
}

bool render::hasEnding(std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length())
    {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else
    {
        return false;
    }
}