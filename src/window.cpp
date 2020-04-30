#include "window.h"
#include "algorithm"
#include "operation.h"


Window::Window(std::string title, const int& width, const int& height) :
	title_(title), w_(width), h_(height)
{
	closed_ = !init();
}

Window::~Window() {
    SDL_DestroyWindow(window_);
    SDL_DestroyRenderer(renderer_);
    SDL_FreeFormat(format_);
    SDL_Quit();
}

bool Window::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 0;
    }

    window_ = SDL_CreateWindow(
        title_.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        w_, h_,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
        );

    if (window_ == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    }

    surface_ = SDL_GetWindowSurface(window_);

    SDL_SetColorKey(surface_, SDL_TRUE, SDL_PIXELFORMAT_RGBA32);

    renderer_ = SDL_CreateRenderer(
        window_,
        -1,
        SDL_RENDERER_PRESENTVSYNC
        );

    if (renderer_ == nullptr) {
        std::cerr << "Window renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    }

    format_ = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);

    if (format_ == nullptr) {
        std::cerr << "Window format could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    }

    return true;
}

void Window::poll_events() {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
           closed_ = true;
           break;
        default:
           break;
        }
    }
}

void Window::clear_screen() const {
    SDL_SetRenderDrawColor(renderer_, 0xff, 0xff, 0xff, 0x00);
    SDL_RenderClear(renderer_);
}

void Window::update_screen() const {
    if (image_ != nullptr) {
        SDL_SetWindowSize(window_, image_->get_surface()->w, image_->get_surface()->h);
        SDL_Texture* img_texture = SDL_CreateTextureFromSurface(renderer_, image_->get_surface());
        if (img_texture == nullptr) {
            std::cerr << "Could not create image texture in update_screen! SDL_Error: " << SDL_GetError() << std::endl;
        }
        SDL_RenderCopy(renderer_, img_texture, nullptr, image_->get_rect()); //mozda ovde za velicinu
        SDL_DestroyTexture(img_texture);
        SDL_RenderPresent(renderer_);
    }
}