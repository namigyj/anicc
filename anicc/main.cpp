/*
 *	Anon's Image Comparison Challenge
 *	(or dotter ?)
 *
 * 	> get image
 *	> get colors from image
 *	> put random circle of random color on blank image1
 *  > compare with source_image
 *  > compare image 2 with source_image
 *  > keep closest image
 *	> re-iterate
 */

#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#pragma once

using namespace cv;
using namespace std;

Vector<Vec3b> get_colors(Mat src) {
	Vector<Vec3b> clr_plt = Vector<Vec3b>(); // color_palette
	int max_x = src.cols;
	int max_y = src.rows;

	bool found;

	// iterate through image
	for (int y = 100; y < max_y; y++) {
		for (int x = 100; x < max_x; x++) {
			found = false;
			// get the color @ (x,y)
			Vec3b color = src.at<Vec3b>(Point(x, y));

			for each (Vec3b c in clr_plt) {
				// if similar color is already in the 
				// color_palette we quit the loop
				if (norm(c, color) < 25) {
					found = true;
					break;
				}
			}
			if (found == false) { // if no color in clr_plt
				//cout << "\n" << color_count++ << ": new color " << color << "@" << x << "x" << y << endl;
				cout << "\r" << clr_plt.size();
				clr_plt.push_back(color);
				
			}

		}
	}
	return clr_plt;
}

Vec3b avg_square(Mat src, int px, int py, int r) {
	int s = r * 2; // size of side
	int x = px - r;
	int y = py - r;
	int div = 0; // divider of average
	Vec3d dresult(0, 0, 0);
	for (y; y < y + s; y++) {
		for (x; x < x + s; x++) {
			dresult += src.at<Vec3d>(Point(x, y));
			div++;
		}
	}

	return (Vec3b)(dresult/div);
}


// TODO
// Is there a cleaner way to do this, if circle is too big
// it averages white and black to grey,
// which doesn't give a good end result in places like eyes
Vec3b avg_circle(Mat src, int px, int py, int r) {
	// P = {(x,y) : (x-px)² + (y-py)² <= r²}
	
	int div = 0; // divider of average
	Vec3d dresult(0, 0, 0);
	
	// start positions
	// TODO
	// it's useless to start in a corner of a square around a circle
	// as the first r iterations on x (or y) won't match 
	// maybe start on (px, py-r) ?? next r iterations are as useless
	// only a gain of 2 iterations on 3px circles -_- so is it worth ?
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
		int rad = 2; // circle radius

		// get a random color from palette
		Vec3b color = color_pal[rand()%color_pal.size()];
		// get average color within circle on source image
		Vec3b src_avg = avg_circle(src, x, y, rad);
		// get average color withing circle on generate image 
		// TODO 
		// this is bloated, since I only draw circles
		// the color in the center will be the same as the others
		Vec3b img_avg = avg_circle(img, x, y, rad);
		//cout << src_avg << " - " << color << "vs" << img_avg << endl;
		
		// compare values, only draw the closest to original image
		if (norm(color, src_avg) < norm(img_avg, src_avg))
			circle(img, Point(x, y), rad, (Scalar)color, CV_FILLED);
		
		
		// show image and iteration number every 500 iteration
		if (i % 500 == 0) {
			cout << i << endl;
			imshow("Image", img);
			char eKey = waitKey(1) & 0xFF;
		}
		
	}
	//imshow("Image", src);
	imshow("Image", img);
	char eKey = waitKey(0);
	
}