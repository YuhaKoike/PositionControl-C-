#include "pch.h"
//#include <iostream>
#include <fstream>

//add by YK
#include "lib_YK.h"


float re_angle_bef = 0.0;
extern float re_angle;
extern int quadrant;

extern bool da_enable;

extern int sw;

extern int video_start;
extern double fps;
extern int th;
extern int roi_point[5];
extern bool roi_ent;
extern CFloatPtr ptrFramerate;

extern int da_out;
extern float voltage, vol_adj;

void change(int position, void* userdata);

void change(int position, void* userdata)
{
	th = position;
}

void menu() {

	int csv_write = 0;
	ofstream csv_file;
	chrono::system_clock::time_point tm_start, tm_end, tm_lap;

	string window_menu = "Menu";
	namedWindow(window_menu);
	createTrackbar("threash", window_menu, &th, 255, change);

	time_t t;
	struct tm l_time;

	bool setup_csv = false;
	//float re_angle_bef = 0.0;
	auto elapsed_bef = 0;
    while (true) {
        if (sw == 1) {

			if (csv_write == 1) {
				if (setup_csv == false) {
					t = time(NULL);
					localtime_s(&l_time, &t);
					const tm* localTime = &l_time;
					stringstream s;

					s << "20" << localTime->tm_year - 100;
					// setw(),setfill()‚Å0‹l‚ß
					s << setw(2) << setfill('0') << localTime->tm_mon + 1;
					s << setw(2) << setfill('0') << localTime->tm_mday;
					s << setw(2) << setfill('0') << localTime->tm_hour;
					s << setw(2) << setfill('0') << localTime->tm_min;
					s << setw(2) << setfill('0') << localTime->tm_sec;
					// std::string‚É‚µ‚Ä’l‚ð•Ô‚·
					string file_name = s.str();

					csv_file.open(file_name + ".csv");
					csv_file << "Time [ms]," << "Angle []," << "Quadrant," << "Voltage [V]," << "Angular Speed []," << endl;
					re_angle_bef = re_angle;
					tm_lap = tm_start;
					setup_csv = true;
				}
				tm_end = chrono::system_clock::now();
				auto elapsed = chrono::duration_cast<chrono::milliseconds>(tm_end - tm_start).count();
				float angular_speed = 1000 * 1000 * (re_angle_bef - re_angle) / float(chrono::duration_cast<chrono::milliseconds>(tm_end - tm_lap).count());

				if (elapsed != elapsed_bef) {
					csv_file << elapsed << "," << re_angle << "," << quadrant << "," << voltage << "," << angular_speed << "," << endl;
					//cout << elapsed << endl;
				}
				elapsed_bef = elapsed;
				re_angle_bef = re_angle;
				tm_lap = tm_end;
			}

			if (GetAsyncKeyState('N')) {
				roi_ent = false;
				roi_point[4] = 0;
			}

			if (GetAsyncKeyState(VK_ESCAPE)) {
				sw = 2;
				break;
			}

			/*
			if (GetAsyncKeyState(VK_RIGHT)) {
				da_out = 1;
			}

			if (GetAsyncKeyState(VK_UP)) {
				voltage += 0.5;
			}
			if (GetAsyncKeyState(VK_DOWN)) {
				voltage -= 0.5;
			}
			*/
			if (GetAsyncKeyState('P')) {
				if (da_enable == false) {
					da_enable = true;
				}

			}
			if (GetAsyncKeyState('O')) {
				if (da_enable == true) {
					da_enable = false;
				}

			}

			if (GetAsyncKeyState('S')) {
				fps = ptrFramerate->GetValue();
				video_start = 1;
				csv_write = 1;
				tm_start = chrono::system_clock::now();

			}
			if (GetAsyncKeyState('E')) {
				video_start = 2;
				csv_write = 0;
				csv_file.close();
			}
        }
        else if (sw == 2) {
            break;
        }
		//continue;
		waitKey(1);
    }
}
