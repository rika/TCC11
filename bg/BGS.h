/*

  some ideas from "real time adaptive background segmentation"
Dareren Butler, Shridha Sridharam, V. Michael Bove
how to learn and model background?
mixture of gaussians (clusters) ordered according to the likelihood
adapted to deal with background and lighting variations

 */


#ifndef __BGS_H
#define __BGS_H

#include <stdio.h>
#include "cv.h"
#include "highgui.h"

using namespace std;
using namespace cv;

#define ALPHA 100

// 
#define ABSDIFF(a,b) (((a) >( b)) ? ((a) - (b)) : ((b) - (a)))
//#define MAX(a,b) ((a)>(b) ? (a) : (b))
/// Background subtraction

class BGS
{
 public:
  BGS(void) {};
  ~BGS(void) {};
  
  void Init(Mat& inBGR);

  void Normalize(const Mat& inBGR, Mat& out);
  void AddMeanSample (Mat& inBGR);
  void AddMedianSample (Mat& inBGR);
  void ComputeMeanBG();
  void ComputeMedianBG();
  void Subtract (Mat& inBGR, Mat& imgResult);
  void BGSegR (Mat& inBGR, Mat& outBGR);
  void BGSeg (Mat& inBGR, Mat& outBGR);
  void FillGaps(Mat& in, Mat& out, int minlen, int maxlen);

  void SetThresholdC(float lo, float hi = 1.0) { loThC = lo; hiThC = hi; };
  void SetThresholdL(float lo, float hi = 1.0) { loThL = lo; hiThL = hi; };
  void SetMinMaxGap(int min, int max) { iMinGap = min; iMaxGap = max; };
  void SetHGap (int h) {hGap = h;};

  void Show (vector<Mat> imgN, char *w1, char *w2, char *w3) {
    imshow ( w1, imgN[0] );
    imshow ( w2, imgN[1] );
    imshow ( w3, imgN[2] );
  };

  void PrintPars () {
    printf ("Thresholds: loL= %5.3f hiL= %5.3f loC= %5.3f hiC= %5.3f\n", loThL, hiThL, loThC, hiThC);
  };
  // private:
  float loThL, loThC;
  float hiThL, hiThC;
  int iMinGap, iMaxGap, hGap;

  Mat bgmImg;
  Mat normImg;
  Mat tempImg;
  Mat ldifImg;
  Mat cdifImg;
  Mat bgBGRImg;
  Mat auxImg;
  Mat loImg;
  Mat hiImg;

  vector<Mat> outP;
  vector<Mat> inP;
  vector<Mat> luvP;

  vector<Mat> lSamples;
  vector<Mat> uSamples;
  vector<Mat> vSamples;

  int iCount;
};


#endif
