#ifndef IMAGE_H
#define IMAGE_H

#include <sdl.h>
#include <vector>
#include <string>
#include "layer.h"
#include "selection.h"
#include "queue"
#include "string"

class Window;
class Layer;
class Operation;

class Image {
public:
	Image();
	~Image();

	void add_layer(int x, int y, int w, int h, int opacity);
	void add_layer(int x, int y, const std::string& path, int opacity);
	void remove_layer(int id);

	Layer* get_layer(int index);

	void add_selection(int x, int y, int w, int h);
	Selection* get_selection(int index);
	void remove_selection(int id);

	void perform_operation(Operation &operation);

	SDL_Surface* get_surface() { return surface_; }
	SDL_Rect* get_rect() { return &rect_; }

	Window* get_my_window() { return my_window_; }
	void set_my_window(Window *window) { my_window_ = window; }

	Layer* find_layer_by_id(int id);
	Selection* find_selection_by_id(int id);

public:
	std::vector<Layer*> layers_;
	std::vector<Selection*> selections_;

private:
	void blit_layers();
	void update_rect_surface(const SDL_Rect &rect);
	void perform_operation_whole_image(Operation& operation);
	void perform_operation_active_selections(Operation& operation);

private:
	SDL_Surface* surface_ = nullptr;
	SDL_Rect rect_;

	Window* my_window_ = nullptr;

	friend class Selection;
	friend class Layer;
	friend class Formatter;
};

#endif