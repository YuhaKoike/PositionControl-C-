#ifndef DISPLAY
#define DISPLAY

#include <iostream>

// add by YK
#include "lib_YK.h"

void calcAngle(cameralistDlg* dlg, int number);
void displayImage(cameralistDlg* dlg, int number);
//void display_v2();
void control_angle();
void after_analysis();
void AnalysisFeature(LPVOID p);
static void mouseCallback(int event, int x, int y, int flags, void* param);



#endif //