#ifndef _HAND_PROCESSOR_H
#define _HAND_PROCESSOR_H

#include "Sensor.h"

#include <opencv/highgui.h>
#include <opencv/cv.h>

#include <vector>

enum debug_type_t
{
    DEBUG_OFF,
    DEBUG_ON
};

// conversion from cvConvexityDefect
struct ConvexityDefect
{
    cv::Point start;
    cv::Point end;
    cv::Point depth_point;
    float depth;
};

struct HandPoint
{
    int x;
    int y;
};

class HandProcessor
{
    public:
        HandProcessor();
        HandProcessor(Sensor* sensor, debug_type_t);
        ~HandProcessor();

        void findConvexityDefects(std::vector<cv::Point>& contour, std::vector<int>& hull, std::vector<ConvexityDefect>& convexDefects);
        cv::Mat rotateMatrix(const cv::Mat& source, double angle);
        std::vector<HandPoint> processHands();

    private:
        Sensor* sensor_;
        debug_type_t debug_ = DEBUG_OFF;

        cv::Mat depthRaw;
        cv::Mat depthShow;
};

#endif