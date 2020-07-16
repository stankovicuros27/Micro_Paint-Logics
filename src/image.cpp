#include "image.h"
#include <algorithm>
#include <iostream>
#include "pixel.h"
#include "compoperation.h"
#include "window.h"
#include "queue"
#include "exception.h"
#include <vector>


Image::Image() {
	rect_ = { 0,0,0,0 };
	surface_ = SDL_CreateRGBSurfaceWithFormat(0, 100, 100, 32, SDL_PIXELFORMAT_RGBA32);
	if (surface_ == nullptr) {
		std::cerr << "Image surface could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		throw Exception("Could not create SDL_Surface!");
	}
	SDL_SetColorKey(surface_, SDL_TRUE, SDL_PIXELFORMAT_RGBA32);
}

Image::~Image() {
	for (std::vector<Layer*>::iterator iter = layers_.begin(); iter != layers_.end(); iter++) {
		delete* iter;
	}
	SDL_FreeSurface(surface_);
	surface_ = nullptr;
}

void Image::add_layer(int x, int y, int w, int h, int opacity) {
	if (opacity > 100) opacity = 100;
	if (opacity < 0) opacity = 0;
	if (x > surface_->w || y > surface_->h) {
		std::cerr << "Cannot create Layer outside of the Image!" << std::endl;
		throw Exception("Bad Layer dimensions!");
	}
	Layer* layer = new Layer(x, y, w, h, this, opacity);
	layers_.push_back(layer);
	update_rect_surface(*layer->get_rect());
	int max_w = surface_->w;
	int max_h = surface_->h;

	std::for_each(layers_.begin(), layers_.end(), [&](Layer* layer) {
		layer->update_surface(max_w, max_h);
	});

	if (opacity != 100) {
		Operation operation("opacity", opacity);
		layer->perform_operation_whole_layer(operation);
	}

	blit_layers();
}

void Image::add_layer(int x, int y, const std::string& path, int opacity) {
	if (opacity > 100) opacity = 100;
	if (opacity < 0) opacity = 0;
	if (x > surface_->w || y > surface_->h) {
		std::cerr << "Cannot create Layer outside of the Image!" << std::endl;
		throw Exception("Bad Layer dimensions!");
	}
	Layer* layer = new Layer(x, y, path, this, opacity);
	if (!layer->get_surface()) throw Exception("Cannot create Layer SDL_Surface!");;
	layers_.push_back(layer);
	update_rect_surface(*layer->get_rect());
	int max_w = surface_->w;
	int max_h = surface_->h;

	std::for_each(layers_.begin(), layers_.end(), [&](Layer* layer) {
		layer->update_surface(max_w, max_h);
	});

	if (opacity != 100) {
		Operation operation("opacity", opacity);
		layer->perform_operation_whole_layer(operation);
	}

	layer->path = path;
	
	blit_layers();
}

void Image::remove_layer(int id) {
	Layer* layer = find_layer_by_id(id);
	if (!layer) return;
	std::vector<Layer*>::iterator iter = layers_.begin();
	while ((*iter != layer && iter != layers_.end())) iter++;
	if(iter != layers_.end()) layers_.erase(iter);
	std::cout << "Layer removed!" << std::endl;
	delete layer;
	blit_layers();
}

Layer* Image::get_layer(int index) {
	if (index < 0 || index >= layers_.size()) {
		std::cerr << "Invalid Image Layers indexing!" << std::endl;
		return nullptr;
	}
	else {
		return layers_[index];
	}
}

Layer* Image::find_layer_by_id(int id) {
	for (std::vector<Layer*>::iterator iter = layers_.begin(); iter != layers_.end(); iter++) {
		if ((*iter)->get_id() == id) return *iter;
	}
	std::cerr << "Layer with given ID not found!" << std::endl;
	return nullptr;
}

void Image::blit_layers() {
	SDL_FillRect(surface_, nullptr, SDL_MapRGBA(surface_->format, 0xff, 0xff, 0xff, 0x00));
	std::for_each(layers_.begin(), layers_.end(), [this](Layer* layer) {
		if (layer->is_active()) {
			SDL_SetColorKey(layer->get_surface(), SDL_TRUE, SDL_PIXELFORMAT_RGBA32);
			SDL_BlitSurface(layer->get_surface(), nullptr, surface_, nullptr);
		}
	});
}

void Image::update_rect_surface(const SDL_Rect &rect) {
	if (rect.w + rect.x > rect_.w) rect_.w = rect.w + rect.x;
	if (rect.h + rect.y > rect_.h) rect_.h = rect.h + rect.y;

	SDL_FreeSurface(surface_);

	SDL_PixelFormat* f = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
	surface_ = SDL_CreateRGBSurfaceWithFormat(0, rect_.w, rect_.h, 32, f->format);
	if (surface_ == nullptr) {
		std::cerr << "Image surface could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		throw Exception("Could not create SDL_Surface!");
	}
	SDL_FreeFormat(f);
}

void Image::add_selection(int x, int y, int w, int h) {
	if (x + w > surface_->w || y + h > surface_->h) {
		std::cerr << "Cannot create Selection outside of the Image!" << std::endl;
		throw Exception("Bad Selection dimensions!");
	}
	Selection* selection = new Selection(x, y, w, h, this);
	selections_.push_back(selection);
}

Selection* Image::get_selection(int index) {
	if (index < 0 || index >= selections_.size()) {
		std::cerr << "Invalid Image Selection indexing!" << std::endl;
		return nullptr;
	}
	else {
		return selections_[index];
	}
}

void Image::remove_selection(int id) {
	Selection* selection = find_selection_by_id(id);
	if (!selection) return;
	std::vector<Selection*>::iterator iter = selections_.begin();
	while ((*iter != selection && iter != selections_.end())) iter++;
	if (iter != selections_.end()) selections_.erase(iter);
	std::cout << "Selection removed!" << std::endl;
	delete selection;
}

Selection* Image::find_selection_by_id(int id) {
	for (std::vector<Selection*>::iterator iter = selections_.begin(); iter != selections_.end(); iter++) {
		if ((*iter)->get_id() == id) return *iter;
	}
	std::cerr << "Selection with given ID not found!" << std::endl;
	return nullptr;
}

void Image::perform_operation(Operation &operation) {
	if (selections_.empty())
		perform_operation_whole_image(operation);
	else
		perform_operation_active_selections(operation);
}

void Image::perform_operation_whole_image(Operation& operation) {
	std::for_each(layers_.begin(), layers_.end(), [&](Layer* layer) {
		layer->perform_operation_whole_layer(operation);
	});
	blit_layers();
}

void Image::perform_operation_active_selections(Operation& operation) {
	std::for_each(selections_.begin(), selections_.end(), [&](Selection* selection) {
		selection->perform_operation_whole_selection(operation);
	});
}
