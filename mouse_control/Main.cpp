#include "opencv2/opencv.hpp"
#include "handTracker.h"
#include<windows.h>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	Mat frame;	
	Rect trackBox;
	Point prePoint, curPoint;
	VideoCapture capture;
	bool gotTrackBox = false;
	int interCount = 0;
	int continue_fist = 0;

	capture.open(0);
	if (!capture.isOpened())
	{
		cout<<"No camera!\n"<<endl;
		return -1;
	}

	HandTracker hand_tracker;

	while (1)
	{
		/************ Get hand which need to be tracked ************/
		while (!gotTrackBox)
		{
			capture >> frame;
			if(frame.empty())
				return -1;

            if (hand_tracker.init(frame, trackBox))
			{
                gotTrackBox = true;
				prePoint = Point(trackBox.x + 0.5 * trackBox.width, trackBox.y + 0.5 * trackBox.height);
			}

			imshow("handTracker", frame);
				
			if (waitKey(2) == 27)
				return -1;
		}

		capture >> frame;
		double t = (double)cvGetTickCount();
		

		/****************** Tracking hand **************************/
		if (!hand_tracker.processFrame(frame, trackBox))
			gotTrackBox = false;
		cout<<"true"<<endl;


		/****************** Control the mouse *********************/
		curPoint = Point(trackBox.x + 0.5 * trackBox.width, trackBox.y + 0.5 * trackBox.height);
		int dx = curPoint.x - prePoint.x;
		int dy = curPoint.y - prePoint.y;
		float k = 1.5;
		mouse_event(MOUSEEVENTF_MOVE, -k * dx, k * dy, 0, 0);
		prePoint = curPoint;

		// When you made a fist, means you click left button of mouse
		// To avoid successive active within short time, when you had actived a single 
		// we will ingnore the next 10 frames
		interCount++;
		if(interCount > 10 && hand_tracker.detectFist(frame, trackBox))
		{
			// To avoid detecting error, need to successive detect three times successfully
			continue_fist++;
			if (continue_fist > 3)
			{
				interCount = 0;
				cout << "YOU active a single click command!" << endl;
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			}
		}
		else
			continue_fist = 0;


		/******************* Show image ****************************/
		rectangle(frame, trackBox, Scalar(0, 0, 255), 3);
		imshow("handTracker", frame);


		/******************* Show cost time ************************/
		t = (double)cvGetTickCount() - t;
        cout << "cost time: " << t / ((double)cvGetTickFrequency()*1000.) << endl;

		if ( cvWaitKey(3) == 27 )
			break;
	}

	return 0;
}
