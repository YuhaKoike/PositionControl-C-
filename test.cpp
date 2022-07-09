#include "pch.h"
#include "test.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int i = 0;


void read_movie() {
    string path = "H:\\20_ÉtÉHÉçÅ[\\Ç–ÇÎÇ‚Ç∑\\program\\test\\sample.avi";
    VideoCapture video;
    video.open(path);
    if (video.isOpened() == false) {
        cout << "Video is not opened" << endl;
        return;
    }

    int fps = video.get(CAP_PROP_FPS);
    int width = video.get(CAP_PROP_FRAME_WIDTH);
    int height = video.get(CAP_PROP_FRAME_HEIGHT);
    Mat image, subimage;
    String window = "window";

    video >> image;

    namedWindow(window, WINDOW_AUTOSIZE);

    video.set(CAP_PROP_POS_FRAMES, 0);
    while (true) {
        video >> image;

        if (image.empty() == false) {

            resize(image, image, Size(width / 2, height / 2));

            imshow(window, image);

        }

        else if (image.empty() == true) {

            video.set(CAP_PROP_POS_FRAMES, 0);
            //break;
        }

        if (i != 0) {
            break;
        }
        waitKey(1);

    }

    return ;

}