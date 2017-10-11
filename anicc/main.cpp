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
#include <chrono>
#include <iomanip>
#pragma once

#define COLOR_PRECISION 25

using namespace cv;
using namespace std;


// TODO
// maybe I should make a list based on the number of occurences in the image
// which would lead to printing more often the most used colors
Vector<Vec3b> get_colors(Mat src) {
	Vector<Vec3b> clr_plt = Vector<Vec3b>(); // color_palette
	Vector<int> clr_occ = Vector<int>(); // color occurences
	int max_x = src.cols;
	int max_y = src.rows;

	bool found;

	// iterate through image
	for (int y = 100; y < max_y; y++) {
		for (int x = 100; x < max_x; x++) {
			found = false;
			// get the color @ (x,y)
			Vec3b color = src.at<Vec3b>(Point(x, y));

			int i = 0;
			for each (Vec3b c in clr_plt) {
				// if similar color is already in the 
				// color_palette we quit the loop
				if (norm(c, color) < COLOR_PRECISION) {
					found = true;
					//if ((clr_occ[i] % 100) == 0)
						//clr_plt.push_back(c);
					clr_occ[i]++;
					break;
				}
				i++;
			}
			if (found == false) { // if no color in clr_plt
				//cout << "\n" << color_count++ << ": new color " << color << "@" << x << "x" << y << endl;
				cout << "\r" << clr_plt.size();
				clr_plt.push_back(color);
				clr_occ.push_back(1);
				if (clr_plt.size() != clr_occ.size())
					cout << "problem" << endl;
				
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
// which doesn't give a good end result for some images
Vec3b avg_circle(Mat src, int px, int py, int r) {
	// P = {(x,y) : (x-px)² + (y-py)² <= r²}
	
	int div = 0; // divider of average
	Vec3d dresult(0, 0, 0);
	
	// trying with slightly smaller radius

	// start positions
	// TODO
	// it's useless to start in a corner of a square around a circle
	// as the first r iterations on x (or y) won't match 
	// maybe start on (px, py-r) ?? next r iterations are as useless though
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
	Mat src = imread("C:/Users/Touko/code/k.jpg");
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

	chrono::system_clock::time_point time_start = chrono::system_clock::now();

	cout << "generating" << endl;
	int rad = 20; // circle radius
	for (int i=0; i < 100000000; i++) {
		int x = rand() % img.cols;
		int y = rand() % img.rows;

		// get a random color from palette
		Vec3b color = color_pal[rand()%color_pal.size()];
		// get average color within circle on source image
		Vec3b src_avg = avg_circle(src, x, y, rad);
		// get average color withing circle on generate image 
		// TODO 
		// this is bloated, since I only draw circles
		// the color in the center will be the same as the rest of circle
		//Vec3b img_avg = avg_circle(img, x, y, rad);
		Vec3b img_avg = img.at<Vec3b>(Point(x, y));
		
		
		// compare values, only draw the closest to original image
		if (norm(color, src_avg) < norm(img_avg, src_avg))
			circle(img, Point(x, y), rad, (Scalar)color, CV_FILLED);
		
		
		// show image and iteration number every 500 iteration
		if (i % 500 == 0) {
			chrono::duration<double> time_now = chrono::system_clock::now() - time_start;
			cout << "\r" << i << " in " << (int)time_now.count()/60 << ":" << (int)time_now.count() % 60;
			imshow("Image", img);
			char eKey = waitKey(1) & 0xFF;
			if (i == 50000)
				rad = 4;
			else if (i == 150000)
				rad = 3;
			else if (i == 300000)
				rad = 2;
			else if (i == 1000000)
				rad = 1;
		} 
		
	}
	//imshow("Image", src);
	imshow("Image", img);
	char eKey = waitKey(0);
	
}