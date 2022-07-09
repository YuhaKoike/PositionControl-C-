#pragma once
using namespace std;
using namespace cv;

//using features
class AngleDetection
{
public:
	AngleDetection();
	~AngleDetection();

public:
	void MakeWindow(string str);
	void doMouse(int event, int x, int y, int flags);
	void run(Mat& img, Mat& outimg, double& outangle, int& outquadrant);
	void Display(Mat img);
	void End();
	void refreshFeatures();
	void CalculateAngle(double& angle, int& quadrant);
	void addFeature(int x, int y);
	int getFeatureIndex(int x, int y, int radius);

	string window_name;
	int k;
	//threathold
	int th;
	//frames
	Mat frame;
	Mat gray_nimg;	//now
	Mat gray_pimg;	//previous
	Mat bin_img;	//binary
	int MAX_FEATURE_NUM;
	//features
	vector<Point2f> features;
	vector<uchar> status;
	vector<float> err;
	TermCriteria CRITERIA;
	//angle
	double angle;
	int quadrant;

private:

};

//using liner fitting

//using median

//using two median
class FuncAngle
{
public:
	FuncAngle();
	~FuncAngle();

public:
	void run(Mat& inimg, Mat& outimg, int thvalue[2], double outangle, double outquadrant, double& targetangle);
	void findmaxContours(int n);
	void CalculateAngle(double& outangle, double& outquadrant, double& targetangle);
	void CalculateMoments(int n);

	Mat img, gray_img;
	Mat th_img[2];
	vector<vector<Point2f>> contours[2];
	int length, length_max, number[2];
	Moments mu[2];
	Point2f mc[2];
private:

};

