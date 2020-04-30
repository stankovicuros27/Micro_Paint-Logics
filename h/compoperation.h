#ifndef COMP_OPERATION
#define COMP_OPERATION
#include "operation.h"
#include <string>
#include <queue>

class CompositeOperation : public Operation {
public:
	CompositeOperation(const std::queue<std::pair<std::string, int>> &opqueue);
	virtual void execute(std::vector<Pixel>& vec);

private:
	std::queue<std::pair<std::string, int>> my_queue_;

	friend class Formatter;
};

#endif
