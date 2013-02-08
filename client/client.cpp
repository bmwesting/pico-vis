#include <cstdio>

#include "sensor.h"

// openCV
#include <opencv/highgui.h>
#include <opencv/cv.h>
using namespace cv;

const unsigned int XRES = 320;
const unsigned int YRES = 240;

// median blur factor
const unsigned int MEDIAN_BLUR_K = 7;

Mat rotateMatrix(const Mat& source, double angle)
{
    Point2f src_center(source.cols/2.0F, source.rows/2.0F);
    Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);
    Mat dst;
    warpAffine(source, dst, rot_mat, source.size());
    return dst;
}

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
        
        depthShow = rotateMatrix(depthShow, 90);
        
        //static binary threshold
        depthShow = (depthShow > 20) & (depthShow < 30);
                
        medianBlur(depthShow, depthShow, MEDIAN_BLUR_K);
        Mat contour = depthShow.clone(); // used for further processing
        cvtColor(depthShow, depthShow, CV_GRAY2RGB);
                
        std::vector< std::vector<Point> > contours;
        findContours(contour, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

        if (contours.size()) {
            for (int i = 0; i < contours.size(); i++) {
                vector<Point> contour = contours[i];
                Mat contourMat = Mat(contour);
                double area = contourArea(contourMat);

                if(area > 1200) // likely the hand
                {
                    printf("Area of contour[%d] = %f\n", i, area);
                    
                    Scalar center = mean(contourMat);
                    Point centerPoint = Point(center.val[0], center.val[1]);
                    
                    // approximate the contour by a simple curve
                    vector<Point> approxCurve;
                    approxPolyDP(contourMat, approxCurve, 10, true);
                    
                    // draw the contour
                    vector< vector<Point> > debugContourV;
                    debugContourV.push_back(approxCurve);
                    drawContours(depthShow, debugContourV, 0, Scalar(0, 128, 0), 3);
                    
                    vector<int> hull;
                    convexHull(Mat(approxCurve), hull, false, false);

                    // draw the hull points
                    for(int j = 0; j < hull.size(); j++)
                    {
                        int index = hull[j];
                        circle(depthShow, approxCurve[index], 3, Scalar(0,128,200), 2);
                    }
                }
            }
        }
        
        resize(depthShow, depthShow, Size(), 3, 3);

        imshow("debugFrame", depthShow);

        key = waitKey(10);
    }
}