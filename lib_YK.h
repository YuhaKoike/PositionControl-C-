#ifndef LIB_YK
#define LIB_YK

#pragma comment(lib,"comctl32.lib")

//Dialogs
#include "analysisDlg.h"
#include "cameralistDlg.h"
#include "CameraControl.h"

//gui
#include "framework.h"
#include "position_control.h"
#include "position_controlDlg.h"
#include "afxdialogex.h"

//add by YK
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <Spinnaker.h>
#include <SpinGenApi/SpinnakerGenApi.h>
#include <SpinVideo.h>
#include <thread>
#include "flircam.h"
#include <chrono>
#include "display.h"
#include <WinUser.h>
#include <queue>
#include <fstream>

//da board
#include <conio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <malloc.h>
#include <mmsystem.h>
#include "FbiDa.h"
#include "da_board.h"

#include "save_video.h"

//angle detection
#include "angle_detection.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace Spinnaker::Video;
using namespace std;
using namespace cv;
using namespace chrono;

//flircam
extern int sw;
extern int record_size;
extern unsigned int numCameras;
extern bool capture;
extern ImagePtr image[2];
extern vector<ImagePtr> record_image[2], cap_image[2];
extern CFloatPtr ptrFramerate[2], ptrExposure[2], ptrGain[2];

//display
extern Mat display_image[2];
extern int th_cam1[2], th_cam2[2];
extern int roi_point[5];
extern int roi_point2[5];
extern int quadrant[2];
extern bool roi_ent[2];
extern double angle[2], re_angle[2], t_angle[2], prec_angle[2];
extern Point2f center_cir;
extern float radius;
extern Moments mu1[2], mu2[2];
extern Point2f mc1[2], mc2[2];
extern Rect roi[2];
extern Mat th_image_show[2];


//save_video
extern int video_start[2];
extern VideoWriter writer[2];
extern bool setup_video[2];
extern int rec_num[2];

//da_board
extern int da_out;
extern double voltage, voltage_bef, vol_adj, max_voltage, min_voltage;
extern int freq;
extern bool da_enable;
extern bool x_axis, y_axis, ccw;
extern bool init_vol;
extern int hov;
extern int da_auto;
extern double Kp, Ki, Kd;
extern double Kp_ver, Ki_ver, Kd_ver;

//position_controlDlg
extern queue <int> record_num, save_num[2];
extern queue <CString> print_log;
extern string folder;
extern bool csv_write;
extern int csv_start;
extern string filename, pathname;
extern ofstream csv_file;
extern double re_angle_bef[2];
extern system_clock::time_point tm_start, tm_end, tm_lap, voltage_start;
extern int csv_n;
extern string video_name;
extern int pre_analysis;

extern AngleDetection* AD_share[2];

typedef struct csv_info 
{
	int elapsed;
	int voltage_time;
	double reangle;
	int quadrant;
	double voltage;
	float angular_speed;
	Point2f p1, p2;

};

typedef struct cam_info
{
	double fps, fps_min, fps_max;
	double exptime, exptime_min, exptime_max;
	double gain, gain_min, gain_max;
	CFloatPtr ptrFramerate, ptrExposure, ptrGain;
};

typedef struct angle_info
{

};

/*

void CreateConsole(void)
{
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
};
*/
extern vector<csv_info> csv_vec[2];
extern cam_info icam[2];


#endif //

/*
measurement speed

system_clock::time_point start, end;
start = system_clock::now();
end = system_clock::now();
double time = static_cast<double>(duration_cast<microseconds>(end-start).count()/1000.0);
printf("time %lf[ms]\n",time);
*/

extern Mat dispImage[2], calcImage[2];
extern vector<Mat> recordImage[2];
extern mutex mt_calc, mt_disp;