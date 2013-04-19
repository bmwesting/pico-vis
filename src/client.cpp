#include <cstdio>

#include "Sensor.h"
#include "HandProcessor.h"

#include "TuioServer.h"

// holds the state of the rendering graphics
struct graphics
{
    image_struct v_image; //rgb array showing cv results
    image_struct c_image; //rgb array showing cloud vis
    bool new_v; // is the vision image new (texture needs creating)?
    bool new_c;
};

// used for thread communication
struct thread_data
{
    std::string hostname;
    int port;
};

// processes sensor data and passes fingertips to TUIO
void* vision_thread(void* threadarg)
{
    using namespace TUIO;
    using namespace std;

    struct thread_data *data = (struct thread_data *) threadarg;
    string hostname = data->hostname;
    int port = data->port;
    
    TuioServer* tuio = new TuioServer(hostname.c_str(), port);
    TuioTime time;
    
    // initialize the sensor
    Sensor* sensor = new Sensor();
    sensor->initialize(DEPTH);
    
    HandProcessor* proc = new HandProcessor(sensor, false);
    
    vector<HandPoint> points;

    while(1)
    {
        points = proc->processHands();
        
        //printf("Number of points: %lu\n", points.size());
        
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
        
    }
    
    delete sensor;
    delete proc;
}

// listens on a socket for jpg data
void* cloud_thread(void* threadarg)
{

}

// redraws the scene
void draw(struct graphics& graphics)
{

}

// creates a new texture and ID
void generateTexture(struct image_struct& image_str)
{
    glDeleteTextures(1, image_str.id);
    glGenTextures(1, image_str.id);
    glBindTexture(GL_TEXTURE_2D, image_str.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_str.width, image_str.height, 0,
                    GL_RGB, GL_UNSIGNED_BYTE, image_str.image);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat) GL_NEAREST);
    glTexParamaterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat) GL_NEAREST);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

int main(int argc, char** argv)
{
    bcm_host_init();

    std::string hostname = "localhost";
    int t_port = 3333; // tuio port
    int c_port = 1234; // cloud port

    if(argc > 1)
    {
        hostname = argv[1];
        t_port = argv[2];
        c_port = argv[3];
    }

    // serves as a lock for textures
    thread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    // the graphics object holds the graphics state
    struct graphics graphics;

    // create data for vision thread
    struct thread_data v_data;
    v_data.hostname = hostname;
    v_data.port = t_port;
    v_data.lock = &mutex;
    v_data.graphics = &graphics;

    // create vision thread
    pthread_t v_thread;
    pthread_create(&v_thread, NULL, vision_thread, (void *) &v_data);
    thread_detach(v_thread);

    // create data for cloud thread
    struct thread_data c_data;
    c_data.hostname = hostname;
    c_data.port = c_port;
    c_data.lock = &mutex;
    c_data.graphics = &graphics;

    // create thread for cloud
    pthread_t c_thread;
    pthread_create(&c_thread, NULL, cloud_thread, (void*) &c_data);
    thread_detach(c_thread);

    // main loop
    while (1)
    {
        if (graphics.new_v)
        {
            // upload the vision texture
            pthread_mutex_lock(&mutex);
            generateTexture(graphics.v_image);
            graphics.new_v = 0;
            pthread_mutex_unlock(&mutex);
        }
        if (graphics.new_c)
        {
            // upload the cloud texture
            pthread_mutex_lock(&mutex);
            generateTexture(graphics.c_image);
            graphics.new_c = 0;
            pthread_mutex_unlock(&mutex);
        }

        draw(graphics);
    }

    return 0;
}
