#include "CImg.h"
#include <iostream>

using namespace cimg_library;

const int IMAGE_TYPE = 1;
const int COLOR_CHANNELS_COUNT = 3;
const double ANGLE = 30.0;
const unsigned char BLACK[] = {0, 0, 0};
const unsigned char BLUE[] = {0, 0, 255};

CImg<unsigned char> GetFragment(const CImg<unsigned char>& inputed_image, int inputed_width, int inputed_height) {
	int fragment_size = inputed_width / 8;
	int x_center = inputed_width / 2;
	int y_center = inputed_height / 2;

	return inputed_image.get_crop(
		x_center - fragment_size / 2,
		y_center - fragment_size / 2,
		x_center + fragment_size / 2,
		y_center + fragment_size / 2
	).rotate(ANGLE);
}

void InsertFragmentIntoImg(const CImg<unsigned char>& fragment, CImg<unsigned char>& tmp_image, int height_new_photo){
	int frag_width = fragment.width();
	int frag_height = fragment.height();
	int x_pos = 10;
	int y_pos = height_new_photo - frag_height - 10;
	cimg_forXY(fragment, x, y) {
		if (fragment(x,y,0) != 0 || fragment(x,y,1) != 0 || fragment(x,y,2) != 0) {
			tmp_image.draw_point(x + x_pos, y + y_pos, fragment.data(x, y));
		}
	}
}

void InsertAxes(CImg<unsigned char>& tmp_image, int graph_x, int graph_y){
	const int DELTA = 10;
	const float OPACITY = 1;
	const int ARROW_SIZE = 7;

	int axis_length = tmp_image.width() > tmp_image.height() ? tmp_image.width() / 4 * 3 : tmp_image.height() / 4 * 3;

    tmp_image.draw_arrow(graph_x - axis_length, graph_y, graph_x + axis_length, graph_y, BLACK, OPACITY, ARROW_SIZE);
    tmp_image.draw_arrow(graph_x, graph_y + axis_length, graph_x, graph_y - axis_length, BLACK, OPACITY, ARROW_SIZE);

    tmp_image.draw_text(graph_x + axis_length - DELTA, graph_y + DELTA, "X", BLACK);
    tmp_image.draw_text(graph_x - DELTA, graph_y - axis_length + DELTA, "Y", BLACK);
}

double GetSizeOfGraph(CImg<unsigned char>& tmp_image){
	const double SIZE_OF_GRAPH = 200;
	return tmp_image.width() / SIZE_OF_GRAPH;
}

void InsertCosGraph(CImg<unsigned char>& tmp_image, int graph_x, int graph_y){
	const double SCALE_SIZE = 50.;

    double scale_x = SCALE_SIZE;
	double scale_y = SCALE_SIZE;
	double x_start = -M_PI * GetSizeOfGraph(tmp_image);
	double x_end = M_PI * GetSizeOfGraph(tmp_image);
	double step = 0.1;

    for (double x = x_start; x < x_end; x += step) {
        int x1 = graph_x + (int)(x * scale_x);
		int y1 = graph_y - (int)(cos(x) * scale_y);
		int x2 = graph_x + (int)((x + step) * scale_x);
		int y2 = graph_y - (int)(cos(x + step) * scale_y);
		tmp_image.draw_line(x1, y1, x2, y2, BLUE);
    }
}

int main(int argc, const char** argv) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " input_file output_file width_new_photo height_new_photo" << std::endl;
        return 1;
    }

    int width_new_photo = std::atoi(argv[3]);
    int height_new_photo = std::atoi(argv[4]);
    CImg<unsigned char> tmp_image(width_new_photo, height_new_photo, IMAGE_TYPE, COLOR_CHANNELS_COUNT, 255);

	CImg<unsigned char> inputed_image(argv[1]);
	int inputed_width = inputed_image.width();
	int inputed_height = inputed_image.height();


	CImg<unsigned char> fragment = GetFragment(inputed_image, inputed_width, inputed_height);
	InsertFragmentIntoImg(fragment, tmp_image, height_new_photo);

    int graph_x = tmp_image.width() / 4;
	int graph_y = tmp_image.height() * 0.75;

	InsertAxes(tmp_image, graph_x, graph_y);
	InsertCosGraph(tmp_image, graph_x, graph_y);

	tmp_image.save_pnm("output.pnm");
	tmp_image.display();

	return 0;
}