#ifndef OPERATION_H
#define OPERATION_H

#include <string>
#include <map>
#include <functional>
#include "selection.h"
#include "pixel.h"

class Operation {
public:
	Operation(const std::string &opcode, int operand);
	virtual void execute(std::vector<Pixel> &vec);
	void median(Pixel& p, std::vector<Pixel>& temp, int i, int j, int w, int h);
	std::string get_operation_code() const { return opcode_; }

protected:
	Operation() : operand_(0), opcode_("") {}
	static std::map<std::string, std::function<void(Pixel&, int)>> function_map_;
	std::string opcode_;
	int operand_;

	friend class Formatter;
};

void add_(Pixel& p, int k);
void sub_(Pixel& p, int k);
void invsub_(Pixel& p, int k);
void mul_(Pixel& p, int k);
void div_(Pixel& p, int k);
void invdiv_(Pixel& p, int k);
void pow_(Pixel& p, int k);
void log_(Pixel& p, int k);
void abs_(Pixel& p, int k);
void min_(Pixel& p, int k);
void max_(Pixel& p, int k);
void inverse_(Pixel& p, int k = 0);
void grayscale_(Pixel& p, int k= 0);
void blackandwhite_(Pixel& p, int k = 0);
void opacity_(Pixel& p, int k);
void fill_red_(Pixel& p, int k);
void fill_green_(Pixel& p, int k);
void fill_blue_(Pixel& p, int k);

#endif