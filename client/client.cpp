#include <cstdio>

#include "sensor.h"

// openCV
#include <opencv/highgui.h>
#include <opencv/cv.h>
using namespace cv;

const unsigned int XRES = 320;
const unsigned int YRES = 240;

int main(int argc, char** argv)
{
    Sensor* sensor = new Sensor();
    sensor->initialize(DEPTH);

    Mat depthRaw(YRES, XRES, CV_16UC1);
    Mat depthShow(YRES, XRES, CV_8UC1);

    namedWindow("debugFrame", CV_WINDOW_AUTOSIZE);

    int key = 0;
    while(key != 27 && key != 'q')
    {
        memcpy(depthRaw.data, sensor->getDepthFrame(), XRES*YRES*2);
        depthRaw.convertTo(depthShow, CV_8U, 255.0/4096.0);

        imshow("debugFrame", depthShow);

        key = waitKey(10);
    }
}