#include "selection.h"
#include "algorithm"
#include "image.h"
#include "pixel.h"
#include "operation.h"
#include "compoperation.h"

int Selection::global_selection_id = 0;

Selection::Selection(int x, int y, int w, int h, Image *image) : 
	rect_{ x, y, w, h }, my_image_(image) {}

void Selection::perform_operation_whole_selection(Operation& operation) {

	std::for_each(my_image_->layers_.begin(), my_image_->layers_.end(), [&](Layer* layer) {
		SDL_Surface* temp = SDL_CreateRGBSurfaceWithFormat(0, rect_.w, rect_.h, 32, SDL_PIXELFORMAT_RGBA32);
		SDL_BlitSurface(layer->surface_, &(this->rect_), temp, NULL);
		std::vector<Pixel> vec = Pixel::pixels_from_surface(temp);
		if (operation.get_operation_code() == "median") {
			std::vector<Pixel> tempV(vec);
			int w = temp->w;
			int h = vec.size() / w;
			for (int i = 0; i < h; i++) 
				for (int j = 0; j < w; j++) 
					if (i > 0 && i < h - 1 && j > 0 && j < w - 1) 
						operation.median(vec[i * w + j], tempV, i, j, w, h);						
		}
		else operation.execute(vec);
		SDL_FreeSurface(temp);
		temp = Pixel::surface_from_pixels(vec, temp->w, temp->h);
		SDL_BlitSurface(temp, NULL, layer->surface_, &(this->rect_));
		SDL_FreeSurface(temp);
	});

	my_image_->blit_layers();
}

std::ostream& operator<<(std::ostream& os, const Selection& selection) {
	return os << "Selection(" << selection.get_id() << "){" << selection.rect_.x << "," << selection.rect_.y << "," << selection.rect_.w << "," << selection.rect_.h << "}"
		<< (selection.active_ ? " active" : " not active") << std::endl;;
}