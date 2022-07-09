#include "pch.h"

// add by YK
#include "lib_YK.h"

Mat display_image[2];
bool display[2] = { false, false };
int th_cam1[2] = {}, th_cam2[2] = {};
int roi_point[5];   //[始点x,始点y,大きさx,大きさy,クリック回数]
int roi_point2[5];   //[始点x,始点y,大きさx,大きさy,クリック回数]
int quadrant[2] = { 0, 0 };
bool roi_ent[2] = { false,false };
double angle[2] = { 0.0, 0.0 }, re_angle[2] = { 0.0, 0.0 }, t_angle[2] = { 0.0, 0.0 }, prec_angle[2] = { 1.0, 1.0 };
Point2f center_cir;
float radius;
Moments mu1[2], mu2[2];
Point2f mc1[2], mc2[2];
Rect roi[2];
const char* th_window_name[2] = { "th_display_bottom", "th_display_side" };
Mat image_show[4];
Mat th_image_show[2];

//function
static void mouseCallback(int event, int x, int y, int flags, void* param);
Mat func_angle_v2(Mat image, int num);
/*
void mouse_callback(int event, int x, int y, int flags, void* userdata);
void mouse_callback_side(int event, int x, int y, int flags, void* userdata);
*/

//after analysis
int pre_analysis = 0;
Mat convert_gray(Mat image);

Mat convert_opencv(ImagePtr convertedImage) {
    //ImagePtr convertedImage = image->Convert(PixelFormat_BGR8, NEAREST_NEIGHBOR);

    unsigned int XPadding = convertedImage->GetXPadding();
    unsigned int YPadding = convertedImage->GetYPadding();
    unsigned int rowsize = convertedImage->GetWidth();
    unsigned int colsize = convertedImage->GetHeight();

    //image data contains padding. When allocating Mat container size, you need to account for the X,Y image data padding. 
    Mat cvimg = Mat(colsize + YPadding, rowsize + XPadding, CV_8UC1, convertedImage->GetData(), convertedImage->GetStride());
    return cvimg;
}

AngleDetection* AD_share[2];


void calcAngle(cameralistDlg* dlg, int i) {
    //cameralistDlg* dlg = (cameralistDlg*)p;
    //int number = dlg->camnumber[0][0];
    Mat image;
    double angle;
    int quad;
    double a = 0.0, b = 0.0;

    while (dlg->end_flag)
    {
        if (!calcImage[i].empty()) 
        {
            lock_guard<std::mutex> lock(mt_calc);
            resize(calcImage[i], image, Size(calcImage[i].cols / 2, calcImage[i].rows / 2));
            AD_share[i]->run(image, image, angle, quad);
            if (AD_share[i]->features.size() == 2)
            {
                //re_angle[i] = angle;

                float a0 = AD_share[i]->features[0].x - AD_share[i]->features[1].x;
                float b0 = -AD_share[i]->features[0].y + AD_share[i]->features[1].y;

                a = a0 * cos(-t_angle[i] * M_PI / 180) - b0 * sin(-t_angle[i] * M_PI / 180);
                b = a0 * sin(-t_angle[i] * M_PI / 180) + b0 * cos(-t_angle[i] * M_PI / 180);
                
                if (a != 0) {
                    double gradient = b / a;
                    double rad = atan(gradient);
                    double angle = rad * 180 / M_PI;

                    if (angle != t_angle[i]) 
                    {
                        if (a >= 0 && b >= 0) 
                        {//1
                            re_angle[i] = angle;
                            if (da_out != 1 && re_angle[i] > prec_angle[i])
                            {
                                da_out = 1;
                                voltage_bef = 0.0;
                            }
                            quadrant[i] = 1;
                        }
                        else if (a <= 0 && b >= 0)
                        {//2
                            re_angle[i] = angle + 180;
                            if (da_out != 1 && re_angle[i] > prec_angle[i])
                            {
                                da_out = 1;
                                voltage_bef = 0.0;
                            }
                            quadrant[i] = 2;
                        }
                        else if (a <= 0 && b <= 0)
                        {//3
                            re_angle[i] = 180 - angle;
                            if (da_out != 2 && re_angle[i] > prec_angle[i])
                            {
                                da_out = 2;
                                voltage_bef = 0.0;
                            }
                            quadrant[i] = 3;
                        }
                        else if (a >= 0 && b <= 0)
                        {//4
                            re_angle[i] = -angle;
                            if (da_out != 2 && re_angle[i] > prec_angle[i])
                            {
                                da_out = 2;
                                voltage_bef = 0.0;
                            }
                            quadrant[i] = 4;
                        }

                        if (re_angle[i] < prec_angle[i])
                        {
                            da_out = 3;
                        }

                        if (da_out == 0)
                        {
                            da_out = 1;
                        }
                    }
                }
            }
        }
        lock_guard<std::mutex> lock(mt_disp);
        dispImage[i] = image.clone();
    }
}


void displayImage(cameralistDlg* dlg, int i) {

    //cameralistDlg* dlg = (cameralistDlg*)p;
    //int number = dlg->camnumber[0][0];
    AD_share[i] = new AngleDetection();


    string windowname = "disp" + to_string(i);
    namedWindow(windowname, WINDOW_AUTOSIZE);
    setMouseCallback(windowname, mouseCallback, AD_share[0]);

    int key = 0;
    while (true) {
        if (!dispImage[i].empty()) {
            lock_guard<std::mutex> lock(mt_disp);
            imshow(windowname, dispImage[i]);
        }
        key = waitKey(1);

        if (getWindowProperty(windowname, WND_PROP_VISIBLE) == 0) 
        {
            break;
        }
        if (key == 0x1b)
        {
            break;
        }
    }
    destroyAllWindows();
    dlg->end_flag[i] = false;
}

/*

void display_v2() {
    while (&sw == 0);
    const char* window_name[2] = { "Bottom", "Side" };
    Mat subimage;
    
    
    namedWindow(window_name[0], WINDOW_AUTOSIZE);
    namedWindow(th_window_name[0], WINDOW_AUTOSIZE);
    if (numCameras == 2)
    {
        namedWindow(window_name[1], WINDOW_AUTOSIZE);
        namedWindow(th_window_name[1], WINDOW_AUTOSIZE);
        setMouseCallback(window_name[1], mouse_callback_side);
    }
    //namedWindow(th_re_window_name, WINDOW_AUTOSIZE);
    setMouseCallback(window_name[0], mouse_callback);
    
    //system_clock::time_point start, t_end;
    while (true) 
    {
        auto start = system_clock::now();
        for (int i = 0; i < numCameras; i++)
        {
            if (display_image[i].empty() == false)
            {
                if (display[i] == true) {

                    Mat cvimg = display_image[i].clone();
                    cv::resize(display_image[i], cvimg, Size(cvimg.cols / 2, cvimg.rows / 2));

                    //imshow(window_name[i], cvimg);
                        
                    display[i] = !display[i];
                }
            }


            if (roi_ent[i] == true)
            {


                
                Mat cvimg = th_image_show[i].clone();

                //imshow(th_window_name[i], th_image_show[i]);
            }
        }
        
        if (sw == 2) 
        {
            
            
            break;
        }
        //waitKey(1);
        //auto t_end = system_clock::now();
        //double msec = double(duration_cast<milliseconds>(t_end - start).count());
        //CString str;
        //str.Format(_T("%f"), msec);
        //print_log.push(str);
        //waitKey(1000);
    }
}
*/

void control_angle() {
    while (true)
    {
        for (int i = 0; i < numCameras; i++)
        {
                
            if (image[i] != NULL)
            {
                    
                ImagePtr convertedImage = image[i]->Convert(PixelFormat_Mono8);

                Mat cvimg = convert_opencv(convertedImage);

                if (roi_ent[i] == true)
                {
                    Mat subimage = cvimg(roi[i]);
                    subimage = func_angle_v2(subimage, i);

                    //cvtColor(subimage, subimage, COLOR_GRAY2RGB);
                    subimage.copyTo(cvimg(roi[i]));
                    rectangle(cvimg, roi[i], Scalar(0, 0, 255));
                }
                    
                if (display[i] == false) 
                {
                    display_image[i] = cvimg.clone();
                    display[i] = !display[i];
                }


            }
        }
        

        if (sw == 2)
        {
            break;
        }

    }
}

Mat func_angle_v2(Mat image, int num) {
    vector<vector<Point>> contours_cam1[2], contours_cam2[2];
    Mat gray_image, th_image_cam1[2], th_image_cam2[2], image_cam1, image_cam2;
    int length_cam1[2] = {}, length_max_cam1[2] = {}, number_cam1[2] = {};
    int length_cam2[2] = {}, length_max_cam2[2] = {}, number_cam2[2] = {};
    double a = 0.0, b = 0.0;


    gray_image = image.clone();

    if (num == 0)
    {
        threshold(gray_image, th_image_cam1[0], th_cam1[0], 255, THRESH_BINARY);
        bitwise_not(gray_image, gray_image);
        threshold(gray_image, th_image_cam1[1], th_cam1[1], 255, THRESH_BINARY);
        Mat img(Size(th_image_cam1[0].cols, th_image_cam1[0].rows / 2), CV_8UC1, Scalar::all(0));
        cv::resize(th_image_cam1[0], image_show[0], Size(th_image_cam1[0].cols / 2, th_image_cam1[0].rows / 2));
        cv::resize(th_image_cam1[1], image_show[1], Size(th_image_cam1[1].cols / 2, th_image_cam1[1].rows / 2));
        Rect comb0 = Rect(Point(0, 0), Size(th_image_cam1[1].cols / 2, th_image_cam1[1].rows / 2));
        Rect comb1 = Rect(Point(th_image_cam1[0].cols / 2, 0), Size(th_image_cam1[1].cols / 2, th_image_cam1[1].rows / 2));
        image_show[0].copyTo(img(comb0));
        image_show[1].copyTo(img(comb1));
        //imshow(th_window_name[0], img);
        th_image_show[num] = img.clone();
    }
    else if (num == 1)
    {
		threshold(gray_image, th_image_cam1[0], th_cam2[0], 255, THRESH_BINARY);
		bitwise_not(gray_image, gray_image);
		threshold(gray_image, th_image_cam1[1], th_cam2[1], 255, THRESH_BINARY);
        Mat img(Size(th_image_cam1[0].cols, th_image_cam1[0].rows/2), CV_8UC1, Scalar::all(0));
        cv::resize(th_image_cam1[0], image_show[2], Size(th_image_cam1[0].cols / 2, th_image_cam1[0].rows / 2));
        cv::resize(th_image_cam1[1], image_show[3], Size(th_image_cam1[1].cols / 2, th_image_cam1[1].rows / 2));
        Rect comb2 = Rect(Point(0, 0), Size(th_image_cam1[1].cols / 2, th_image_cam1[1].rows / 2));
        Rect comb3 = Rect(Point(th_image_cam1[0].cols / 2, 0), Size(th_image_cam1[1].cols / 2, th_image_cam1[1].rows / 2));

        image_show[2].copyTo(img(comb2));
        image_show[3].copyTo(img(comb3));
        //imshow(th_window_name[1], img);
        th_image_show[num] = img.clone();
    }


    //imshow(th_re_window_name, image_show[1]);
    //imshow(th_window_name, img);

    findContours(th_image_cam1[0], contours_cam1[0], RETR_LIST, CHAIN_APPROX_NONE);
    findContours(th_image_cam1[1], contours_cam1[1], RETR_LIST, CHAIN_APPROX_NONE);

    for (int i = 0; i < contours_cam1[0].size(); i++) {
        length_cam1[0] = arcLength(contours_cam1[0][i], true);

        if (length_cam1[0] > length_max_cam1[0]) {
            length_max_cam1[0] = length_cam1[0];
            number_cam1[0] = i;
        }
    }

    for (int i = 0; i < contours_cam1[1].size(); i++) {
        length_cam1[1] = arcLength(contours_cam1[1][i], true);

        if (length_cam1[1] > length_max_cam1[1]) {
            length_max_cam1[1] = length_cam1[1];
            number_cam1[1] = i;
        }
    }

    cvtColor(gray_image, image_cam1, COLOR_GRAY2RGB);
    image_cam1 = image.clone();
    drawContours(image_cam1, contours_cam1[0], number_cam1[0], Scalar(255, 0, 0));
    drawContours(image_cam1, contours_cam1[1], number_cam1[1], Scalar(0, 255, 0));

    if (contours_cam1[0].size() != 0 && contours_cam1[1].size() != 0)
    {
        mu1[0] = moments(contours_cam1[0][number_cam1[0]]);
        mc1[0] = Point2f(mu1[0].m10 / mu1[0].m00, mu1[0].m01 / mu1[0].m00);
        mu1[1] = moments(contours_cam1[1][number_cam1[1]]);
        mc1[1] = Point2f(mu1[1].m10 / mu1[1].m00, mu1[1].m01 / mu1[1].m00);

        float a0 = mc1[0].x - mc1[1].x;
        float b0 = -mc1[0].y + mc1[1].y;
        a = a0 * cos(-t_angle[num] * M_PI / 180) - b0 * sin(-t_angle[num] * M_PI / 180);
        b = a0 * sin(-t_angle[num] * M_PI / 180) + b0 * cos(-t_angle[num] * M_PI / 180);

        if (a != 0)
        {
            double gradient = b / a;
            double rad = atan(gradient);
            angle[num] = rad * 180 / M_PI;

            if (angle[num] != t_angle[num])
            {
                if (a >= 0 && b >= 0)
                {//1
                    re_angle[num] = angle[num];
                    if (da_out != 1)
                    {
                        da_out = 1;
                        voltage_bef = 0.0;
                    }
                    quadrant[num] = 1;
                }
                else if (a <= 0 && b >= 0)
                {//2
                    re_angle[num] = angle[num] + 180;
                    if (da_out != 1)
                    {
                        da_out = 1;
                        voltage_bef = 0.0;
                    }
                    quadrant[num] = 2;
                }
                else if (a <= 0 && b <= 0)
                {//3
                    re_angle[num] = 180 - angle[num];
                    if (da_out != 2)
                    {
                        da_out = 2;
                        voltage_bef = 0.0;
                    }
                    quadrant[num] = 3;
                }
                else if (a >= 0 && b <= 0)
                {//4
                    re_angle[num] = -angle[num];
                    if (da_out != 2)
                    {
                        da_out = 2;
                        voltage_bef = 0.0;
                    }
                    quadrant[num] = 4;
                }

                if (re_angle[num] < prec_angle[num])
                {
                    da_out = 3;
                }

                //cout << da_out << endl;

                if (da_out == 0)
                {
                    da_out = 1;
                    //cout << "da_start" << endl;
                }

            }
            cv::line(image_cam1, mc1[0], mc1[1], Scalar(0, 0, 255), 2, LINE_8);            
        }
    }
    return image_cam1;
}
/*

void mouse_callback(int event, int x, int y, int flags, void* userdata)
{

    if (event == EVENT_LBUTTONDOWN) {

        if (roi_point[4] == 0) {
            roi_point[0] = x;
            roi_point[1] = y;
            roi_point[4] = 1;

        }
        else if (roi_point[4] == 1) {
            roi_point[2] = x - roi_point[0];
            roi_point[3] = y - roi_point[1];
            roi_point[4] = 2;
            roi[0] = Rect(Point(2 * roi_point[0], 2 * roi_point[1]), Size(2 * roi_point[2], 2 * roi_point[3]));
            roi_ent[0] = true;
        }

    }
}

void mouse_callback_side(int event, int x, int y, int flags, void* userdata)
{

    if (event == EVENT_LBUTTONDOWN) {

        if (roi_point2[4] == 0) 
        {
            roi_point2[0] = x;
            roi_point2[1] = y;
            roi_point2[4] = 1;

        }
        else if (roi_point2[4] == 1) 
        {
            roi_point2[2] = x - roi_point2[0];
            roi_point2[3] = y - roi_point2[1];
            roi_point2[4] = 2;
            roi[1] = Rect(Point(2 * roi_point2[0], 2 * roi_point2[1]), Size(2 * roi_point2[2], 2 * roi_point2[3]));
            roi_ent[1] = true;
        }

    }
}

*/

/// <summary>
/// after analysis
/// </summary>
void after_analysis() {
    Mat image;
    VideoCapture video;
    CString str;
    int fps = 0;
    int count = 0;

    while (true)
    {
        if (sw == 2)
        {
            break;
        }

        if (pre_analysis == 1)
        {
            video.open(video_name);
            if (video.isOpened() == false)
            {
                CString str;
                str = "video open error";
                print_log.push(str);
                pre_analysis = 0;
            }

            count = video.get(CAP_PROP_FRAME_COUNT);
            fps = video.get(CAP_PROP_FPS);
            str.Format(_T("%d"), count);
            print_log.push(str);

            pre_analysis = 2;
            video >> image;
        }
        else if (pre_analysis == 3)
        {
            video.set(CAP_PROP_POS_FRAMES, 0);
            //video >> image;
            if (image.empty())
            {
                pre_analysis = 0;
                continue;
            }
            for (int i = 0; i < count; i++)
            {
                if (i == 0)
                {
                    csv_file.open(pathname + filename + ".csv");
                    csv_file << "Frame," << "time," <<  "Angle [°]," << "Quadrant,"  << endl;
                }

                video >> image;
                image = convert_gray(image);
                Mat subimg = image(roi[0]);
                subimg = func_angle_v2(subimg, 0);
                csv_file << i <<","<< double(i * 1.0 / fps) <<","<< re_angle[0] <<","<< quadrant[0] <<","<< endl;
                //imshow("A!", subimg);
                //waitKey(1);
            }
            //imshow("a", image);

            roi_ent[0] = false;
            roi_point[4] = 0;
            count = 0;
            pre_analysis = 0;
            csv_file.close();
            str = "Analysis end";
            print_log.push(str);
        }
        cv::waitKey(1);
    }
}

Mat convert_gray(Mat image) {
    if (image.channels() == 1)
    {
        return image;
    }
    else if (image.channels() == 3)
    {
        cvtColor(image, image, COLOR_BGR2GRAY);
        return image;
    }
}

/*

void a_roi(Mat image) {
    string windowname = "roi";
    namedWindow(windowname, WINDOW_AUTOSIZE);
    setMouseCallback(windowname, mouse_callback);

    string thwindowname = "th";
    namedWindow(thwindowname, WINDOW_AUTOSIZE);


    Mat img = convert_gray(image);
    Mat cvimg;
    while (true)
    {
        if (sw == 2)
        {
            break;
        }
        if (roi_ent[0] == false)
        {
            destroyWindow(thwindowname);
            cv::resize(img, cvimg, Size(img.cols / 2, img.rows / 2));
            imshow(windowname, cvimg);
        }
        if (roi_ent[0] == true)
        {


            Mat subimg = img(roi[0]);
            subimg = func_angle_v2(subimg, 0);
            imshow(windowname, subimg);
            imshow(thwindowname, th_image_show[0]);

        }
        if (pre_analysis == 3)
        {
            destroyAllWindows();
            roi_ent[0] = false;
            break;
        }
        //break;
        waitKey(1);
    }
}
*/


//
static void mouseCallback(int event, int x, int y, int flags, void* param)
{

    AngleDetection* self = static_cast<AngleDetection*>(param);
    self->doMouse(event, x, y, flags);

}

void AnalysisFeature(LPVOID p)
{
    analysisDlg* ptr = (analysisDlg*)p;
    Mat image;
    VideoCapture video;
    CString str;
    int fps = 0;
    int count = 0;
    AngleDetection Ang;
    double a;
    int b, key = 0;
    string win_name = "analysis";
    
    while (true)
    {
        if (ptr->flag_end)
        {
            break;
        }

        if (ptr->flag_init == 1)
        {
            video.open(video_name);
            if (video.isOpened() == false)
            {
                CString str;
                str = "video open error";
                print_log.push(str);
                pre_analysis = 0;
            }

            count = video.get(CAP_PROP_FRAME_COUNT);
            fps = video.get(CAP_PROP_FPS);
            str.Format(_T("%d"), count);
            print_log.push(str);

            namedWindow(win_name, WINDOW_AUTOSIZE);
            setMouseCallback(win_name, mouseCallback, &Ang);

            video >> image;
            resize(image, image, Size(image.cols / 2, image.rows / 2));
            while (key != 0x1b)
            {
                Mat show_img;
                Ang.run(image, show_img, a, b);
                imshow(win_name, show_img);
                key = waitKey(1);
            }
            video.set(CAP_PROP_POS_FRAMES, 0);

            ptr->flag_init = 2;
        }
        else if (ptr->flag_init == 2)
        {
            key = waitKey(1);
            video >> image;
            if (image.empty())
            {
                video.set(CAP_PROP_POS_FRAMES, 0);
                continue;
            }
            resize(image, image, Size(image.cols / 2, image.rows / 2));

            Ang.run(image, image, a, b);

            imshow(win_name, image);

            if (Ang.features.size() == 2)
            {
                cout << a << "," << b << endl;
            }
            
            if (key == 0x1b)
            {
                
                break;
            }
            else if (key == 0x72)
            {
                video.set(CAP_PROP_POS_FRAMES, 0);
            }
        }
        
    }
    ptr->flag_init = 0;
    destroyWindow(win_name);
}

