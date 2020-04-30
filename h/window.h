#ifndef WINDOW_H
#define WINDOW_H

#include <sdl.h>
#include <iostream>
#include <string>
#include "image.h"
#include "pixel.h"

class Image;

class Window {
public:
	Window(std::string title, const int& width, const int& height);
	virtual ~Window();

	SDL_Window* get_window() { return window_; }
	SDL_Surface* get_surface() { return surface_; }
	SDL_Renderer* get_renderer() { return renderer_; }
	bool is_closed() const { return closed_; }
	void set_closed(bool closed) { closed_ = closed; }
	Image* get_image() const { return image_; }
	void set_image(Image* image) { image_ = image; }

	void poll_events();
	void clear_screen() const;
	void update_screen() const;

private:
	bool init();

private:
	SDL_Window* window_ = nullptr;
	SDL_Renderer* renderer_ = nullptr;
	SDL_Surface* surface_ = nullptr;
	SDL_PixelFormat* format_ = nullptr;
	Image* image_ = nullptr;

	std::string title_;
	int w_, h_;
	bool closed_ = false;
};

#endif
