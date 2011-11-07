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
#define MAX_FRAMES 100

void printHelp() {
	printf("Usage: bgpets seqName start end step NPyrs loThL hiThL loThC hiThC minGap maxGap hGap\n");
	printf("\n");
	printf("bgpets  : Background Subtraction for the PETS sequences, in jpeg format.\n");
	printf("seqName :<string> e.g. S1-T1-C/video/1/S1-T1-C\n\n");

	printf("BGstart :<int>    first sequence frame to learn background. \n");
	printf("BGend   :<int>    last sequence frame to learn background.\n");
	printf("BGstep  :<int>    number of frames to step between background frames.\n\n");

	printf("start   :<int>    first sequence frame. \n");
	printf("end     :<int>    last sequence frame.\n");
	printf("step    :<int>    number of frames to step between processed frames.\n\n");

	printf("NPyrs   :<int>    number >= 0 of pyramid levels.\n");
	printf("lo/hiThL:<float>  low/high brightness ratio thresholds. e.g: 0.15 and 0.25 \n");
	printf("lo/hiThC:<float>  low/high color thresholds. e.g: 0.022 and 0.045 \n");
	printf("minGap  :<int>    minimum gap between holes. No hole use 2, for 1 use 3, etc.\n");
	printf("maxGap  :<int>    maximum gap between object holes. eg: 9\n");
	printf("hGap    :<int>    maximum gap between horizontal holes. eg: 5\n");
	printf("\n");
}

int main(int argc, char* argv[]) {

	Mat frame;

	printHelp();
	if (argc != 16) {
		printf("\nInvalid parameters\n.");
		exit(-1);
	}

	char fileName[80];
	char baseName[80]; sprintf(baseName, "%s", argv[1] );

	int firstBGF = atoi(argv[2]);
	int lastBGF = atoi(argv[3]);
	int stepBGF = atoi(argv[4]);

	int firstF = atoi(argv[5]);
	int lastF = atoi(argv[6]);
	int stepF = atoi(argv[7]);

	int iLevels = atoi(argv[8]);
	float loThL = atof(argv[9]);
	float hiThL = atof(argv[10]);
	float loThC = atof(argv[11]);
	float hiThC = atof(argv[12]);
	int minGap = atoi(argv[13]);
	int maxGap = atoi(argv[14]);
	int hGap   = atoi(argv[15]);

	sprintf(fileName, "%s.%05d.jpeg", baseName, firstF);
	frame = imread(fileName, 1);
	if (!frame.data) {
		printf("Could not open file %s.\n", fileName);
		return (-1);
	}

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
	int cont = firstBGF;
	int step = stepBGF;
	while ( continua && cont < lastBGF )
	{
#if MEDIAN_BGS
		bgs.AddMedianSample(pyr[iLevels]);
#else
		bgs.AddMeanSample(pyr[iLevels]);
#endif
		bgs.Show(bgs.outP, WAN, WBN, WLN );

		// load frame
		cont += step;
		if (cont >= lastBGF) break;

		sprintf(fileName, "%s.%05d.jpeg", baseName, cont);
		frame = imread(fileName, 1);
		if (!frame.data) {
			printf("Could not open file %s.\n", fileName);
			return (-1);
		}

		buildPyramid(frame, pyr, iLevels);
		imshow(WIN, pyr[iLevels]);

		if (waitKey(30) >= 0)
			continua = false;
	}
#if MEDIAN_BGS
	bgs.ComputeMedianBG();
#else
	bgs.ComputeMeanBG();
#endif
	bgs.Show(bgs.luvP, WAN, WBN, WLN );
	imshow(WIN, bgs.bgBGRImg);

	printf("Press q to quit or c to continue.\n");
	char k = waitKey();
	if (k == 'q' || k == 'Q')
		return 0;

	///////////////////////////////////////////////////////////////////////
	////
	//// Process the video
	////
    ///////////////////////////////////////////////////////////////////////


	// loop parameters
	Mat binImg;
	binImg = Mat (pyr[iLevels].size(), CV_8UC1);
	namedWindow(WBG, CV_WINDOW_AUTOSIZE );

	int notEnd = 1;
	char keyPressed;


	/// Video
	VideoWriter vw;
	vw.open("movie.mpeg", CV_FOURCC('P','I','M','1'), 25, pyr[iLevels].size());

	continua = true;
	for (cont = firstF; continua && cont<lastF; cont+= stepF) {

		// load image
		sprintf(fileName, "%s.%05d.jpeg", baseName, cont);
		frame = imread(fileName, 1);
		if (!frame.data) {
			printf("Could not open file %s.\n", fileName);
			return (-1);
		}

		buildPyramid(frame, pyr, iLevels);
		imshow(WIN, pyr[iLevels]);
#if MEDIAN_BGS
		bgs.BGSeg(pyr[iLevels], binImg);
		imshow(WBG, binImg);
		imshow(WAN, bgs.hiImg);
		imshow(WBN, bgs.loImg);
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
