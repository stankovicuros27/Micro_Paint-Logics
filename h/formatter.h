#ifndef FORMATTER_H
#define FORMATTER_H

#include <SDL.h>
#include <string>
#include "tinyxml2.h"
#include "compoperation.h"

class Formatter {
public:
	static void import_bmp(SDL_Surface** surface, std::string path);
	static bool export_bmp(SDL_Surface* surface, std::string path);

	static void import_pam(SDL_Surface** surface, std::string path);
	static bool export_pam(SDL_Surface* surface, std::string path);

	static void export_composite_operation_xml(const CompositeOperation& cop, const std::string &filename);
	static CompositeOperation import_composite_operation_xml(const std::string& filename);

	static void export_project(Image* image, const std::string &path, std::string dir);
	static Image* import_project(std::string filename);
};

#endif
