//============================================================================
// Name        : background.cpp
// Author      : Hitoshi
// Version     :
// Copyright   : hitoshi@ime.usp.br
// Description : Background subtraction in C++, Ansi-style
// 				 using OpenCV 2.1
//
//============================================================================

#include <stdio.h>
#include <cv.h>
#include <highgui.h>

#include "BGS.h"

using namespace std;
using namespace cv;

#define DEBUG 1
#define MEDIAN_BGS 1
#define WIN "Input"
#define WBG "Background subtraction"
#define WAN "1st normalized color channel"
#define WBN "2nd normalized color channel"
#define WLN "Intensity channel"

#define INIT_ALPHA 1000
#define INIT_COLOR_TH  (12.0/255)
#define INIT_L_TH  0.25
#define MAX_FRAMES 500

void print_help() {
	printf(" Hitoshi's Background subtration help menu\n");
	printf("\n");
	printf("Usage: background file_name NPyrs step loThL hiThL loThC hiThC minGap maxGap hGap \n");
	printf("\n");
	printf("background : BG Subtraction for avi files.\n");
	printf("fileName :<string> clip.avi\n\n");

	printf("NPyrs   :<int>    number >= 0 of pyramid levels.\n");
	printf("step    :<int>    number of frames to be skiped between two processed frames.\n");
	printf("lo/hiThL:<float>  low/high brightness ratio thresholds. e.g: 0.15 and 0.25 \n");
	printf("lo/hiThC:<float>  low/high color thresholds. e.g: 0.022 and 0.045 \n");
	printf("minGap  :<int>    minimum gap between holes. No hole use 2, for 1 use 3, etc.\n");
	printf("maxGap  :<int>    maximum gap between object holes. eg: 9\n");
	printf("hGap    :<int>    maximum gap between horizontal holes. eg: 5\n");
	printf("\n");
}

int main(int argc, char* argv[]) {
	VideoCapture capture; // open default capture device
	Mat frame;

	print_help();

	if (argc != 11) {
		printf("\nInvalid number of arguments.\n\n");
		return(-1);
	}

	int iLevels = atoi(argv[2]);
	int iStep   = atoi(argv[3]);
	float loThL = atof(argv[4]);
	float hiThL = atof(argv[5]);
	float loThC = atof(argv[6]);
	float hiThC = atof(argv[7]);
	int minGap = atoi(argv[8]);
	int maxGap = atoi(argv[9]);
	int hGap = atoi(argv[10]);

	// set capturing from camera or file name passed as 1st argument
	capture.open(argv[1]);

	if (!capture.isOpened()) {
		printf("Could not open file %s.\n", argv[1]);
		return (-1);
	}

	// Lets grab one frame to setup all auxiliary images
	if (!capture.grab()) {
		printf("Could not grab image.\n");
		return (-1);
	}
	capture.retrieve(frame);
	printf("Input frame resolution: %d x %d\n", frame.cols, frame.rows);

	// pyramid
	vector<Mat> pyr;
	buildPyramid(frame, pyr, iLevels);
	printf("Image resolution %d x %d \n", pyr[iLevels].cols, pyr[iLevels].rows);

	// BackGround subtraction
	BGS bgs;
	bgs.Init(pyr[iLevels]);
	//  BGS parameters
	bgs.SetThresholdL(loThL, hiThL);
	bgs.SetThresholdC(loThC, hiThC);
	bgs.SetMinMaxGap(minGap, maxGap);
	bgs.SetHGap(hGap);

	// create windows to display images
	namedWindow(WIN, CV_WINDOW_AUTOSIZE );
#if DEBUG
	namedWindow(WAN, CV_WINDOW_AUTOSIZE );
	namedWindow(WBN, CV_WINDOW_AUTOSIZE );
	namedWindow(WLN, CV_WINDOW_AUTOSIZE );
#endif

	// Build background model
	bool continua = true;
	int cont = 0;
	int step = 90;
	while (continua && capture.grab()) {
		if (cont++ % step != 0)
			continue;
		capture.retrieve(frame);
		buildPyramid(frame, pyr, iLevels);
		imshow(WIN, pyr[iLevels]);

		//		bgs.Normalize( pyr[iLevels], bgs.bgmImg );
#if MEDIAN_BGS
		bgs.AddMedianSample(pyr[iLevels]);
#else
		bgs.AddMeanSample(pyr[iLevels]);
#endif
		bgs.Show(bgs.outP, WAN, WBN, WLN );

		if (waitKey(30) >= 0 || cont > MAX_FRAMES)
			continua = false;
	}
#if MEDIAN_BGS
	bgs.ComputeMedianBG();
#else
	bgs.ComputeMeanBG();
#endif
	bgs.Show(bgs.luvP, WAN, WBN, WLN );
	imshow(WIN, bgs.bgBGRImg);
	capture.release();

	printf("Press q to quit or c to continue.\n");
	char k = waitKey();
	if (k == 'q' || k == 'Q')
		return 0;

	///////////////////////////////////////////////////////////////////////
	////
	//// Process the video
	////
    ///////////////////////////////////////////////////////////////////////

	// reopen file
	capture.open(argv[1]);

	if (!capture.isOpened()) {
		printf("Could not re-open file %s.\n", argv[1]);
		return (-1);
	}

	/// Video
	VideoWriter vw;
	vw.open("movie.mpeg", CV_FOURCC('P','I','M','1'), 25, pyr[iLevels].size());

	// loop parameters
	Mat binImg;
	binImg = Mat (pyr[iLevels].size(), CV_8UC1);
	namedWindow(WBG, CV_WINDOW_AUTOSIZE );

	int notEnd = 1;
	char keyPressed;

	continua = true;
	cont = 0;
	step = iStep;

	while (continua && capture.grab()) {
		if (cont++ % step != 0)
			continue;
		capture.retrieve(frame);
		buildPyramid(frame, pyr, iLevels);
		imshow(WIN, pyr[iLevels]);

#if MEDIAN_BGS
		bgs.BGSeg(pyr[iLevels], binImg);
		imshow(WBG, binImg);
		imshow(WAN, bgs.hiImg);
		imshow(WBN, bgs.loImg);
		imshow(WLN, bgs.outP[0]);
#else
		bgs.Subtract(pyr[iLevels], binImg);
		imshow(WBG, binImg);
#endif

		Mat vid;
		cvtColor(binImg, vid, CV_GRAY2BGR);
		vw << vid;

		keyPressed = waitKey(10);

		switch (keyPressed) {
		case 'q':
		case 'Q':
			notEnd = 0;
			break;
		default:
			break;
		}



	}

	return 0;
}
