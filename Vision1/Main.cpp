#include <iostream>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\core\core.hpp"
#include "opencv2\opencv.hpp"
#include "opencv2\ml\ml.hpp"
#include <conio.h>
#include <Windows.h>
#include <stdlib.h>
#pragma comment(lib, "user32")

using namespace cv;
using namespace std;

//contour
std::vector<std::vector<cv::Point>> contours;
// testing the convex hull
vector<vector<Point> >hull;


vector<vector<Point>> pts;
int nowVector = 0;
int x = 0, y = 0;
//Treshold
int threshold_value = 57;
int threshold_type = 0;
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;
Mat tresholded;
Mat imageGrayScale, bg, bg_;
Mat edge;
char* treshWin = "Threshold Demo";

char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
char* trackbar_value = "Value";

void Threshold_Demo(int, void*);

//hsv
Mat gambarHsv, hsv_out;
int hueMin = 109, valMin = 121, satMin = 47;
int hueMax = 167, valMax = 250, satMax = 111;

//in out
void prosesGambar(Mat& in, Mat& out);
Mat gambar;
Mat rgbPicture;
bool draw = false;

void handleKeyInput();
void initWindowDialogs();

int main() {
	pts.push_back({});
	bg_ = imread("bg.jpg");

	VideoCapture cap(0);

	initWindowDialogs();

	createTrackbar(trackbar_value,
		treshWin, &threshold_value,
		max_value, Threshold_Demo);

	while (true) {

		cap >> gambar;

		rgbPicture = gambar;
		//bg_.copyTo(bg);
		//bg.copySize(gambar);

		flip(gambar, gambar, 2);
		//flip(rgbPicture, rgbPicture, 2);
		cvtColor(gambar, edge, CV_BGR2GRAY);

		cvtColor(gambar, gambarHsv, CV_BGR2HSV);
		//Canny(edge, edge, 30, 60);
		//imshow("jendelaedge", edge);
		//pyrMeanShiftFiltering(gambarHsv,gambarHsv,)
		//GaussianBlur(gambarHsv, gambarHsv, cv::Size(5, 5), 1.5);
		Threshold_Demo(0, 0);

		//cv::findContours(tresholded,
		//	contours, // a vector of contours
		//	CV_RETR_EXTERNAL, // retrieve the external contours
		//	CV_CHAIN_APPROX_NONE); // all pixels of each contours

		//// Draw black contours on a white image
		//
		//cv::drawContours(edge, contours,
		//	-1, // draw all contours
		//	cv::Scalar(255,255,255), // in white
		//	2); // with a thickness of 2
		////convexHull()
		//// testing the bounding box
		//hull.resize(contours.size());
		if (contours.size() > 0)
			for (int i = 0; i < contours.size(); i++) {
				cv::Rect r0 = cv::boundingRect(contours[i]);
				convexHull(Mat(contours[i]), hull[i], false);
				cv::rectangle(edge, r0, cv::Scalar(255), 2);
			}

		//cv::convexHull(contours[0], hull);
		cv::drawContours(edge, hull,
			-1, // draw all contours
			cv::Scalar(255, 100, 255), // in white
			2);
		/**
		* Draw polygon
		*/
		if (pts.size() > 0) {
			for (int j = 0; j < pts.size(); j++) {
				for (int i = 0; i < pts[j].size(); i++) {
					if (i == 0) continue;
					line(rgbPicture, pts[j][i], pts[j][i - 1], Scalar(255, 250, 250), 10);

				}


			}
		}
		if (pts[nowVector].size() > 0) {
			auto xS = std::to_string(pts[nowVector].back().x), yS = std::to_string(pts[nowVector].back().y);
			String pt = xS + "," + yS;
			//putText(bg, pt, pts[nowVector].back(), FONT_ITALIC, 1, Scalar(0, 255, 255), 2);
			//circle(bg, pts[nowVector].back(), 20, Scalar(255, 0, 0), 5);
		}
		/*
		Draw circle in the pointer
		*/
		circle(rgbPicture, Point(x, y), 20, Scalar(170, 170, 0), 5);
		imshow("jendelaasli", rgbPicture);
		//imshow("HSV", gambarHsv);
		cv::erode(tresholded, tresholded, cv::Mat());
		dilate(tresholded, tresholded, Mat());
		/*
		check the pointer and add polygon element if match color
		*/
		prosesGambar(tresholded, gambar);
		imshow(treshWin, tresholded);
		//imshow("ori", gambar);
		/*
		process key input
		*/
		handleKeyInput();


		waitKey(1);
	}
	waitKey(0);
	return 0;

}

void initWindowDialogs() {
	namedWindow("jendelaasli", CV_WINDOW_AUTOSIZE);
	//namedWindow("jendelaedge", CV_WINDOW_AUTOSIZE);
	namedWindow(treshWin, CV_WINDOW_AUTOSIZE);
	namedWindow("HSVTrck", CV_WINDOW_AUTOSIZE);

	//HSV
	createTrackbar("Hue min", "HSVTrck", &hueMin, 179);
	createTrackbar("Hue max", "HSVTrck", &hueMax, 179);
	createTrackbar("Sat min", "HSVTrck", &satMin, 255);
	createTrackbar("Sat max", "HSVTrck", &satMax, 255);
	createTrackbar("val min", "HSVTrck", &valMin, 255);
	createTrackbar("val max", "HSVTrck", &valMax, 255);

	//TRESH
	createTrackbar(trackbar_type,
		treshWin, &threshold_type,
		max_type, Threshold_Demo);
}

void handleKeyInput() {
	if (_kbhit()) {
		int ch = 0 | _getch();
		if (ch == 0xe0 && _kbhit()) { // extended character (0xe0, xxx)
			ch = (ch << 8) | _getch(); // get extended xharaxter info
		}
		switch (ch) {
		case 0x1b:
			pts.clear();
			nowVector = 0;
			pts.push_back({});
			cout << "BERSIHKAN GARIS.." << endl;
			break;
		case 0xe048: //UP
			cout << "Drawing Enabled" << endl;
			draw = true;
			break;
		case 0xe050: //DOWN
			cout << "Drawing Disabled" << endl;
			draw = false;
			nowVector++;
			pts.push_back({});
			break;
		case 0xe04b: //LEFT
			cout << "Left pressed" << endl;
			break;
		case 0xe04d: //RIGHT
			cout << "Right pressed" << endl;
			break;
		default:
			cout << "Some other key pressed " << hex
				<< (int)(unsigned char)ch << " '"
				<< (char)ch << "'" << endl;
			break;
		}
		cout << endl;

	}
}

void Threshold_Demo(int, void*) {
	/* 0: Binary
	1: Binary Inverted
	2: Threshold Truncated
	3: Threshold to Zero
	4: Threshold to Zero Inverted
	*/
	inRange(gambarHsv, Scalar(hueMin, satMin, valMin), Scalar(hueMax, satMax, valMax), imageGrayScale);

	//cvtColor(hsv_out, imageGrayScale, CV_BGR2GRAY);
	String tipe = "";
	switch (threshold_type)
	{
	case 0:
		tipe = "binary: if the intensity of the pixel src(x,y) is higher than thresh, then the new pixel intensity is set to a MaxVal. Otherwise, the pixels are set to 0.";
		break;
	case 1:
		tipe = "binary inverted: If the intensity of the pixel src(x,y) is higher than thresh, then the new pixel intensity is set to a 0. Otherwise, it is set to MaxVal.";
		break;
	case 2:
		tipe = "treshold truncated : if src(x,y) is greater, then its value is truncated.";
		break;
	case 3:
		tipe = "tresh to zero: If src(x,y) is lower than thresh, the new pixel value will be set to 0.";
		break;
	case 4:
		tipe = "tresh to zero inverted: If  src(x,y) is greater than thresh, the new pixel value will be set to 0.";
		break;
	default:
		break;
	}
	//cout << "TIPE :" << tipe << endl;
	threshold(imageGrayScale, tresholded, threshold_value, max_BINARY_value, threshold_type);

}

void prosesGambar(Mat& in, Mat& out)
{

	//output_image = input_image.clone();

	for (int row = 1; row < in.rows - 1; row++)
		for (int col = 1; col < in.cols - 1; col++)
		{
			//int tengah_atas = in.at<uchar>(row - 1, col);
			int kiri = in.at<uchar>(row, col - 1);
			int tengah = in.at<uchar>(row, col);
			//int kanan = in.at<uchar>(row - 1, col + 1);
			//int tengah_bawah = in.at<uchar>(row + 1, col);

			if (
				//tengah_atas == 255 &&
				kiri == 255 &&
				//kanan == 255 &&
				tengah == 255
				//tengah_bawah == 255
				)

			{
				x = col; y = row;
				auto xS = std::to_string(col);
				auto yS = std::to_string(row);
				//SetCursorPos(x, y);
				if (draw) {
					pts[nowVector].push_back(Point(col, row));
					//setcursor

				}
				goto skip;
			}
		}

skip:
	;
}

