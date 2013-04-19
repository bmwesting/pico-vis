#include "PicoTuioServer.h"

#include <TuioTime.h>

PicoTuioServer::PicoTuioServer(std::string hostname, unsigned int port) :
                        TuioServer(hostname.c_str(), port),
                        stateChanged_(false)
{}

void PicoTuioServer::addPoint(float x, float y)
{
    // range clamping normalized coordinates
    if (x > 1.0f) x = 1.0f;
    if (x < 0.0f) x = 0.0f;
    if (y > 1.0f) x = 1.0f;
    if (y < 0.0f) y = 0.0f;

    // begin a new frame
    if(!stateChanged_)
    {
        stateChanged_ = !stateChanged_;
        TUIO::TuioTime time = TUIO::TuioTime::getSessionTime();
        TuioServer::initFrame(time);
    }

    return TuioServer::addTuioCursor(x, y);
}

void PicoTuioServer::commit()
{
    if(stateChanged_)
    {
        stateChanged_ = !stateChanged_;
        TuioServer::stopUntouchedMovingCursors();
        TuioServer::removeUntouchedStoppedCursors();
        TuioServer::commitFrame();
    }
}
