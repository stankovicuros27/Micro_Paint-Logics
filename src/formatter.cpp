#include "formatter.h"
#include "image.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <io.h>
#include <direct.h>
#include <Windows.h>
#include "selection.h"
#include <fstream>
#include <regex>
#include "exception.h"

using namespace tinyxml2;

#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult); return a_eResult; }


void Formatter::import_bmp(SDL_Surface** surface, std::string path) {
	*surface = SDL_LoadBMP(path.c_str());
	if ((*surface) == nullptr) {
		std::cerr << "BMP Image could not be imported! SDL_Error: " << SDL_GetError() << std::endl;
		exit(-1);
		throw Exception("Invalid BMP File!");
	}
	SDL_SetColorKey(*surface, SDL_TRUE, SDL_PIXELFORMAT_RGBA32);
}

bool Formatter::export_bmp(SDL_Surface* surface, std::string path) {
	if (SDL_SaveBMP(surface, path.c_str()) < 0)
	{
		std::cout << "BMP Image could not be exported! SDL_Error: " << SDL_GetError() << std::endl;
		exit(-2);
		throw Exception("Invalid BMP export path!");
	}
	return true;
}

void Formatter::export_composite_operation_xml(const CompositeOperation& cop, const std::string &filename) {
	tinyxml2::XMLDocument xml_doc;
	XMLNode* root = xml_doc.NewElement("Root");
	xml_doc.InsertFirstChild(root);

	XMLElement* comp_op = xml_doc.NewElement("Composite_Operation");
	comp_op->SetAttribute("Operation_Count", cop.my_queue_.size());

	std::queue<std::pair<std::string, int>> temp_q(cop.my_queue_);

	while (!temp_q.empty()) {
		std::pair<std::string, int> pair = temp_q.front();
		temp_q.pop();

		XMLElement* op = xml_doc.NewElement("Operation");
		
		XMLElement* opcode = xml_doc.NewElement("Operation_Code");
		opcode->SetText(pair.first.c_str());
		op->InsertEndChild(opcode);

		XMLElement* operand = xml_doc.NewElement("Operand");
		operand->SetText(pair.second);
		op->InsertEndChild(operand);

		comp_op->InsertEndChild(op);
	}

	root->InsertEndChild(comp_op);
	
	XMLError eResult = xml_doc.SaveFile(filename.c_str());
	if (eResult != XML_SUCCESS) throw Exception("Cannot create XML File!");
}

CompositeOperation Formatter::import_composite_operation_xml(const std::string& filename) {
	tinyxml2::XMLDocument xml_doc;
	XMLError eResult = xml_doc.LoadFile(filename.c_str());
	if (eResult != XML_SUCCESS) {
		exit(-3);
		throw Exception("Cannot open XML File!");
	}

	XMLNode* root = xml_doc.FirstChild();
	if (root == nullptr) { 
		exit(-4);
		throw Exception("XML File Corrupted!"); 
	}

	XMLElement* comp_op = root->FirstChildElement("Composite_Operation");
	if (comp_op == nullptr) { 
		exit(-5);
		throw Exception("XML File Corrupted!"); 
	}

	int Operation_Count;
	eResult = comp_op->QueryIntAttribute("Operation_Count", &Operation_Count);
	if (eResult != XML_SUCCESS) {
		exit(-6);
		throw Exception("XML File Corrupted!"); 
	}

	std::queue<std::pair<std::string, int>> queue;

	XMLElement* op = comp_op->FirstChildElement("Operation");

	while (op != nullptr) {

		std::string opcode;
		XMLElement* Operation_Code = op->FirstChildElement("Operation_Code");
		opcode = Operation_Code->GetText();

		int oper;
		XMLElement* Operand = op->FirstChildElement("Operand");
		eResult = Operand->QueryIntText(&oper);
		if (eResult != XML_SUCCESS) {
			exit(-7);
			throw Exception("XML File Corrupted!");
		}

		queue.push(std::make_pair(opcode, oper));

		op = op->NextSiblingElement("Operation");
	}
	return CompositeOperation(queue);
}

void Formatter::export_project(Image* image, const std::string& path, std::string dir = "res/") {

	if (path.substr(path.find_last_of(".") + 1) != "xml") {
		std::cout << "Wrong file format!" << std::endl;
		return;
	}

	tinyxml2::XMLDocument xml_doc;
	XMLNode* root = xml_doc.NewElement("Root");
	xml_doc.InsertFirstChild(root);

	XMLElement* layer_list = xml_doc.NewElement("Layer_List");
	layer_list->SetAttribute("Layer_Count", image->layers_.size());

	std::for_each(image->layers_.begin(), image->layers_.end(), [&](Layer* lay) {
		std::string layid = "/layer" + std::to_string(lay->get_id()) + ".bmp";
		std::string layerfile = dir + layid;

		Formatter::export_bmp(lay->get_surface(), layerfile);

		XMLElement* layer = xml_doc.NewElement("Layer");
		layer->SetAttribute("Path", layerfile.c_str());
		layer->SetAttribute("x", 0);
		layer->SetAttribute("y", 0);
		layer_list->InsertEndChild(layer);
	});

	root->InsertEndChild(layer_list);

	XMLElement* selection_list = xml_doc.NewElement("Selection_List");
	selection_list->SetAttribute("Selection_Count", image->selections_.size());

	std::for_each(image->selections_.begin(), image->selections_.end(), [&](Selection* sel) {
		XMLElement* selection = xml_doc.NewElement("Selection");
		selection->SetAttribute("x", sel->get_rect().x);
		selection->SetAttribute("y", sel->get_rect().y);
		selection->SetAttribute("w", sel->get_rect().w);
		selection->SetAttribute("h", sel->get_rect().h);
		selection_list->InsertEndChild(selection);
	});

	root->InsertEndChild(selection_list);

	std::string project = dir + "/" + path;
	XMLError eResult = xml_doc.SaveFile(project.c_str());
	if (eResult != XML_SUCCESS) { exit(-9); throw Exception("Couldn't create XML File!"); }
}

Image* Formatter::import_project(std::string filename) {
	tinyxml2::XMLDocument xml_doc;
	XMLError eResult = xml_doc.LoadFile(filename.c_str());
	if (eResult != XML_SUCCESS) throw Exception("Couldn't load XML File!");

	Image* image = new Image();


	XMLNode* root = xml_doc.FirstChild();
	if (root == nullptr) throw Exception("XML File corrupted!");

	XMLElement* layer_list = root->FirstChildElement("Layer_List");
	if (layer_list == nullptr) throw Exception("XML File corrupted!");

	int layer_cnt;
	eResult = layer_list->QueryIntAttribute("Layer_Count", &layer_cnt);
	if (eResult != XML_SUCCESS) throw Exception("XML File corrupted!");

	XMLElement* layer = layer_list->FirstChildElement("Layer");

	while (layer != nullptr) {
		std::string imgpath;
		imgpath = layer->Attribute("Path");
		int x = atoi(layer->Attribute("x"));
		int y = atoi(layer->Attribute("y"));
		image->add_layer(x, y, imgpath, 100);
		layer = layer->NextSiblingElement("Layer");
	}

	XMLElement* selection_list = root->FirstChildElement("Selection_List");
	if (selection_list == nullptr) throw Exception("XML File corrupted!");

	int selection_cnt;
	eResult = selection_list->QueryIntAttribute("Selection_Count", &selection_cnt);
	if (eResult != XML_SUCCESS) throw Exception("XML File corrupted!");

	XMLElement* selection = selection_list->FirstChildElement("Selection");

	while (selection != nullptr) {
		int x, y, w, h;
		selection->QueryIntAttribute("x", &x);
		selection->QueryIntAttribute("y", &y);
		selection->QueryIntAttribute("w", &w);
		selection->QueryIntAttribute("h", &h);
		image->add_selection(x, y, w, h);
		selection = selection->NextSiblingElement("Selection");
	}

	return image;
}

void Formatter::import_pam(SDL_Surface** surface, std::string path) {
	std::ifstream file(path.c_str(), std::ios::binary);

	if (!file)
	{
		std::cout << "PAM Image could not be imported!" << std::endl;
		throw Exception("Bad PAM file path!");
	}

	std::string temp;
	
	getline(file, temp);
	if (temp != "P7") {
		std::cout << "PAM file is not valid!" << std::endl;
		file.close();
		throw Exception("Bad PAM file!");
	}

	int w, h, depth, maxval;
	std::string tupltype;

	std::regex rx_num("([^0-9]*)([0-9]*)");
	std::smatch check;

	//width
	getline(file, temp);
	if (std::regex_match(temp, check, rx_num)) {
		w = atoi(check.str(2).c_str());
	}
	else {
		std::cout << "PAM file is not valid!" << std::endl;
		file.close();
		throw Exception("Bad PAM file!");
	}

	//height
	getline(file, temp);
	if (std::regex_match(temp, check, rx_num)) {
		h = atoi(check.str(2).c_str());
	}
	else {
		std::cout << "PAM file is not valid!" << std::endl;
		file.close();
		throw Exception("Bad PAM file!");
	}

	//depth
	getline(file, temp);
	if (std::regex_match(temp, check, rx_num)) {
		depth = atoi(check.str(2).c_str());
	}
	else {
		std::cout << "PAM file is not valid!" << std::endl;
		file.close();
		throw Exception("Bad PAM file!");
	}

	//maxval
	getline(file, temp);
	if (std::regex_match(temp, check, rx_num)) {
		maxval = atoi(check.str(2).c_str());
	}
	else {
		std::cout << "PAM file is not valid!" << std::endl;
		file.close();
		throw Exception("Bad PAM file!");
	}

	std::regex rx_tupltype("TUPLTYPE ([A-Z]*[\u005F]*[A-Z]*)");

	//tupltype
	getline(file, temp);
	if (std::regex_match(temp, check, rx_tupltype)) {
		maxval = atoi(check.str(1).c_str());
	}
	else {
		std::cout << "PAM file is not valid!" << std::endl;
		file.close();
		throw Exception("Bad PAM file!");
	}

	getline(file, temp);

	std::vector<Pixel> pixels;

	unsigned char tempch[4];

	if (depth == 3) {
		for (long i = 0; i < w * h; i++) {
			file.read((char*)&tempch, 3);
			tempch[3] = 0xff;
			Uint32 pixVal = *(Uint32*)tempch;
			pixels.push_back(Pixel(&pixVal));
		}
	}
	else if (depth == 4) {
		for (long i = 0; i < w * h; i++) {
			file.read((char*)&tempch, 4);
			Uint32 pixVal = *(Uint32*)tempch;
			pixels.push_back(Pixel(&pixVal));
		}
	}
	else { std::cout << "Unsupported PAM depth!" << std::endl; throw Exception("Unsupported PAM file!");; }

	*surface = Pixel::surface_from_pixels(pixels, w, h);
	return;
}

bool Formatter::export_pam(SDL_Surface* surface, std::string path) {

	if (path.substr(path.find_last_of(".") + 1) != "pam") {
		std::cout << "Unsupported file extension!" << std::endl;
		throw Exception("Bad PAM file path!");
	}

	std::ofstream file(path.c_str(), std::ios::binary);

	if (!file)
	{
		std::cout << "Error creating file!" << std::endl;
		throw Exception("Unable to create PAM file!");
	}

	std::vector<Pixel> pixels = Pixel::pixels_from_surface(surface);

	file << "P7\n";
	file << "WIDTH " << surface->w << "\n";
	file << "HEIGHT " << surface->h << "\n";
	file << "DEPTH 4\n";
	file << "MAXVAL 255\n";
	file << "TUPLTYPE RGB_ALPHA\n";
	file << "ENDHDR\n";

	for (long i = 0; i < pixels.size(); i++) {
		file << pixels[i];
	}
	file << EOF;
	file.close();
	return true;
}