#ifndef _SENSOR_H
#define _SENSOR_H

#include <stdint.h>
#include <OpenNI.h>

using namespace openni;

enum stream_type_t
{
    RGB_STREAM,
    DEPTH_STREAM,
    IR_STREAM
};

class Sensor
{
    public:
        
        Sensor();
        ~Sensor();
        
        int initialize(stream_type_t type);
        int getWidth(stream_type_t type);
        int getHeight(stream_type_t type);
        uint16_t* getDepthFrame();
        
    private:
        Device* device_;
        VideoStream* depthStream_;
        VideoStream* rgbStream_;
        VideoFrameRef* depthFrame_;
        VideoFrameRef* rgbFrame_;
};

#endif