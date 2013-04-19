
/*
Universal controller execution entry point.

The controller processes the finger locations and/or gestures of the hand and passes
them via network protocol to a listening client.

Author: Brandt Westing - 04/16/2013
*/

#include <vector>
#include <string>

#include "Sensor.h"
#include "PicoTuioServer.h"
#include "HandProcessor.h"

int main(int argc, char* argv[])
{
    std::string tuioHostname = "localhost";
    unsigned int tuioPort = 3333;
    debug_type_t debug = DEBUG_OFF;

    // get hostname/port from command line args
    if(argc > 1)
    {
        for(int i = 1; i < argc; i++)
        {
            std::string arg = argv[i];
            if("-h" == arg)
                tuioHostname = argv[i++];
            else if("-p" == arg)
                tuioPort = atoi(argv[i++]);
            else if("-d" == arg)
                debug = DEBUG_ON;
            else
                std::cout << "usage: ./cont -h <hostname> -p <port>" << std::endl;
        }
    }

    // create a TUIO server
    PicoTuioServer* tuio = new PicoTuioServer(tuioHostname, tuioPort);

    // initialize the depth sensor
    Sensor* sensor = new Sensor();
    sensor->initialize(DEPTH);

    HandProcessor* hProc = new HandProcessor(sensor, debug);

    std::vector<HandPoint> points;

    while(1)
    {
        // get latest data
        points = hProc->processHands();

        for(std::vector<HandPoint>::iterator it = points.begin(); it != points.end(); it++)
        {
            int u = (*it).x;
            int v = (*it).y;

            float tX = 1.0f - u / sensor->getWidth(DEPTH);
            float tY = v / sensor->getHeight(DEPTH);

            tuio->addPoint(tX, tY);
        }

        tuio->commit();
    }

    delete tuio;
    delete sensor;
    delete hProc;

    return 1;
}