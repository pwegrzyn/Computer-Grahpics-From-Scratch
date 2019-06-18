#ifndef PAINTER_H
#define PAINTER_H

#include <string>
#include <fstream>

namespace render 
{
    class Painter 
    {
    private:
        int width;
        int height;
        std::ofstream *img;
    public:
        Painter(int width, int height, std::string file_name);
        void init_img();
        void draw_pixel(int x, int y, int r, int g, int b);
        void cleanup();
    };

    bool hasEnding(std::string const &fullString, std::string const &ending);
}

#endif