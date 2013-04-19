#ifndef _PICOTUIOSERVER_H
#define _PICOTUIOSERVER_H

#include <string>

#include <TuioServer.h>

class PicoTuioServer : public TUIO::TuioServer
{
    public:
        PicoTuioServer(std::string hostname = "localhost",
                                    unsigned int port = 3333);

        void addPoint(float x, float y);
        void commit();

    private:
        bool stateChanged_;
};

#endif