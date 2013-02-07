#include <cstdio>

#include "sensor.h"

int main(int argc, char** argv)
{
    Sensor* sensor = new Sensor();
    sensor->initialize(DEPTH_STREAM);
    
    uint16_t* frame;
    
    while(1)
    {
        frame = sensor->getDepthFrame();
        int middleIndex = (sensor->getHeight(DEPTH_STREAM)+1)*sensor->getWidth(DEPTH_STREAM)/2;
        
    
        printf("%8d\n", frame[middleIndex]);
    }
}