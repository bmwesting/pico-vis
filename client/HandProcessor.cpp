#include "HandProcessor.h"

using namespace cv;
using namespace std;

const float PI = 3.14159;

const unsigned int XRES = 320;
const unsigned int YRES = 240;

// median blur factor
const unsigned int MEDIAN_BLUR_K = 7;

// cutoff factor (0.0-1.0), lower value means less points reported
const float CUTOFF_FACTOR = 0.25;

HandProcessor::HandProcessor()
{
    
}

HandProcessor::HandProcessor(Sensor* sensor, bool debug)
{
    debug_ = debug;
    sensor_ = sensor;
    
    HandProcessor();
}

HandProcessor::~HandProcessor()
{
    
}

void HandProcessor::findConvexityDefects(vector<Point>& contour, vector<int>& hull, vector<ConvexityDefect>& convexDefects)
{
    if(hull.size() > 0 && contour.size() > 0)
    {
        CvSeq* contourPoints;
        CvSeq* defects;
        CvMemStorage* storage;
        CvMemStorage* strDefects;
        CvMemStorage* contourStr;
        CvConvexityDefect *defectArray = 0;

        strDefects = cvCreateMemStorage();
        defects = cvCreateSeq( CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvSeq),sizeof(CvPoint), strDefects );

        //We transform our vector<Point> into a CvSeq* object of CvPoint.
        contourStr = cvCreateMemStorage();
        contourPoints = cvCreateSeq(CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), contourStr);
        for(int i = 0; i < (int)contour.size(); i++) {
            CvPoint cp = {contour[i].x,  contour[i].y};
            cvSeqPush(contourPoints, &cp);
        }

        //Now, we do the same thing with the hull index
        int count = (int) hull.size();
        //int hullK[count];
        int* hullK = (int*) malloc(count*sizeof(int));
        for(int i = 0; i < count; i++) { hullK[i] = hull.at(i); }
        CvMat hullMat = cvMat(1, count, CV_32SC1, hullK);

        // calculate convexity defects
        storage = cvCreateMemStorage(0);
        defects = cvConvexityDefects(contourPoints, &hullMat, storage);
        defectArray = (CvConvexityDefect*)malloc(sizeof(CvConvexityDefect)*defects->total);
        cvCvtSeqToArray(defects, defectArray, CV_WHOLE_SEQ);
        //printf("DefectArray %i %i\n",defectArray->end->x, defectArray->end->y);

        //We store defects points in the convexDefects parameter.
        for(int i = 0; i<defects->total; i++){
            ConvexityDefect def;
            def.start       = Point(defectArray[i].start->x, defectArray[i].start->y);
            def.end         = Point(defectArray[i].end->x, defectArray[i].end->y);
            def.depth_point = Point(defectArray[i].depth_point->x, defectArray[i].depth_point->y);
            def.depth       = defectArray[i].depth;
            convexDefects.push_back(def);
        }

    // release memory
    cvReleaseMemStorage(&contourStr);
    cvReleaseMemStorage(&strDefects);
    cvReleaseMemStorage(&storage);

    }
}

Mat HandProcessor::rotateMatrix(const Mat& source, double angle)
{
    Point2f src_center(source.cols/2.0F, source.rows/2.0F);
    Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);
    Mat dst;
    warpAffine(source, dst, rot_mat, source.size());
    return dst;
}

std::vector<HandPoint> HandProcessor::processHands()
{
    Mat depthRaw(YRES, XRES, CV_16U);
    Mat depthShow(YRES, XRES, CV_8U);
    
    Mat depthImage;
    
    std::vector<HandPoint> returnPoints;
    
    memcpy(depthRaw.data, sensor_->getDepthFrame(), XRES*YRES*2);
    depthRaw.convertTo(depthShow, CV_8U, 255.0/4096.0);
    
    if (debug_)
        depthImage = depthShow.clone();
    
    //depthShow = rotateMatrix(depthShow, 90);
    
    //static binary threshold
    depthShow = (depthShow > 20) & (depthShow < 35);
            
    medianBlur(depthShow, depthShow, MEDIAN_BLUR_K);
    Mat contour = depthShow.clone(); // used for further processing
    
    if (debug_)
        cvtColor(depthShow, depthShow, CV_GRAY2RGB);
            
    std::vector< std::vector<Point> > contours;
    findContours(contour, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    if (contours.size()) {
        for (int i = 0; i < contours.size(); i++) {
            vector<Point> contour = contours[i];
            Mat contourMat = Mat(contour);
            double area = contourArea(contourMat);

            if(area > 1200) // likely a hand
            {
                
                Scalar center = mean(contourMat);
                Point centerPoint = Point(center.val[0], center.val[1]);
                
                // approximate the contour by a simple curve
                vector<Point> approxCurve;
                approxPolyDP(contourMat, approxCurve, 20, true);
                
                // draw the contour
                if (debug_)
                {
                    vector< vector<Point> > debugContourV;
                    debugContourV.push_back(approxCurve);
                    drawContours(depthShow, debugContourV, 0, Scalar(0, 128, 0), 3);
                }
                
                vector<int> hull;
                convexHull(Mat(approxCurve), hull, false, false);

                // draw the hull points
                if (debug_)
                {
                    for(int j = 0; j < hull.size(); j++)
                    {
                        int index = hull[j];
                        circle(depthShow, approxCurve[index], 3, Scalar(0,128,200), 2);
                    }
                }
                
                // find the upper and lower points of the hull
                int upper = 0, lower = 240;
                for (int j = 0; j < hull.size(); j++)
                {
                    if (approxCurve[j].y > upper) upper = approxCurve[j].y;
                    if (approxCurve[j].y < lower) lower = approxCurve[j].y;
                }
                
                int cutoff = upper - (upper - lower)*CUTOFF_FACTOR;
                
                // find interior angles of hull corners
                for (int j = 0; j < hull.size(); j++)
                {
                    if (approxCurve.size() < 2) break;
                    int idx = hull[j];
                    int pdx = idx == 0 ? approxCurve.size() - 1 : idx - 1;
                    int sdx = idx == approxCurve.size() - 1 ? 0 : idx + 1;
                    
                    Point v1 = approxCurve[sdx] - approxCurve[idx];
                    Point v2 = approxCurve[pdx] - approxCurve[idx];
                    
                    float angle = acos( (v1.x*v2.x + v1.y*v2.y) / (norm(v1) * norm(v2)) ) * 180./PI;
                                            
                    // low interior angle and within cutoff = accepted point
                    if (angle < 50 && approxCurve[idx].y < cutoff)
                    {
                        HandPoint pt;
                        pt.x = approxCurve[idx].x;
                        pt.y = approxCurve[idx].y;
                        
                        returnPoints.push_back(pt);
                        
                        if (debug_)
                        {
                            circle(depthShow, Point2i(pt.x,pt.y), 4, Scalar(255, 0, 0), 3);
                            line(depthShow, Point(centerPoint.x - 100, cutoff), 
                                            Point(centerPoint.x + 100, cutoff), 
                                            Scalar(255,255,255), 2);
                        }
                    }
                }
                
                // find convexity defects
                if (debug_)
                {
                    vector<ConvexityDefect> convexDefects;
                    findConvexityDefects(approxCurve, hull, convexDefects);
                    printf("Number of defects: %d.\n", (int) convexDefects.size());

                    for(int j = 0; j < convexDefects.size(); j++)
                    {
                        circle(depthShow, convexDefects[j].depth_point, 3, Scalar(0, 255, 0), 2);

                    }
                }
            }
        }
    }
    
    if (debug_)
    {
        resize(depthShow, depthShow, Size(), 3, 3);

        imshow("Processed Image", depthShow);
        imshow("Depth Image", depthImage);
    }
    
    return returnPoints;
}


        
       