#ifndef LAYER_H
#define LAYER_H
#include <sdl.h>
#include <string>
#include <queue>
#include <iostream>

class Image;
class Operation;

class Layer {
public:
	SDL_Surface* get_surface() { return surface_; }
	SDL_Rect* get_rect() { return &rect_; }
	int get_id() const { return id_; }
	bool is_active() const { return active_; }
	void set_active(bool flag);
	friend std::ostream& operator<<(std::ostream& os, const Layer& layer);

public:
	Layer(int w, int h, int x, int y, Image* image, int opacity = 100);
	Layer(int x, int y, const std::string& path, Image *image, int opacity = 100);
	~Layer();

	bool load_surface(const std::string& path);
	void update_surface(int x, int y);
	void delete_surface();

	void perform_operation_whole_layer(Operation& operation);

	std::string path;
	SDL_Rect rect_;

private:
	SDL_Surface* surface_ = nullptr;

	SDL_Rect creation_rect_;
	Image* my_image_;

	int id_ = global_layer_id++;
	int opacity_;
	bool active_ = true;

	static int global_layer_id;
	friend class Image;
	friend class Selection;
};


#endif