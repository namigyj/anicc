#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#pragma once

using namespace cv;
using namespace std;

Vector<Vec3b> get_colors(Mat src) {
	Vector<Vec3b> clr_plt = Vector<Vec3b>(); // color_palette
	int max_x = src.cols;
	int max_y = src.rows;

	int color_count = 0;
	bool found;

	// iterate through image
	for (int y = 100; y < max_y; y++) {
		for (int x = 100; x < max_x; x++) {
			found = false;
			// get the color @ (x,y)
			Vec3b color = src.at<Vec3b>(Point(x, y));

			for each (Vec3b c in clr_plt) {
				// if similar color's already in the clr_plt we quit the loop
				if (norm(c, color) < 25) {
					found = true;
					break;
				}
			}
			if (found == false) {
				//cout << "\n" << color_count++ << ": new color " << color << "@" << x << "x" << y << endl;
				cout << "\r" << clr_plt.size();
				clr_plt.push_back(color);
				
			}

		}
	}
	return clr_plt;
}

Vec3b avg_square(Mat src, int x, int y, int rad) {
	int s = rad * 2; // size of side
	x = x - rad;
	y = y - rad;
	int div = 0; // divider of average
	Vec3d dresult(0, 0, 0);
	for (y; y < y + s; y++) {
		for (x; x < x + s; x++) {
			dresult += src.at<Vec3d>(Point(x, y));
			div++;
		}
	}
	dresult[0] = dresult[0] / div;
	dresult[1] = dresult[1] / div;
	dresult[2] = dresult[2] / div;
	
	return (Vec3b)dresult;
}

Vec3b avg_circle(Mat src, int px, int py, int r) {
	// P = {(x,y) : (x-px)² + (y-py)² <= r²}
	
	int div = 0; // divider of average
	Vec3d dresult(0, 0, 0);
	int y = py-r;
	int x = px-r;
	for (y; y < py + r; y++) {
		for (x; x < px + r; x++) {
			double dx = x - px;
			double dy = y - py;
			double dsquared = (dx * dx) + (dy*dy);
			
			// if inside the circle
			if (dsquared <= (r*r) && x >= 0 && x <= src.cols && y >= 0 && y <= src.rows) {
				//cout << src.at<Vec3b>(Point(x, y));
				dresult = dresult + (Vec3d)src.at<Vec3b>(Point(x, y));
				div++;
			}
		}
	}
	
	return (Vec3b)(dresult/div);
}

int main() {
	Mat src = imread("C:/Users/Touko/code/paysage1.jpg");
	if (src.empty()) {
		cout << "Cannot load img !" << endl;
		return -1;
	}
	cout << "loaded img..." << endl;

	Mat ref(src.rows, src.cols, CV_8UC3, Scalar(237, 238, 243));
	Mat img = ref.clone();

	cout << "generating color palette... " << endl;
	Vector<Vec3b> color_pal = get_colors(src);
	cout << " colors" << endl;


	cout << "generating" << endl;
	for (int i=0; i < 1000000; i++) {
		int x = rand() % img.cols;
		int y = rand() % img.rows;
		int rad = 2;
		// void circle(Mat& img, Point center, int radius, const Scalar& color, int thickness = 1, int lineType = 8, int shift = 0)
		Vec3b color = color_pal[rand()%color_pal.size()];
		Vec3b src_avg = avg_circle(src, x, y, rad);
		Vec3b img_avg = avg_circle(img, x, y, rad);
		//cout << src_avg << " - " << color << "vs" << img_avg << endl;
		
		if (norm(color, src_avg) < norm(img_avg, src_avg))
			circle(img, Point(x, y), rad, (Scalar)color, CV_FILLED);
		
		
		if (i % 500 == 0) {
			cout << i << endl;
			imshow("Image", img);
			char eKey = waitKey(1) & 0xFF;
		}
		
	}
	//imshow("Image", src);
	imshow("Image", img);
	char eKey = waitKey(0);
	//clr_plt.~Vector();
	
}

/* color examples :
		(110, 73, 62)
		(80, 51, 44)
		(168, 112, 97)
		(55, 44, 42)
		(229, 228, 255)
		(161, 145, 153)
		(33, 61, 121)
		(154, 150, 209)
		(34, 21, 19)
		(124, 121, 168)
		(107, 79, 82)
		(176, 116, 110)
		(153, 94, 90)
		(70, 35, 39)
		(55, 19, 32)
*/