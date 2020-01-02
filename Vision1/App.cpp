#include "App.h"

using namespace std;

App::App()
{
}


App::~App()
{
}

void App::processImage(cv::Mat& in, cv::Mat& out)
{
	 
	for (int row = 1; row < in.rows - 1; row++)
		for (int column = 1; column < in.cols - 1; column++)
		{
			//int tengah_atas = in.at<uchar>(row - 1, col);
			int leftSidePixel = in.at<uchar>(row, column - 1);
			int centerSidePixel = in.at<uchar>(row, column);
			//int kanan = in.at<uchar>(row - 1, col + 1);
			//int tengah_bawah = in.at<uchar>(row + 1, col);

			/*Determine surrounding pixel, one pixel not enough*/

			if (
				//tengah_atas == 255 &&
				leftSidePixel == 255 &&
				//kanan == 255 &&
				centerSidePixel == 255
				//tengah_bawah == 255
				)

			{
				x = column; 
				y = row;
				auto xS = std::to_string(column);
				auto yS = std::to_string(row);
				//SetCursorPos(x, y);
				if (drawingMode) {
					points[polygonCount].push_back(cv::Point(column, row));
					//setcursor

				}
				goto skip;
			}
		}

skip:
	;
}
 
void App::handleKeyInput()
{
	if (_kbhit()) {
		int ch = 0 | _getch();
		if (ch == 0xe0 && _kbhit()) { // extended character (0xe0, xxx)
			ch = (ch << 8) | _getch(); // get extended xharaxter info
		}
		switch (ch) {
		case 0x1b:
			clearPolygon();
			break;
		case 0xe048: //UP
			enableDrawing();
			break;
		case 0xe050: //DOWN
			disableDrawing();
			break;
		case 0xe04b: //LEFT cout << "Left pressed" << endl;
			cout << "Leaving the App" << endl;
			endApp();
			break;
		case 0xe04d: //RIGHT cout << "Right pressed" << endl;
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

void App::disableDrawing() {
	cout << "Drawing Disabled" << endl;
	drawingMode = false;
	polygonCount++;
	points.push_back({});
}

void App::enableDrawing() {
	cout << "Drawing Enabled" << endl;
	drawingMode = true;
}

void App::clearPolygon() {
	points.clear();
	polygonCount = 0;
	/*
	initiate new polygon arrays if user wnts to enable drawing again
	*/
	points.push_back({});
	cout << "Clear Polygons.." << endl;
}

void App::initWindowDialogs()
{
	/*Create windows*/
	cv::namedWindow(ORIGINAL_WINDOW, CV_WINDOW_AUTOSIZE);
	//namedWindow("jendelaedge", CV_WINDOW_AUTOSIZE);
	cv::namedWindow(TRESHOLD_WINDOW, CV_WINDOW_AUTOSIZE);
	cv::namedWindow(HSV_TRACK_WINDOW, CV_WINDOW_AUTOSIZE);

	/*Create Controller Trackbar*/

	//HSV
	cv::createTrackbar("Hue min", HSV_TRACK_WINDOW, &hueMin, 179);
	cv::createTrackbar("Hue max", HSV_TRACK_WINDOW, &hueMax, 179);
	cv::createTrackbar("Sat min", HSV_TRACK_WINDOW, &satMin, 255);
	cv::createTrackbar("Sat max", HSV_TRACK_WINDOW, &satMax, 255);
	cv::createTrackbar("val min", HSV_TRACK_WINDOW, &valMin, 255);
	cv::createTrackbar("val max", HSV_TRACK_WINDOW, &valMax, 255);

	 
}

void App::endApp()
{
	this->running = false;
}
 
int App::run()
{
	points.push_back({});
	bg_ = cv::imread("bg.jpg");

	cv::VideoCapture cap(0);

	initWindowDialogs(); 

	while (running) {

		cap >> mainPicture;
		rgbPicture = mainPicture;

		/*mirror*/
		cv::flip(mainPicture, mainPicture, 2);

		/*convert color*/
		cv::cvtColor(mainPicture, edge, CV_BGR2GRAY);
		cv::cvtColor(mainPicture, hsvPicture, CV_BGR2HSV);

		initThreshold(0, 0);
		 
		performDrawing();

		/*
		 ==========================================
		 ========== SHOW THE IMAGE ================
		 ==========================================
		*/
		imshow(ORIGINAL_WINDOW, rgbPicture);
		//imshow("HSV", gambarHsv);
		cv::erode(tresholded, tresholded, cv::Mat());
		dilate(tresholded, tresholded, cv::Mat());
		/*
		check the pointer and add polygon element if match color
		*/
		processImage(tresholded, mainPicture);
		imshow(TRESHOLD_WINDOW, tresholded);

		/*
		process key input
		*/
		handleKeyInput(); 
		cv::waitKey(1);
	}
	/*cv::waitKey(0);*/
	return 0;
}

void App::performDrawing() {
	if (contours.size() > 0)
		for (int i = 0; i < contours.size(); i++) {
			cv::Rect r0 = cv::boundingRect(contours[i]);
			cv::convexHull(cv::Mat(contours[i]), hull[i], false);
			cv::rectangle(rgbPicture, r0, cv::Scalar(255), 2);
		}

	//cv::convexHull(contours[0], hull);
	cv::drawContours(rgbPicture, hull,
		-1, // draw all contours
		cv::Scalar(255, 100, 255), // in white
		2);

	/**
	====================
	=== DRAW POLYGON ===
	====================
	*/
	if (points.size() > 0) {
		for (int j = 0; j < points.size(); j++) {
			for (int i = 0; i < points[j].size(); i++) {
				if (i == 0) continue;

				//do draw line
				cv::line(rgbPicture, points[j][i], points[j][i - 1], cv::Scalar(255, 250, 250), 10);
			}
		}
	}
	if (points[polygonCount].size() > 0) {
		auto xS = std::to_string(points[polygonCount].back().x), yS = std::to_string(points[polygonCount].back().y);
		cv::String coordinate = xS + "," + yS;
		cv::putText(rgbPicture, coordinate, points[polygonCount].back(), cv::FONT_ITALIC, 1, cv::Scalar(0, 255, 255), 2);
		//circle(bg, pts[polygonCount].back(), 20, Scalar(255, 0, 0), 5);
	}
	/*
	Draw circle in the pointer
	*/
	cv::circle(rgbPicture, cv::Point(x, y), 20, cv::Scalar(170, 170, 0), 5);
}

void App::initThreshold(int, void*) {
	/* 0: Binary
	1: Binary Inverted
	2: Threshold Truncated
	3: Threshold to Zero
	4: Threshold to Zero Inverted
	*/
	cv::inRange(hsvPicture, cv::Scalar(hueMin, satMin, valMin), cv::Scalar(hueMax, satMax, valMax), imageGrayScale);

	//cvtColor(hsv_out, imageGrayScale, CV_BGR2GRAY);
	cv::String tipe = "";
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
	cv::threshold(imageGrayScale, tresholded, threshold_value, max_BINARY_value, threshold_type);

}
