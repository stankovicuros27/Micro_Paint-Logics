#include "compoperation.h"
#include "exception.h"
#include <iostream>

CompositeOperation::CompositeOperation(const std::queue<std::pair<std::string, int>> &opqueue) : my_queue_(opqueue) {}

void CompositeOperation::execute(std::vector<Pixel>& vec) {
	while (!my_queue_.empty()) {
		std::pair<std::string, int> pair = my_queue_.front();
		my_queue_.pop();
		if (function_map_.find(pair.first) == function_map_.end()) {
			std::cerr << "Invalid operation code! Terminating operation..." << std::endl;
			throw Exception("Invalid operation code for composite operation!");
		}
		for_each(vec.begin(), vec.end(), [&](Pixel& p) {
			function_map_[pair.first](p, pair.second);
		});
	}
}