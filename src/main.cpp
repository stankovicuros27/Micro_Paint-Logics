#include <iostream>
#include <string>
#include "sdl.h"
#include "window.h"
#include "formatter.h"
#include "pixel.h"
#include "SDL_thread.h"
#include <queue>
#include "exception.h"

void console_output_start() {
	std::cout << "Welcome to Micro_Paint image editing program!";
}

void constole_output() {
	std::cout << std::endl << std::endl << "Select option from the menu:" << std::endl << std::endl;
	std::cout << "1.\tCreate Window" << std::endl;
	std::cout << "2.\tCreate Image" << std::endl;
	std::cout << "3.\tManage Image Layers" << std::endl;
	std::cout << "4.\tManage Image Selections" << std::endl;
	std::cout << "5.\tPerform Operation on Image" << std::endl;
	std::cout << "6.\tPerform Composite Operation on Image" << std::endl;
	std::cout << "0.\tExit program" << std::endl;
	std::cout << std::endl << std::endl;
	std::cout << "Enter option:";
}

void update_image_and_window(Image** image, Window** window);
void create_window(Image** image, Window** window);
void create_image(Image** image, Window** window);
void manage_image_layers(Image** image);
void manage_image_selections(Image** image);
void perform_operation(Image** image);
void perform_composite_operation(Image** image);
void save_image(Image** image);
void save_project(Image** image);
void load_existing_project(Image** image);
void argc_mode(char** argv);

int main(int argc, char **argv) {

	
	Window* window = nullptr;
	Image* image = nullptr;
	int option;

	try {
		if (argc == 3) argc_mode(argv);
	}
	catch (Exception& e) {
		std::cout << e.what() << std::endl;
		exit(0);
	}

	console_output_start();

	try {
		load_existing_project(&image);
	}
	catch (Exception& e) {
		std::cout << e.what() << std::endl;
		exit(0);
	}

	while (true) {
		try {
			update_image_and_window(&image, &window);
			constole_output();
			std::cin >> option;

			switch (option) {
			case 1:
				create_window(&image, &window);
				break;

			case 2:
				create_image(&image, &window);
				break;

			case 3:
				manage_image_layers(&image);
				break;

			case 4:
				manage_image_selections(&image);
				break;

			case 5:
				perform_operation(&image);
				break;

			case 6:
				perform_composite_operation(&image);
				break;

			case 0:
				try {
					save_image(&image);
					save_project(&image);
				}
				catch (Exception& e) {
					std::cout << e.what() << std::endl;
					exit(0);
				}
				delete window;
				delete image;
				std::cout << "Terminating the program..." << std::endl;
				exit(0);
				break;

			default:
				break;
			}
			update_image_and_window(&image, &window);
		}
		catch (Exception& e) {
			std::cout << e.what() << std::endl;
		}
	}
	return 0;
}
