#include "operation.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Operation::Operation(const std::string &opcode, int operand) : opcode_(opcode), operand_(operand) {}

void add_(Pixel& p, int k) {
	p.r_ += k;
	p.g_ += k;
	p.b_ += k;
}

void sub_(Pixel& p, int k) {
	p.r_ -= k;
	p.g_ -= k;
	p.b_ -= k;
}

void invsub_(Pixel& p, int k) {
	p.r_ = k - p.r_;
	p.g_ = k - p.g_;
	p.b_ = k - p.b_;
}

void mul_(Pixel& p, int k) {
	p.r_ *= k;
	p.g_ *= k;
	p.b_ *= k;
}

void div_(Pixel& p, int k) {
	p.r_ /= k;
	p.g_ /= k;
	p.b_ /= k;
}

void invdiv_(Pixel& p, int k) {
	p.r_ = k / p.r_;
	p.g_ = k / p.g_;
	p.b_ = k / p.b_;
}

void pow_(Pixel& p, int k) {
	p.r_ = pow(p.r_, k);
	p.g_ = pow(p.g_, k);
	p.b_ = pow(p.b_, k);
}

void log_(Pixel& p, int k) {
	p.r_ = log(p.r_);
	p.g_ = log(p.g_);
	p.b_ = log(p.b_);
}

void abs_(Pixel& p, int k) {
	p.r_ = abs(p.r_);
	p.g_ = abs(p.g_);
	p.b_ = abs(p.b_);
}

void min_(Pixel& p, int k) {
	p.r_ = std::min(p.r_, k);
	p.g_ = std::min(p.g_, k);
	p.b_ = std::min(p.b_, k);
}

void max_(Pixel& p, int k) {
	p.r_ = std::max(p.r_, k);
	p.g_ = std::max(p.g_, k);
	p.b_ = std::max(p.b_, k);
}

void inverse_(Pixel& p, int k) {
	p.r_ = 255 - p.r_;
	p.g_ = 255 - p.g_;
	p.b_ = 255 - p.b_;
}

void grayscale_(Pixel& p, int k) {
	int avg = (p.r_ + p.g_ + p.b_) / 3;
	p.r_ = p.g_ = p.b_ = avg;
}

void blackandwhite_(Pixel& p, int k) {
	int avg = (p.r_ + p.g_ + p.b_) / 3;
	avg = avg < 127 ? 0 : 255;
	p.r_ = p.g_ = p.b_ = avg;
}

void fill_red_(Pixel& p, int k) {
	p.r_ = k;
}

void fill_green_(Pixel& p, int k) {
	p.g_ = k;
}

void fill_blue_(Pixel& p, int k) {
	p.b_ = k;
}

void opacity_(Pixel& p, int k) {
	p.a_ = p.a_ * k / 100;
}

std::map<std::string, std::function<void(Pixel&, int)>> Operation::function_map_ =
{ 
	{"add", add_}, {"sub", sub_}, {"invsub", invsub_}, {"mul", mul_}, {"div", div_}, 
	{"invdiv", invdiv_},  {"pow", pow_}, {"log", log_}, {"abs", abs_}, 
	{"min", min_}, {"max", max_}, {"inverse", inverse_}, {"grayscale", grayscale_}, 
	{"blackandwhite", blackandwhite_}, {"fillred", fill_red_}, {"fillgreen", fill_green_},
	{"fillblue", fill_blue_}, {"opacity", opacity_}
};


void Operation::execute(std::vector<Pixel> &vec) {
	if (function_map_.find(opcode_) == function_map_.end()) {
		std::cerr << "Invalid operation code! Terminating operation..." << std::endl;
		return;
	}
	for_each(vec.begin(), vec.end(), [&](Pixel& p) {
		function_map_[opcode_](p, operand_);
	});
}


void Operation::median(Pixel &p, std::vector<Pixel>& temp, int i, int j, int w, int h) {
	std::vector<int> vec;
	vec.push_back(temp[(i - 1) * w + j].r_);
	vec.push_back(temp[(i) * w + j + 1].r_);
	vec.push_back(temp[(i + 1) * w + j].r_);
	vec.push_back(temp[(i) * w + j - 1].r_);
	vec.push_back(temp[(i) * w + j].r_);
	std::sort(vec.begin(), vec.end());
	p.r_ = vec[2];
	vec.clear();


	vec.push_back(temp[(i - 1) * w + j].g_);
	vec.push_back(temp[(i)*w + j + 1].g_);
	vec.push_back(temp[(i + 1) * w + j].g_);
	vec.push_back(temp[(i)*w + j - 1].g_);
	vec.push_back(temp[(i)*w + j].g_);
	std::sort(vec.begin(), vec.end());
	p.g_ = vec[2];
	vec.clear();


	vec.push_back(temp[(i - 1) * w + j].b_);
	vec.push_back(temp[(i)*w + j + 1].b_);
	vec.push_back(temp[(i + 1) * w + j].b_);
	vec.push_back(temp[(i)*w + j - 1].b_);
	vec.push_back(temp[(i)*w + j].b_);
	std::sort(vec.begin(), vec.end());
	p.b_ = vec[2];
	vec.clear();
}


