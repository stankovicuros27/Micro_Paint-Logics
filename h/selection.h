#ifndef SELECTION_H
#define SELECTION_H
#include <SDL.h>
#include <string>
#include <queue>

class Image;
class Operation;

class Selection {
public:

	int get_id() const { return id_; }
	bool is_active() const { return active_; }
	void set_active(bool active) { active_ = active; }
	SDL_Rect get_rect() { return rect_; }

private:
	Selection(int x, int y, int w, int h, Image* image);
	void perform_operation_whole_selection(Operation& operation);

	friend std::ostream& operator<<(std::ostream& os, const Selection& selection);
	
private:
	SDL_Rect rect_;
	Image* my_image_;

	int id_ = global_selection_id++;

	bool active_ = true;
	static int global_selection_id;
	friend class Image;
};

#endif
