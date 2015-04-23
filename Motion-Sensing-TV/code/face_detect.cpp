#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <alsa/asoundlib.h>

#include <iostream>
#include <stdio.h>
#include <cmath>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );
void SetAlsaMasterVolume(long volume);

/** Global variables */
String face_cascade_name = "haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);

/** @function main */
int main( int argc, const char** argv )
{
    CvCapture* capture;
    Mat frame;

    //-- 1. Load the cascades
    if( !face_cascade.load(face_cascade_name)){
        printf("--(!)Error loading\n");
        return -1;
    }

    //-- 2. Read the video stream
    capture = cvCaptureFromCAM( -1 );
    if(capture){
        while(true){
            frame = cvQueryFrame(capture);

            //-- 3. Apply the classifier to the frame
            if(!frame.empty()){
                detectAndDisplay(frame);
            }
            else{
                printf(" --(!) No captured frame -- Break!");
                break;
            }

        int c = waitKey(10);
        if((char)c == 'c'){
            break;
        }
      }
   }
   return 0;
 }

/** @function detectAndDisplay */
void detectAndDisplay( Mat frame )
{
    std::vector<Rect> faces, humanoids, found_filtered;
    Mat frame_gray;

    HOGDescriptor hog;
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

    cvtColor(frame, frame_gray, CV_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

    size_t i, j;
    for (i=0; i<faces.size(); i++){
        Rect r = faces[i];
        for (j=0; j<faces.size(); j++)
            if (j!=i && (r & faces[j])==r)
                break;
        if (j==faces.size())
            found_filtered.push_back(r);
    }
    for (i=0; i<found_filtered.size(); i++){
        Rect r = found_filtered[i];
        r.x += cvRound(r.width*0.1);
        r.width = cvRound(r.width*0.8);
        r.y += cvRound(r.height*0.06);
        r.height = cvRound(r.height*0.9);
        rectangle(frame, r.tl(), r.br(), cv::Scalar(0,255,0), 2);
    }

    //-- Detect humanoid form
    hog.detectMultiScale(frame, humanoids, 0, Size(8,8), Size(32,32), 1.05, 2);
    for (i=0; i<humanoids.size(); i++){
        Rect r = humanoids[i];
        for (j=0; j<humanoids.size(); j++)
            if (j!=i && (r & humanoids[j])==r)
                break;
        if (j==humanoids.size())
            found_filtered.push_back(r);
    }
    for (i=0; i<found_filtered.size(); i++)
    {
        Rect r = found_filtered[i];
        r.x += cvRound(r.width*0.1);
        r.width = cvRound(r.width*0.8);
        r.y += cvRound(r.height*0.06);
        r.height = cvRound(r.height*0.9);
        rectangle(frame, r.tl(), r.br(), cv::Scalar(0,255,0), 2);
    }

    float total_area = frame.cols * frame.rows;
    float min_area = total_area;
    int min_index = 0;
    for(int i  = 0; i < found_filtered.size(); i++){
        float area = found_filtered[i].width * found_filtered[i].height;
        if(area < min_area && area > 10){
            min_area = area;
            min_index = i;
        }
    }
    if(min_area == total_area){
        min_area = 0;
    }

    if(found_filtered.size() != 0){
        //-- Get angle
        float disp_x = found_filtered[min_index].x - frame.cols/2;
        float angle = (atan(disp_x*2/frame.cols)*180)/M_PI;

        cout << "disp_x " << disp_x << " |||| angle = " << angle << endl;
        int int_angle = angle;
        //-- Set volume
        long volume = 100 - (4*min_area * 100)/total_area;
        cout << "Volume = " << volume << endl;
        SetAlsaMasterVolume(volume);
    }
    imshow(window_name, frame);
 }

void SetAlsaMasterVolume(long volume){
    long min, max;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "Master";

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(handle);
}