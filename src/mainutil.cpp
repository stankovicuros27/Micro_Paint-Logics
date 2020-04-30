#include <iostream>
#include <string>
#include "sdl.h"
#include "window.h"
#include "formatter.h"
#include "pixel.h"
#include "SDL_thread.h"
#include <queue>
#include <vector>
#include <algorithm>
#include <regex>
#include <iostream>
#include "compoperation.h"


bool is_integer(const std::string& s) {
	return std::regex_match(s, std::regex("[(-|+)|][0-9]+"));
}

void update_image_and_window(Image** image, Window **window) {
	if ((*image) && (*window)) SDL_SetWindowSize((*window)->get_window(), (*image)->get_surface()->w, (*image)->get_surface()->h);
	if ((*window)) {
		(*window)->clear_screen();
		(*window)->update_screen();
		(*window)->poll_events();
	}
}

void create_window(Image **image, Window** window) {
	if ((*window)) {
		std::cout << "Window already created!" << std::endl;
		return;
	}
	(*window) = new Window("Micro_Paint", 100, 100);
	SDL_SetWindowPosition((*window)->get_window(), 0, 33);
	if ((*image)) (*window)->set_image((*image));
	if ((*image) && (*window)) SDL_SetWindowSize((*window)->get_window(), (*image)->get_surface()->w, (*image)->get_surface()->h);

	std::cout << "Window initialized!";
}

void create_image(Image** image, Window** window) {
	{
		if ((*image)) {
			std::cout << "Image already created!" << std::endl;
			return;
		}
		(*image) = new Image();
		if ((*window)) (*window)->set_image((*image));
		if ((*image) && (*window)) SDL_SetWindowSize((*window)->get_window(), (*image)->get_surface()->w, (*image)->get_surface()->h);
		std::cout << "Image initialized!";
	}
}

void manage_image_layers(Image** image) {

	if (!(*image)) {
		std::cout << "Image not initialized!" << std::endl;
		return;
	}

	std::cout << "All created layers:" << std::endl << std::endl;
	std::for_each((*image)->layers_.begin(), (*image)->layers_.end(), [](Layer* layer) {
		std::cout << *layer;
	});
	std::cout << std::endl;

	std::cout << "To add new Layer enter 1." << std::endl << "To enable/disable existing Layer, enter 2" 
		<< std::endl << "To remove existing Layer, enter 3" << std::endl;
	std::cout << "Enter option:";
	int choice;
	std::cin >> choice;
	while (choice != 1 && choice != 2 && choice != 3) {
		std::cout << "Wrong input!" << std::endl;
		std::cout << "To add new Layer enter 1." << std::endl << "To enable/disable existing Layer, enter 2" 
			<< std::endl << "To remove existing Layer, enter 3" << std::endl;
		std::cout << "Enter option:";
		std::cin >> choice;
	}

	//add new layer
	if (choice == 1) {
		std::cout << "To add empty layer enter 1" << std::endl << "To add layer with image enter 2" << std::endl;
		std::cout << "Enter option:";
		int choice;
		std::cin >> choice;
		while (choice != 1 && choice != 2) {
			std::cout << "Wrong input!" << std::endl;
			std::cout << "To add empty layer enter 1, to add layer with image enter 2" << std::endl;
			std::cout << "Enter option:";
			std::cin >> choice;
		}
		if (choice == 1) {
			int lay_x;
			int lay_y;
			int lay_w;
			int lay_h;
			int lay_opacity;
			std::string util;

			std::cout << "Enter layer X coordinate:";
			getchar();
			std::cin >> util;
			while (!is_integer(util)) {
				std::cout << "Wrong input! Enter integer value!" << std::endl;
				std::cout << "Enter layer X coordinate:";
				std::cin >> util;
			}
			lay_x = std::stoi(util);

			std::cout << "Enter layer Y coordinate:";
			std::cin >> util;
			while (!is_integer(util)) {
				std::cout << "Wrong input! Enter integer value!" << std::endl;
				std::cout << "Enter layer Y coordinate:";
				std::cin >> util;
			}
			lay_y = std::stoi(util);

			std::cout << "Enter layer width:";
			std::cin >> util;
			while (!is_integer(util)) {
				std::cout << "Wrong input! Enter integer value!" << std::endl;
				std::cout << "Enter layer  width:";
				std::cin >> util;
			}
			lay_w = std::stoi(util);

			std::cout << "Enter layer height:";
			std::cin >> util;
			while (!is_integer(util)) {
				std::cout << "Wrong input! Enter integer value!" << std::endl;
				std::cout << "Enter layer  height:";
				std::cin >> util;
			}
			lay_h = std::stoi(util);

			std::cout << "Enter layer opacity:";
			std::cin >> util;
			lay_opacity = std::stoi(util);

			(*image)->add_layer(lay_x, lay_y, lay_w, lay_h, lay_opacity);
		}


		else if (choice == 2) {
			int lay_x;
			int lay_y;
			int lay_opacity;
			std::string path;
			std::string util;

			std::cout << "Enter layer X coordinate:";
			getchar();
			std::cin >> util;
			while (!is_integer(util)) {
				std::cout << "Wrong input! Enter integer value!" << std::endl;
				std::cout << "Enter layer X coordinate:";
				std::cin >> util;
			}
			lay_x = std::stoi(util);

			std::cout << "Enter layer Y coordinate:";
			std::cin >> util;
			while (!is_integer(util)) {
				std::cout << "Wrong input! Enter integer value!" << std::endl;
				std::cout << "Enter layer Y coordinate:";
				std::cin >> util;
			}
			lay_y = std::stoi(util);

			std::cout << "Enter relative image path:";
			std::cin >> path;

			while (!(path.substr(path.find_last_of(".") + 1) == "bmp" || path.substr(path.find_last_of(".") + 1) == "pam")) {
				std::cout << "Incorrect format! You need to enter .bmp or .pam file!" << std::endl;
				std::cout << "Enter relative image path:";
				std::cin >> path;
			}

			std::cout << "Enter layer opacity:";
			std::cin >> util;
			lay_opacity = std::stoi(util);

			(*image)->add_layer(lay_x, lay_y, path, lay_opacity);
		}
	}

	//manage existing layer
	else if (choice == 2) {
		std::cout << "To enable Layer enter 1" << std::endl << "To disable Layer enter 2" << std::endl;
		std::cout << "Enter option:";
		int choice;
		std::cin >> choice;
		while (choice != 1 && choice != 2) {
			std::cout << "Wrong input!" << std::endl;
			std::cout << "To enable Layer enter 1" << std::endl << "To disable Layer enter 2" << std::endl;
			std::cout << "Enter option:";
			std::cin >> choice;
		}

		if (choice == 1) {
			int choice;
			std::cout << "Enter Layer ID of the Layer to be enabled:";
			std::cin >> choice;
			Layer* temp = (*image)->find_layer_by_id(choice);
			if (temp != nullptr) {
				temp->set_active(true);
				std::cout << "Layer state set to Active!" << std::endl;
			}
		}
		else if (choice == 2) {
			int choice;
			std::cout << "Enter Layer ID of the Layer to be disabled:";
			std::cin >> choice;
			Layer* temp = (*image)->find_layer_by_id(choice);
			if (temp != nullptr) {
				temp->set_active(false);
				std::cout << "Layer state set to Disabled!" << std::endl;
			}
		}
	}

	//remove layer
	else if (choice == 3) {
		int choice;
		std::cout << "Enter Layer ID of the Layer to be removed:";
		std::cin >> choice;
		(*image)->remove_layer(choice);
	}
}

void manage_image_selections(Image** image) {
	if (!(*image)) {
		std::cout << "Image not initialized!" << std::endl;
		return;
	}
	std::cout << "All created selections:" << std::endl << std::endl;
	std::for_each((*image)->selections_.begin(), (*image)->selections_.end(), [](Selection* selection) {
		std::cout << *selection;
	});
	std::cout << std::endl;

	std::cout << "To add new Selection, enter 1." << std::endl << "To endable/disable existing Selection, enter 2" 
		<< std::endl << "To remove existing Selection, enter 3" << std::endl;
	std::cout << "Enter option:";
	int choice;
	std::cin >> choice;
	while (choice != 1 && choice != 2 && choice != 3) {
		std::cout << "Wrong input!" << std::endl;
		std::cout << "To add new Selection, enter 1." << std::endl << "To endable/disable existing Selection, enter 2"
			<< std::endl << "To remove existing Selection, enter 3" << std::endl;
		std::cout << "Enter option:";
		std::cin >> choice;
	}

	if (choice == 1) {
		int sel_x;
		int sel_y;
		int sel_w;
		int sel_h;
		std::string util;

		std::cout << "Enter Selection X coordinate:";
		getchar();
		std::cin >> util;
		while (!is_integer(util)) {
			std::cout << "Wrong input! Enter integer value!" << std::endl;
			std::cout << "Enter Selection X coordinate:";
			std::cin >> util;
		}
		sel_x = std::stoi(util);

		std::cout << "Enter Selection Y coordinate:";
		std::cin >> util;
		while (!is_integer(util)) {
			std::cout << "Wrong input! Enter integer value!" << std::endl;
			std::cout << "Enter Selection Y coordinate:";
			std::cin >> util;
		}
		sel_y = std::stoi(util);

		std::cout << "Enter Selection width:";
		std::cin >> util;
		while (!is_integer(util)) {
			std::cout << "Wrong input! Enter integer value!" << std::endl;
			std::cout << "Enter Selection width:";
			std::cin >> util;
		}
		sel_w = std::stoi(util);

		std::cout << "Enter Selection height:";
		std::cin >> util;
		while (!is_integer(util)) {
			std::cout << "Wrong input! Enter integer value!" << std::endl;
			std::cout << "Enter Selection height:";
			std::cin >> util;
		}
		sel_h = std::stoi(util);

		(*image)->add_selection(sel_x, sel_y, sel_w, sel_h);
	}

	else if (choice == 2) {
		std::cout << "To enable Selection enter 1" << std::endl << "To disable Selection enter 2" << std::endl;
		std::cout << "Enter option:";
		int choice;
		std::cin >> choice;
		while (choice != 1 && choice != 2) {
			std::cout << "Wrong input!" << std::endl;
			std::cout << "To enable Selection enter 1" << std::endl << "To disable Selection enter 2" << std::endl;
			std::cout << "Enter option:";
			std::cin >> choice;
		}

		if (choice == 1) {
			int choice;
			std::cout << "Enter Selection ID of the Selection to be enabled:";
			std::cin >> choice;
			Selection* temp = (*image)->find_selection_by_id(choice);
			if (temp != nullptr) {
				temp->set_active(true);
				std::cout << "Selection state set to Active!" << std::endl;
			}
		}
		else if (choice == 2) {
			int choice;
			std::cout << "Enter Selection ID of the Selection to be disabled:";
			std::cin >> choice;
			Selection* temp = (*image)->find_selection_by_id(choice);
			if (temp != nullptr) {
				temp->set_active(false);
				std::cout << "Selection state set to Disabled!" << std::endl;
			}
		}
	}

	else if (choice == 3) {
		int choice;
		std::cout << "Enter Selection ID of the Selection to be removed:";
		std::cin >> choice;
		(*image)->remove_selection(choice);
	}
}

void perform_operation(Image** image) {
	if (!(*image)) {
		std::cout << "Image not initialized!" << std::endl;
		return;
	}
	std::string opcode;
	int k;
	std::cout << "Enter operation and operand value:";
	std::cin >> opcode;
	std::cin >> k;
	std::cout << std::endl << "Your operation is being executed..." << std::endl;
	Operation operation(opcode, k);
	(*image)->perform_operation(operation);
}

void perform_composite_operation(Image** image) {
	if (!(*image)) {
		std::cout << "Image not initialized!" << std::endl;
		return;
	}

	std::cout << "To load existing operation enter 1" << std::endl << "To make new operation enter 2" << std::endl;
	std::cout << "Enter option:";
	int choice;
	std::cin >> choice;
	while (choice != 1 && choice != 2) {
		std::cout << "Wrong input!" << std::endl;
		std::cout << "To load existing operation enter 1" << std::endl << "To make new operation enter 2" << std::endl;
		std::cout << "Enter option:";
		std::cin >> choice;
	}

	if (choice == 1) {
		std::cout << "Enter relative file path (for your operation):";
		std::string path;
		std::cin >> path;

		while (!(path.substr(path.find_last_of(".") + 1) == "fun")) {
			std::cout << "Incorrect format! You need to enter .fun extension file!" << std::endl;
			std::cout << "Enter relative file path:";
			std::cin >> path;
		}

		CompositeOperation operation(Formatter::import_composite_operation_xml(path));
		std::cout << std::endl << "Your operation is being executed..." << std::endl;
		(*image)->perform_operation(operation);
	} 

	else if (choice == 2) {
		std::queue<std::pair<std::string, int>> queue;
		std::cout << "Enter number of operations in composite operation:";
		int n;
		std::cin >> n;
		std::string opcode;
		int k;
		for (int i = 0; i < n; i++) {
			std::cout << "Enter " << i + 1 << ". operation and operand:";
			std::cin >> opcode;
			std::cin >> k;
			std::pair<std::string, int> pair = std::make_pair(opcode, k);
			queue.push(pair);
		}
		CompositeOperation operation(queue);

		std::cout << "If you want to save your Composite Operation, enter 1" << std::endl << "If you dont want to, enter any other symbol" << std::endl;
		std::cout << "Enter option:";
		int choice;
		std::cin >> choice;

		if (choice == 1) {
			getchar();
			std::cout << "Enter FileName for your operation (extension should be .fun):";
			std::string file_name;
			std::cin >> file_name;
			Formatter::export_composite_operation_xml(operation, file_name);
			std::cout << "Composite Operation exported!" << std::endl;
		}

		std::cout << std::endl << "Your operation is being executed..." << std::endl;
		(*image)->perform_operation(operation);
	}
}

void save_image(Image** image) {
	int choice;
	std::cout << "Enter 1 if you want to save your Image" << std::endl << "Enter any other number if you don't want to" << std::endl;
	std::cout << "Enter option:";
	std::cin >> choice;
	if (choice == 1) { 
		std::cout << "Enter File Name for your image (extension must be .fun or .pam):";
		std::string file_name;
		std::cin >> file_name;
		if (file_name.substr(file_name.find_last_of(".") + 1) == "pam") {
			Formatter::export_pam((*image)->get_surface(), file_name);
		}
		else if (file_name.substr(file_name.find_last_of(".") + 1) == "bmp") {
			Formatter::export_bmp((*image)->get_surface(), file_name);
		}
		else std::cout << "Unsupported file extension!" << std::endl; 
	}
}

void save_project(Image** image) {
	int choice;
	std::cout << "Enter 1 if you want to save your Project" << std::endl << "Enter any other number if you don't want to" << std::endl;
	std::cout << "Enter option:";
	std::cin >> choice;
	if (choice == 1) {
		std::cout << "Enter File Name for your Project (extension must be .xml, projects are saved in res/ folder):";
		std::string file_name;
		std::cin >> file_name;
		if (file_name.substr(file_name.find_last_of(".") + 1) == "xml") {
			Formatter::export_project((*image), file_name);
		}
		else std::cout << "Unsupported file extension!" << std::endl;
	}
}

void load_existing_project(Image** image) {
	std::cout << "\n\nTo load existing project enter 1" << std::endl;
	std::cout << "To make new project, enter any other value" << std::endl;
	std::cout << "Enter option:";
	int choice;
	std::cin >> choice;
	if (choice == 1) {
		std::cout << "Enter file to read from(extension should be .xml):";
		std::string project;
		std::cin >> project;
		*image = Formatter::import_project(project);
		std::cout << "Project successfully loaded!" << std::endl;
	}
}


void argc_mode(char** argv) {
	Image image;
	std::string img;
	img.append(argv[1]);
	image.add_layer(0, 0, img, 100);

	std::string oper;
	oper.append(argv[2]);

	if (oper == "median") {
		Operation op("median", 0);
		image.perform_operation(op);
	}
	else{
		CompositeOperation operation(Formatter::import_composite_operation_xml(oper));
		image.perform_operation(operation);
	}


	std::string file_name = img;

	if (file_name.substr(file_name.find_last_of(".") + 1) == "pam") {
		Formatter::export_pam(image.get_surface(), file_name);
	}
	else if (file_name.substr(file_name.find_last_of(".") + 1) == "bmp") {
		Formatter::export_bmp(image.get_surface(), file_name);
	}
	else std::cout << "Unsupported file extension!" << std::endl;

	exit(0);
}