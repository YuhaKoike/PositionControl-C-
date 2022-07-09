#include "pch.h"

//add by YK
#include "lib_YK.h"


AngleDetection::AngleDetection()
{
	window_name = "";
	k = 0;

	//threathold
	th = 0;

	//features
	CRITERIA = TermCriteria((TermCriteria::EPS || TermCriteria::COUNT), 10, 0.03);
	MAX_FEATURE_NUM = 2;


	//angle
	angle = -1.0;
	quadrant = 0;
}

AngleDetection::~AngleDetection()
{
}

void AngleDetection::MakeWindow(string str)
{

	//make window
	window_name = str;
	namedWindow(window_name, WINDOW_AUTOSIZE);
	
}

void AngleDetection::doMouse(int event, int x, int y, int flags)
{
	if (event == EVENT_RBUTTONDOWN)
	{
		if (!features.empty())
		{
			features.erase(features.begin());
			status.erase(status.begin());
		}
	}

	if (event == EVENT_LBUTTONDOWN)
	{
		//first
		if (features.empty())
		{
			addFeature(x, y);
			return;
		}
		int radius = 5;
		int index = getFeatureIndex(x, y, radius);

		if (index >= 0)
		{
			features[index] = features.back();
			features.pop_back();
			status[index] = status.back();
			status.pop_back();
		}
		else
		{
			addFeature(x, y);
		}
		return;
	}

}

void AngleDetection::run(Mat& img, Mat& outimg, double& outangle, int& outquadrant)
{

	frame = img.clone();
	if (frame.channels() == 3)
	{
		cvtColor(frame, gray_nimg, COLOR_BGR2GRAY);
	}
	else
	{
		gray_nimg = frame.clone();
		cvtColor(frame, frame, COLOR_GRAY2BGR);
	}

	if (gray_pimg.empty())
	{
		gray_pimg = gray_nimg.clone();
	}

	if (!features.empty())
	{
		vector<Point2f> features_prev = features;
		calcOpticalFlowPyrLK(gray_pimg, gray_nimg, features_prev, features, status, err, Size(5, 5), 3, CRITERIA, 0);

		refreshFeatures();

		if (!features.empty())
		{
			int num = 0;
			for (Point2f feature: features)
			{
				if (num == 0)
				{
					circle(frame, Point(feature.x, feature.y), 4, Scalar(0, 0, 255), -1, 8, 0);
					num++;
				}
				else
				{
					circle(frame, Point(feature.x, feature.y), 4, Scalar(0, 255, 0), -1, 8, 0);
				}
			}
		}


		CalculateAngle(angle, quadrant);
		gray_pimg = gray_nimg.clone();

	}
	

	outimg = frame;
	outangle = angle;
	outquadrant =  quadrant;
}

void AngleDetection::Display(Mat img)
{
	imshow(window_name, img);
	k = waitKey(1);

}

void AngleDetection::End()
{
	destroyAllWindows();

}

void AngleDetection::refreshFeatures()
{
	if (features.empty())
	{
		return;
	}
	int i = 0;
	while (i < features.size())
	{
		if (status[i] == 0)
		{
			features[i] = features.back();
			features.pop_back();
			status[i] = status.back();
			status.pop_back();
			i--;
		}
		i++;
	}
}

void AngleDetection::CalculateAngle(double& outangle, int& outquadrant)
{
	if (features.empty()) 
	{
		return;
	}
	else if (features.size() == 2)
	{
		float a = features[0].x - features[1].x;
		float b = -(features[0].y - features[1].y);
		if (a == 0)
		{
			outangle = 90.0;
			return;
		}
		double grad = b / a;
		double rad = atan(grad);
		angle = rad * 180 / M_PI;

		if (a >= 0 && b >= 0)
		{
			outangle = angle;
			outquadrant = 1;
		}
		else if (a <= 0 && b >= 0)
		{
			outangle = angle + 180;
			outquadrant = 2;
		}
		else if (a <= 0 && b <= 0)
		{
			outangle = 180 - angle;
			outquadrant = 3;
		}
		else if (a >= 0 && b <= 0)
		{
			outangle = -angle;
			outquadrant = 4;
		}

	}
}

void AngleDetection::addFeature(int x, int y)
{
	if (features.size() >= MAX_FEATURE_NUM)
	{
		cout << "max feature num over:" << MAX_FEATURE_NUM << endl;
	}
	else
	{
		features.push_back(Point2f(x, y));
		status.push_back('1');
	}
}

int AngleDetection::getFeatureIndex(int x, int y, int radius)
{
	int index = -1;

	if (features.empty())
	{
		return index;
	}

	int max_r2 = pow(radius, 2);
	index = 0;

	for (Point2f point : features)
	{
		int dx = x - point.x;
		int dy = y - point.y;
		int r2 = pow(dx, 2) + pow(dy, 2);

		if (r2 <= max_r2)
		{
			return index;
		}
		else
		{
			index++;
		}
	}
	return -1;
}

/// <summary>
///	using two medians
/// </summary>

FuncAngle::FuncAngle()
{
}

FuncAngle::~FuncAngle()
{
}

void FuncAngle::run(Mat& inimg, Mat& outimg, int thvalue[2], double outangle, double outquadrant, double& targetangle)
{
	if (inimg.channels() == 3)
	{
		cvtColor(inimg, gray_img, COLOR_BGR2GRAY);
	}
	else
	{
		gray_img = inimg.clone();
	}

	threshold(gray_img, th_img[0], thvalue[0], 255, THRESH_BINARY);
	bitwise_not(gray_img, gray_img);
	threshold(gray_img, th_img[1], thvalue[1], 255, THRESH_BINARY);

	Mat img(Size(th_img[0].cols, th_img[0].rows / 2), CV_8UC1, Scalar::all(0));
	cv::resize(th_img[0], th_img[0], Size(th_img[0].cols / 2, th_img[0].rows / 2));
	cv::resize(th_img[1], th_img[1], Size(th_img[1].cols / 2, th_img[1].rows / 2));
	Rect comb0 = Rect(Point(0, 0), Size(th_img[1].cols / 2, th_img[1].rows / 2));
	Rect comb1 = Rect(Point(th_img[0].cols / 2, 0), Size(th_img[1].cols / 2, th_img[1].rows / 2));
	th_img[0].copyTo(img(comb0));
	th_img[1].copyTo(img(comb1));

	for (int i = 0; i < 2; i++)
	{
		findmaxContours(i);
	}

	cvtColor(gray_img, img, COLOR_GRAY2RGB);
	img = inimg.clone();
	drawContours(img, contours[0], number[0], Scalar(255, 0, 0));
	drawContours(img, contours[1], number[1], Scalar(0, 255, 0));

	CalculateAngle(outangle, outquadrant, targetangle);
}

void FuncAngle::findmaxContours(int n)
{
	findContours(th_img[n], contours[n], RETR_LIST, CHAIN_APPROX_NONE);

	for (int i = 0; i < contours[n].size(); i++) {
		length = arcLength(contours[n][i], true);

		if (length > length_max) {
			length_max = length;
			number[n] = i;
		}
	}
}

void FuncAngle::CalculateAngle(double& outangle, double& outquadrant, double& targetangle)
{
	if (!contours[0].empty() && !contours[1].empty())
	{
		for (int i = 0; i < 2; i++)
		{
			CalculateMoments(i);
		}
		
		float a0 = mc[0].x - mc[1].x;
		float b0 = -mc[0].y + mc[1].y;
		double a = a0 * cos(-targetangle * M_PI / 180) - b0 * sin(-targetangle * M_PI / 180);
		double b = a0 * sin(-targetangle * M_PI / 180) + b0 * cos(-targetangle * M_PI / 180);

		if (a != 0)
		{
			double gradient = b / a;
			double rad = atan(gradient);
			double angle = rad * 180 / M_PI;

			if (angle != targetangle)
			{
				if (a >= 0 && b >= 0)
				{//1
					outangle = angle;
					outquadrant = 1;
				}
				else if (a <= 0 && b >= 0)
				{//2
					outangle = angle + 180;
					outquadrant = 2;
				}
				else if (a <= 0 && b <= 0)
				{//3
					outangle = 180 - angle;
					outquadrant = 3;
				}
				else if (a >= 0 && b <= 0)
				{//4
					outangle = -angle;
					outquadrant = 4;
				}

			}
			cv::line(img, mc[0], mc[1], Scalar(0, 0, 255), 2, LINE_8);
		}
	}
}

void FuncAngle::CalculateMoments(int n)
{
	mu[n] = moments(contours[n][number[n]]);
	mc[n] = Point2f(mu1[n].m10 / mu1[n].m00, mu1[n].m01 / mu1[n].m00);
}