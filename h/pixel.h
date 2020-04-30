#ifndef PIXEL_H
#define PIXEL_H
#include "SDL.h"
#include <vector>
#include <iostream>

class Pixel {
public:
	Pixel(Uint32* rgba);
	~Pixel();

	Uint32 get_32bit();

	friend std::ostream& operator<<(std::ostream& os, const Pixel& pixel);
	static std::vector<Pixel> pixels_from_surface(SDL_Surface* surface);
	static SDL_Surface* surface_from_pixels(std::vector<Pixel> pixels, int w, int h);
	int r_ = 0;
	int g_ = 0;
	int b_ = 0;
	int a_ = 0;
};

#endif
