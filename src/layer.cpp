#include "layer.h"
#include <iostream>
#include "formatter.h"
#include "image.h"
#include "pixel.h"
#include "operation.h"
#include <queue>
#include "compoperation.h"
#include "exception.h"

int Layer::global_layer_id = 0;

Layer::Layer(int x, int y, int w, int h, Image* image, int opacity) : 
	my_image_(image), opacity_(opacity)
{
	rect_.x = x;
	rect_.y = y;
	rect_.w = w;
	rect_.h = h;
	creation_rect_.x = 0;
	creation_rect_.y = 0;
	surface_ = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
	creation_rect_.w = w;
	creation_rect_.h = h;

	if (surface_ == nullptr) {
		std::cerr << "Image surface could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		throw Exception("Could not create SDL_Surface!");
	}

	SDL_SetColorKey(surface_, SDL_TRUE, SDL_PIXELFORMAT_RGBA32);
}

Layer::Layer(int x, int y, const std::string& path, Image* image, int opacity) : 
	my_image_(image), opacity_(opacity)
{
	rect_.x = x;
	rect_.y = y;
	creation_rect_.x = 0;
	creation_rect_.y = 0;

	load_surface(path);

	creation_rect_.w = surface_->w;
	creation_rect_.h = surface_->h;
	rect_.w = surface_->w;
	rect_.h = surface_->h;

	SDL_SetColorKey(surface_, SDL_TRUE, SDL_PIXELFORMAT_RGBA32);
}

Layer::~Layer() {
	if (surface_ != nullptr) {
		SDL_FreeSurface(surface_);
		surface_ = nullptr;
	}
	active_ = false; 
}

bool Layer::load_surface(const std::string& path) {
	if (surface_ != nullptr) {
		std::cerr << "Layer SDL_Surface already initialized! Delete it first to initialize new!" << std::endl;
		return false;
	}
	if (path.substr(path.find_last_of(".") + 1) == "bmp") {
		Formatter::import_bmp(&surface_, path);
	}
	else if (path.substr(path.find_last_of(".") + 1) == "pam") {
		Formatter::import_pam(&surface_, path);
	}
	else {
		std::cout << "Unsupported Image extension!" << std::endl;
		return false;
	}
	
	return true;
}

void Layer::delete_surface() {
	if (surface_ != nullptr) {
		SDL_FreeSurface(surface_);
		surface_ = nullptr;
	}
}

void Layer::update_surface(int w, int h) {
	SDL_Surface* temp = surface_;
	surface_ = SDL_CreateRGBSurface(0, w, h, 32, 0xff, 0xff00, 0xff0000, 0xff000000);
	if (surface_ == nullptr) {
		std::cerr << "Image surface could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		throw Exception("Could not create SDL_Surface!");
	}
	SDL_BlitSurface(temp, &creation_rect_, surface_, &rect_);
	if (creation_rect_.x == 0) {
		creation_rect_.x = rect_.x;
	}
	if (creation_rect_.y == 0) {
		creation_rect_.y = rect_.y;
	}

	SDL_FreeSurface(temp);
}

void Layer::perform_operation_whole_layer(Operation& operation) {
	std::vector<Pixel> vec = Pixel::pixels_from_surface(surface_); 
	if (operation.get_operation_code() == "median") {
		std::vector<Pixel> temp(vec);
		int w = surface_->w;
		int h = vec.size() / w;
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				if (i > 0 && i < h - 1 && j > 0 && j < w - 1) {
					operation.median(vec[i * w + j], temp, i, j, w, h);
				}
			}
		}
	}
	else operation.execute(vec);
	SDL_Surface* new_surface = Pixel::surface_from_pixels(vec, surface_->w, surface_->h);
	SDL_Surface* toDelete = surface_;
	surface_ = new_surface;
	SDL_FreeSurface(toDelete);
}


std::ostream& operator<<(std::ostream& os, const Layer& layer) {
	return os << "Layer(id:" << layer.get_id() << ")rect:{" << layer.rect_.x << "," 
		<< layer.rect_.y << "," << layer.rect_.w << "," << layer.rect_.h << "}surface:{"
		<< layer.surface_->w << "," << layer.surface_->h << "}"
		<< (layer.active_ ? " active" : " not active") << std::endl;
}

void Layer::set_active(bool flag){ active_ = flag; my_image_->blit_layers(); }