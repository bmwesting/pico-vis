#include <cstdio>

#include "Sensor.h"

using namespace openni;

Sensor::Sensor()
{
    Status rc = OpenNI::initialize();
	if (rc != STATUS_OK)
	{
		printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
	}

	device_ = new Device();
	rc = device_->open(ANY_DEVICE);
	if (rc != STATUS_OK)
	{
		printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
        delete device_ ;
	}
}

int Sensor::initialize(stream_type_t type)
{
    Status rc;
    if(DEPTH == type)
    {
        depthStream_ = new VideoStream();
        const SensorInfo* info = device_->getSensorInfo(SENSOR_DEPTH);
        if (info != NULL)
        {

            // choose the correct videomode
            printf("Available video modes for this sensor:\n");
            for(int i = 0; i < info->getSupportedVideoModes().getSize(); i++)
            {
                int xRes  = info->getSupportedVideoModes()[i].getResolutionX();
                int yRes  = info->getSupportedVideoModes()[i].getResolutionY();
                int fps   =  info->getSupportedVideoModes()[i].getFps();
                int pixel = info->getSupportedVideoModes()[i].getPixelFormat();
                printf("Video Mode %d: %dx%d at %d with pixel format=%d\n", i, xRes, yRes, fps, pixel);
            }

            rc = depthStream_->create(*device_, SENSOR_DEPTH);
            if (rc != STATUS_OK)
            {
                printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
                delete device_ ;
                delete depthStream_ ;
                return -1;
            }

            // turn off mirroring
            depthStream_->setMirroringEnabled(false);

            // set video mode to option 6, (160x120 @ 30 (mm))
            rc = depthStream_->setVideoMode(info->getSupportedVideoModes()[4]);
            if (rc != STATUS_OK)
            {
                printf("Couldn't set video mode!\n%s\n", OpenNI::getExtendedError());
                delete device_ ;
                delete depthStream_ ;
                return -1;
            }

        }

        rc = depthStream_->start();
        if (rc != STATUS_OK)
        {
            printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
            delete device_ ;
            delete depthStream_ ;
            return-1;
        }

        depthFrame_ = new VideoFrameRef();
    }

    return 0;
}

Sensor::~Sensor()
{

    if(depthStream_)
    {
        depthStream_->stop();
        depthStream_->destroy();
        delete depthStream_;
    }
    if(rgbStream_)
    {
        rgbStream_->stop();
        rgbStream_->destroy();
        delete rgbStream_;
    }
    if(depthFrame_)
        delete depthFrame_;
    if(rgbFrame_)
        delete rgbFrame_;
    if(device_)
        delete device_;

    OpenNI::shutdown();
}

int Sensor::getWidth(stream_type_t type)
{
    if (DEPTH == type)
        return depthFrame_->getWidth();
    else if (RGB == type)
        return rgbFrame_->getWidth();

    return -1;
}

int Sensor::getHeight(stream_type_t type)
{
    if (DEPTH == type)
        return depthFrame_->getHeight();
    else if (RGB == type)
        return rgbFrame_->getHeight();

    return -1;
}

uint16_t* Sensor::getDepthFrame()
{
    Status rc = depthStream_->readFrame(depthFrame_);
    if (rc != STATUS_OK)
    {
        printf("Wait failed\n");
    }

    if (depthFrame_->getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_1_MM && depthFrame_->getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_100_UM)
    {
        printf("Unexpected frame format\n");
    }

    return (uint16_t*) depthFrame_->getData();
}
