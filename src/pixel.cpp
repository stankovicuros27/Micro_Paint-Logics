#include "pixel.h"
#include "exception.h"
#include <algorithm>

Pixel::Pixel(Uint32* rgba) {
    r_ = ((Uint8*)rgba)[0];
    g_= ((Uint8*)rgba)[1];
    b_ = ((Uint8*)rgba)[2];
    a_ = ((Uint8*)rgba)[3];
}

Pixel::~Pixel() {}

Uint32 Pixel::get_32bit() {
    r_ = std::max(r_, 0);
    r_ = std::min(r_, 255);
    g_ = std::max(g_, 0);
    g_ = std::min(g_, 255);
    b_ = std::max(b_, 0);
    b_ = std::min(b_, 255);
    a_ = std::max(a_, 0);
    a_ = std::min(a_, 255);
    Uint8 ret[4];
    ret[0] = (Uint8)r_;
    ret[1] = (Uint8)g_;
    ret[2] = (Uint8)b_;
    ret[3] = (Uint8)a_;
    return *(Uint32*)ret;
}

std::vector<Pixel> Pixel::pixels_from_surface(SDL_Surface* surface) {
    std::vector<Pixel> vec;
    Uint32* pixels = (Uint32*)surface->pixels;
    for (int h = 0; h < surface->h; h++) {
        for (int w = 0; w < surface->w; w++) {
            Uint32 temp = (Uint32)(pixels[h * surface->w + w]);
            vec.push_back(Pixel(&temp));
        }
    }
    return vec;
}

SDL_Surface* Pixel::surface_from_pixels(std::vector<Pixel> pixels, int w, int h) {
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    if(!surface )throw Exception("Failed to create surface from pixels!");
    for (int h = 0; h < surface->h; h++) {
        for (int w = 0; w < surface->w; w++) {
            ((Uint32*)surface->pixels)[surface->w * h + w] = pixels[surface->w * h + w].get_32bit();
        }
    }
    return surface;
}

std::ostream& operator<<(std::ostream& os, const Pixel& pixel) {
    unsigned char r, g, b, a;
    r = pixel.r_;
    g = pixel.g_;
    b = pixel.b_;
    a = pixel.a_;
    return os << r << g << b << a;
}