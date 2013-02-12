#include <cstdio>

#include "Sensor.h"
#include "HandProcessor.h"

#include "TuioServer.h"

int main(int argc, char** argv)
{
    using namespace TUIO;
    using namespace std;
    
    TuioServer* tuio = new TuioServer("10.0.1.2", 3333);
    TuioTime time;
    
    // initialize the sensor
    Sensor* sensor = new Sensor();
    sensor->initialize(DEPTH);
    
    HandProcessor* proc = new HandProcessor(sensor, false);
    
    std::vector<HandPoint> points;

    int key = 0;
    while(key != 27 && key != 'q')
    {
        points = proc->processHands();
        
        printf("Number of points: %lu\n", points.size());
        
        time = TuioTime::getSessionTime();
        tuio->initFrame(time);
        for(vector<HandPoint>::iterator it = points.begin(); it != points.end(); it++)
        {
            int u = (*it).x;
            int v = (*it).y;
            
            float tX = 1.0f - u / 320.0f;
            float tY = v / 240.0f;
            
            TuioCursor* cursor = tuio->getClosestTuioCursor(tX, tY);
            if (cursor == NULL || cursor->getDistance(tX, tY) > 0.05)
            {
                tuio->addTuioCursor(tX, tY);
            }
            else if (cursor->getTuioTime() != time)
            {
                tuio->updateTuioCursor(cursor, tX, tY);
            }
        }
        
        tuio->stopUntouchedMovingCursors();
        tuio->removeUntouchedStoppedCursors();
        tuio->commitFrame();
        
        key = cv::waitKey(10);
    }
    
    delete sensor;
    delete proc;
}
