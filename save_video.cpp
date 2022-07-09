#include "pch.h"
//#include <iostream>

//add by YK
#include "lib_YK.h"


int video_start[2] = { 0,0 };
int rec_num[2] = { 0,0 };
VideoWriter writer[2];
bool setup_video[2] = { false, false };

vector<csv_info> csv_vec[2];
int elapsed[2] = { 0, 0 }, elapsed_bef[2] = { 0,0 }, voltage_time[2] = { 0, 0 };

void record() {
    time_t t;
    struct tm l_time;

    while (true)
    {
        for (int j = 0; j < numCameras; j++)
        {
            if (sw == 1)
            {
                if (video_start[j] == 1 || video_start[j] == 2)
                {
                    if (recordImage[j].size() != 0)
                    {
                        if (rec_num[j] <= recordImage[j].size() - 1)
                        {
                            Mat cvimg = recordImage[j][rec_num[j]];

                            if (setup_video[j] == false)
                            {
                                int fmt = VideoWriter::fourcc('D', 'I', 'V', '3');
                                //int fmt = 0;
                                string bos;

                                bos = "-Bottom";
                                if (j == 1)
                                {
                                    bos = "-Side";
                                }

                                if (folder == "")
                                {
                                    writer[j].open("..\\saved_file\\" + filename + bos + ".avi", fmt, icam[j].fps, Size(cvimg.cols, cvimg.rows), false);
                                }
                                else
                                {
                                    writer[j].open(folder + "\\" + filename + bos + ".avi", fmt, icam[j].fps, Size(cvimg.cols, cvimg.rows), false);
                                }

                                setup_video[j] = true;

                            }
                            writer[j] << cvimg;

                            if (j == 0)
                            {
                                rec_num[j]++;
                                record_num.push(recordImage[j].size());
                                save_num[j].push(rec_num[j]);
                            }
                            else if (j == 1)
                            {
                                rec_num[j]++;
                                save_num[j].push(rec_num[j]);
                            }
                        }
                    }
                }



                if (setup_video[j] == true && rec_num[j] == recordImage[j].size())
                {
                    video_start[j] = 3;
                }

                if (video_start[j] == 3)
                {
                    CString str;
                    str = "record end";
                    print_log.push(str);
                    video_start[j] = 0;
                    setup_video[j] = false;
                    rec_num[j] = 0;
                    recordImage[j].clear();

                }

                if (cap_image[j].empty() == false)
                {
                    ImagePtr convertedImage = cap_image[j][0];

                    unsigned int XPadding = convertedImage->GetXPadding();
                    unsigned int YPadding = convertedImage->GetYPadding();
                    unsigned int rowsize = convertedImage->GetWidth();
                    unsigned int colsize = convertedImage->GetHeight();

                    //image data contains padding. When allocating Mat container size, you need to account for the X,Y image data padding. 
                    Mat cvimg = Mat(colsize + YPadding, rowsize + XPadding, CV_8UC1, convertedImage->GetData(), convertedImage->GetStride());

                    t = time(NULL);
                    localtime_s(&l_time, &t);
                    const tm* localTime = &l_time;
                    stringstream s;

                    s << "20" << localTime->tm_year - 100;
                    // setw(),setfill()で0詰め
                    s << setw(2) << setfill('0') << localTime->tm_mon + 1;
                    s << setw(2) << setfill('0') << localTime->tm_mday;
                    s << setw(2) << setfill('0') << localTime->tm_hour;
                    s << setw(2) << setfill('0') << localTime->tm_min;
                    s << setw(2) << setfill('0') << localTime->tm_sec;
                    // std::stringにして値を返す
                    string fname = s.str();

                    string bos;
                    bos = "-Bottom";
                    if (j == 1)
                    {
                        bos = "-Side";
                    }

                    if (folder == "")
                    {
                        imwrite("..\\saved_file\\" + fname + bos + ".bmp", cvimg);
                    }
                    else
                    {
                        imwrite(folder + "\\" + fname + bos + ".bmp", cvimg);
                    }


                    CString str;
                    str = "capture image";
                    print_log.push(str);
                    capture = false;
                    cap_image[j].clear();
                }
            }

        }


        if (csv_start == 0 || csv_start == 1)
        {
            if (csv_n < csv_vec[0].size())
            {
                if (numCameras == 2)
                {
                    if (hov == 0)
                    {
                        csv_file << csv_vec[0][csv_n].elapsed << "," << csv_vec[0][csv_n].voltage_time << "," << csv_vec[0][csv_n].reangle << "," << csv_vec[0][csv_n].quadrant << "," << csv_vec[0][csv_n].voltage << "," << csv_vec[0][csv_n].angular_speed << ",,";
                        csv_file << 0 << "," << csv_vec[1][csv_n].reangle << "," << csv_vec[1][csv_n].quadrant << "," << 0 << "," << 0 << ",,";
                    }
                    else if (hov == 1)
                    {
                        csv_file << csv_vec[0][csv_n].elapsed << "," << 0 << "," << csv_vec[0][csv_n].reangle << "," << csv_vec[0][csv_n].quadrant << "," << 0 << "," << 0 << ",,";
                        csv_file << csv_vec[1][csv_n].voltage_time << "," << csv_vec[1][csv_n].reangle << "," << csv_vec[1][csv_n].quadrant << "," << csv_vec[1][csv_n].voltage << "," << csv_vec[1][csv_n].angular_speed << ",,";
                    }

                    csv_file << csv_vec[0][csv_n].p1.x << "," << csv_vec[0][csv_n].p1.y << "," << csv_vec[0][csv_n].p2.x << "," << csv_vec[0][csv_n].p2.y << endl;

                }
                else
                {
                    csv_file << csv_vec[0][csv_n].elapsed << "," << csv_vec[0][csv_n].voltage_time << "," << csv_vec[0][csv_n].reangle << "," << csv_vec[0][csv_n].quadrant << "," << csv_vec[0][csv_n].voltage << "," << csv_vec[0][csv_n].angular_speed << ",,";
                    csv_file << csv_vec[0][csv_n].p1.x << "," << csv_vec[0][csv_n].p1.y << "," << csv_vec[0][csv_n].p2.x << "," << csv_vec[0][csv_n].p2.y << endl;
                }

                csv_n++;
            }

            if (csv_start == 1 && csv_n == csv_vec[0].size())
            {
                csv_start = 2;
            }
        }
        else if (csv_start == 2)
        {
            csv_start = -1;
            csv_n = 0;
            csv_file.close();
            for (int i = 0; i < 2; i++)
            {
                csv_vec[i].clear();
            }

            CString str;
            str = "saved csv file";
            print_log.push(str);
        }



        if (sw == 2)
        {
            break;
        }
        //waitKey(1);
    }
}

void save_csv() {
    //while (&sw == 0);

    while (true)
    {
        for (int i = 0; i < numCameras; i++)
        {
            if (csv_start == 0)
            {
                //csv
                if (i == 0)
                {
                    tm_end = system_clock::now();
                }

                elapsed[i] = int(duration_cast<milliseconds>(tm_end - tm_start).count());
                float angular_speed = 1000.0 * 1000.0 * (re_angle_bef[i] - re_angle[i]) / int(duration_cast<milliseconds>(tm_end - tm_lap).count());

                if (da_enable == true)
                {
                    voltage_time[i] = int(duration_cast<milliseconds>(tm_end - voltage_start).count());
                }

                if (elapsed[i] != elapsed_bef[i] && elapsed[i] >= 0)
                {
                    csv_info csv;
                    csv.elapsed = elapsed[i];
                    csv.voltage = voltage;
                    csv.voltage_time = voltage_time[i];
                    csv.reangle = re_angle[i];
                    csv.quadrant = quadrant[i];
                    csv.angular_speed = angular_speed;


                    if (!AD_share[i]->features.empty())
                    {
                        //cout << AD_share[0]->features << endl;
                        csv.p1 = AD_share[i]->features[0] * 2;  //表示の特徴点の座標は1/2になってる
                        csv.p2 = AD_share[i]->features[1] * 2;
                    }

                    csv_vec[i].push_back(csv);

                    elapsed_bef[i] = elapsed[i];
                    re_angle_bef[i] = re_angle[i];

                    if (i == 0)
                    {
                        tm_lap = tm_end;
                    }
                }
            }
        }
            
        if (sw == 2)
        {
            break;
        }

        //waitKey(1);
    }
}