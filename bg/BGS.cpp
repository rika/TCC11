/*
 Background Subtraction

 Hitoshi
 07/05/2010

 OpenCV 2.1
 */

#include "BGS.h"
#define COLOR_FG1 255
#define COLOR_FG2 130
#define COLOR_BG  0

#if 0
#define TO_COLOR_SPC CV_BGR2Lab
#define FROM_COLOR_SPC CV_Lab2BGR
#else
#define TO_COLOR_SPC CV_BGR2YCrCb
#define FROM_COLOR_SPC CV_YCrCb2BGR
#endif
// CV_Luv2BGRlu

/*
 ************************************************************
 Init

 */

void BGS::Init(Mat& inBGR) {
	Size s = inBGR.size();

	bgBGRImg = Mat(s, inBGR.type()); // stores Background image in BGR format
	auxImg = Mat(s, inBGR.type());
	loImg = Mat(s, CV_8UC1);
	hiImg = Mat(s, CV_8UC1);

	bgmImg = Mat(s, CV_32FC3); // mean bg image in Luv
	normImg = Mat(s, CV_32FC3);
	tempImg = Mat(s, CV_32FC3);
	cdifImg = Mat(s, CV_32FC3);

	ldifImg = Mat(s, CV_32FC1);

	// weights to update background images
	// high ALPHA => slow learning
	iCount = 0;
}

/*
 ************************************************************
 Normalize
 OpenCV channel order is Blue Green Red

 */

void BGS::Normalize(const Mat& inBGR, Mat& out) {
	Mat hsv(inBGR);

	cvtColor(inBGR, hsv, TO_COLOR_SPC);
	split(hsv, inP);
	split(out, outP);
	for (int row = 0; row < inP[0].rows; row++)
		for (int col = 0; col < inP[0].cols; col++) {
			outP[2].at<float> (row, col) = inP[2].at<uchar> (row, col) / 255.0;
			outP[1].at<float> (row, col) = inP[1].at<uchar> (row, col) / 255.0;
			outP[0].at<float> (row, col) = inP[0].at<uchar> (row, col) / 255.0;
#if DEBUG_BGS
			printf(" %d %d %d %d %d %5.3f %5.3f %5.3f\n", row, col,
					inP[0].at<uchar>(row,col), inP[1].at<uchar>(row,col), inP[2].at<uchar>(row,col),
					outP[0].at<float>(row,col), outP[1].at<float>(row,col), outP[2].at<float>(row,col));
#endif
		}
	merge(outP, out);
}

/*
 ************************************************************
 *
 *  Learn Mean Background
 *
 ************************************************************
 */

void BGS::AddMeanSample(Mat& inBGR) {
	iCount++;
	GaussianBlur(inBGR, inBGR, Size(5, 5), 1.0, 1.0);
	// medianBlur (inBGR, inBGR, 3);
	Normalize(inBGR, normImg);
	bgmImg += normImg;
}

void BGS::ComputeMeanBG() {
	bgmImg *= 1.0 / iCount; // normalized Luv background mean image
	float scale = 255.0; // back to uchar

	split(bgBGRImg, inP);
	split(bgmImg, luvP);

	for (int row = 0; row < inP[0].rows; row++)
		for (int col = 0; col < inP[0].cols; col++) {
			inP[0].at<uchar> (row, col) = saturate_cast<uchar> (luvP[0].at<
					float> (row, col) * scale);
			inP[1].at<uchar> (row, col) = saturate_cast<uchar> (luvP[1].at<
					float> (row, col) * scale);
			inP[2].at<uchar> (row, col) = saturate_cast<uchar> (luvP[2].at<
					float> (row, col) * scale);
#if DEBGU_BGS
			printf(" %d %d %d %d %d %5.3f %5.3f %5.3f %5.3f %d\n", row, col,
					inP[0].at<uchar>(row,col), inP[1].at<uchar>(row,col), inP[2].at<uchar>(row,col),
					luvP[0].at<float>(row,col), luvP[1].at<float>(row,col), luvP[2].at<float>(row,col),
					scale, iCount);
#endif
		}
	// to avoid division by zero
	luvP[0] += 1.0 / 255;
	merge(inP, bgBGRImg);
	cvtColor(bgBGRImg, bgBGRImg, FROM_COLOR_SPC);
#if 0
	printf("Finish: frames: %d  chan = %d  depth = %d \n", iCount,
			bgBGRImg.channels(), bgBGRImg.depth());
#endif

}

/*
 * ***************************************************************************** *
 *  Learn Median Background
 * ***************************************************************************** *
 */

void BGS::AddMedianSample(Mat& inBGR) {
	iCount++;
	GaussianBlur(inBGR, inBGR, Size(5, 5), 1.0, 1.0);
	// medianBlur (inBGR, inBGR, 3);
	Normalize(inBGR, normImg);

	lSamples.push_back(outP[0].clone());
	uSamples.push_back(outP[1].clone());
	vSamples.push_back(outP[2].clone());
}

void BGS::ComputeMedianBG() {
	int row, col, s;
	int cont = 0;
	bgmImg *= 0.0;
	vector<float> l;
	vector<float> u;
	vector<float> v;

	split(bgmImg, luvP);

	for (row = 0; row < bgmImg.rows; row++) {
		for (col = 0; col < bgmImg.cols; col++) {
			l.clear();
			u.clear();
			v.clear();
			for (s = 0; s < iCount; s++) {
				l.push_back(lSamples[s].at<float> (row, col));
				u.push_back(uSamples[s].at<float> (row, col));
				v.push_back(vSamples[s].at<float> (row, col));
			}
			sort(l.begin(), l.end());
			sort(u.begin(), u.end());
			sort(v.begin(), v.end());
			luvP[0].at<float> (row, col) = l[l.size() / 2];
			luvP[1].at<float> (row, col) = u[l.size() / 2];
			luvP[2].at<float> (row, col) = v[l.size() / 2];
#if DEBUG_BGS
			for (s = 0; s < l.size(); s++)
			printf(" %.3f ", l[s]);
			printf("\n %d %d %d %.3f \n", row, col, l.size(), l[l.size()/2]);
#endif
		}
	}

	float scale = 255.0; // back to uchar
	split(bgBGRImg, inP);

	for (int row = 0; row < inP[0].rows; row++)
		for (int col = 0; col < inP[0].cols; col++) {
			inP[0].at<uchar> (row, col) = saturate_cast<uchar> (luvP[0].at<
					float> (row, col) * scale);
			inP[1].at<uchar> (row, col) = saturate_cast<uchar> (luvP[1].at<
					float> (row, col) * scale);
			inP[2].at<uchar> (row, col) = saturate_cast<uchar> (luvP[2].at<
					float> (row, col) * scale);
#if DEBGU_BGS
			printf(" %d %d %d %d %d %5.3f %5.3f %5.3f %5.3f %d\n", row, col,
					inP[0].at<uchar>(row,col), inP[1].at<uchar>(row,col), inP[2].at<uchar>(row,col),
					luvP[0].at<float>(row,col), luvP[1].at<float>(row,col), luvP[2].at<float>(row,col),
					scale, iCount);
#endif
		}
	// to avoid division by zero
	luvP[0] += 1.0 / 255;
	merge(inP, bgBGRImg);
	merge(luvP, bgmImg);
	cvtColor(bgBGRImg, bgBGRImg, FROM_COLOR_SPC);
#if 0
	printf("Finish: frames: %d  chan = %d  depth = %d \n", cont,
			bgBGRImg.channels(), bgBGRImg.depth());
#endif
	lSamples.clear();
	uSamples.clear();
	vSamples.clear();
}

/*
 ************************************************************
 Subtract
 Uses the background model stored at
 bgmImg : mean Luv
 luvP   : Luv planes of bgmImg

 */

void BGS::Subtract(Mat& inBGR, Mat& imgResult) {
	Mat aux(inBGR);
	const uchar foreG = COLOR_FG1;
	const uchar backG = COLOR_BG;
	float s = loThC * loThC;
	uchar r;

	GaussianBlur(inBGR, inBGR, Size(5, 5), 1.0, 1.0);
	//	GaussianBlur(inBGR, inBGR, Size(3,3), 0.5, 0.5);

	Normalize(inBGR, normImg); // planes in Luv format in outP
	ldifImg = outP[0] / luvP[0];
	cdifImg = normImg - bgmImg;
	cdifImg = cdifImg.mul(cdifImg);

#if 1
	GaussianBlur(ldifImg, ldifImg, Size(3, 7), 1.5, 1.5);
	GaussianBlur(cdifImg, cdifImg, Size(3, 7), 1.5, 1.5);
#endif

	split(cdifImg, outP);

	for (int row = 0; row < inBGR.rows; row++) {
		float *dL = ldifImg.ptr<float> (row);
		float *d2u = outP[1].ptr<float> (row);
		float *d2v = outP[2].ptr<float> (row);
		uchar *res = aux.ptr<uchar> (row);

		for (int col = 0; col < inBGR.cols; col++) {
			r = backG;
			if (dL[col] > 1 + loThL || dL[col] < 1 - loThL)
				r = foreG;
			else if (d2u[col] + d2v[col] > s)
				r = foreG;
			// else if (*dL < fThS)  // detect shadow areas
			*res++ = r;
		}
	}
	imgResult *= 0;
	FillGaps(aux, imgResult, iMinGap, iMaxGap);
	//	UpdateBackground();
#if 0
	dilate(imgResult, imgResult, Mat(), Point(-1,-1), 2);
	erode(imgResult, imgResult, Mat(), Point(-1,-1), 2);
	//cvDilate( imgResult, imgResult, NULL, 1);
#endif
}

void BGS::FillGaps(Mat& inBGR, Mat& outBGR, int minlen, int maxlen) {
	for (int row = 0; row < inBGR.rows - maxlen; row++) {
		for (int col = 0; col < inBGR.cols; col++) {
			for (int len = minlen; len < maxlen; len++)
				if (inBGR.at<uchar> (row, col) > 0 && inBGR.at<uchar> (row
						+ len - 1, col) > 0) {
					for (int k = 0; k < len; k++)
						outBGR.at<uchar> (row + k, col) = 100;
					len = maxlen;
				}
		}
	}
#if 1
	for (int row = 2; row < inBGR.rows - 2; row++) {
		for (int col = 2; col < inBGR.cols - 2; col++) {
			if (outBGR.at<uchar> (row, col) > 0) {
				for (int i = -2; i <= 2; i++)
					for (int j = -2; j <= 2; j++)
						if (outBGR.at<uchar> (row + i, col + j) == 0
								&& inBGR.at<uchar> (row + i, col + j) > 0)
							outBGR.at<uchar> (row + i, col + j) = 255;
			}
		}
	}
#endif
}

/*
 ************************************************************
 BGSegR
 Uses the background model stored at
 bgmImg : mean Luv
 luvP   : Luv planes of bgmImg

 vertical pixels nearby a hi-threshold, higher than a low-threshold,
 are grouped.

 */

void BGS::BGSeg(Mat& inBGR, Mat& outBGR)
{
	const uchar foreG = COLOR_FG1;
	const uchar backG = COLOR_BG;
	float c2H = hiThC * hiThC;
	float c2L = loThC * loThC;
	uchar h, l;

	GaussianBlur(inBGR, inBGR, Size(5, 5), 1.0, 1.0);
	Normalize(inBGR, normImg); // planes in Luv format in outP
	ldifImg = outP[0] / luvP[0];
	cdifImg = normImg - bgmImg;
	cdifImg = cdifImg.mul(cdifImg);
	split(cdifImg, outP);

	GaussianBlur(ldifImg, ldifImg, Size(7,7), 3, 3);
	GaussianBlur(outP[1], outP[1], Size(7,7), 3, 3);
	GaussianBlur(outP[2], outP[2], Size(7,7), 3, 3);

	/// Compute low and high thresholded images
	for (int row = 0; row < inBGR.rows; row++) {
		float *dL = ldifImg.ptr<float> (row);
		float *d2u = outP[1].ptr<float> (row);
		float *d2v = outP[2].ptr<float> (row);
		uchar *hPtr = hiImg.ptr<uchar> (row);
		uchar *lPtr = loImg.ptr<uchar> (row);

		for (int col = 0; col < inBGR.cols; col++) {
			h = backG;
			if (dL[col] > 1 + hiThL || dL[col] < 1 - hiThL)
				h = foreG;
			else if (d2u[col] + d2v[col] > c2H)
				h = foreG;
			// else if (*dL < fThS)  // detect shadow areas
			*hPtr++ = h;

			l = backG;
			if (dL[col] > 1 + loThL || dL[col] < 1 - loThL)
				l = foreG;
			else if (d2u[col] + d2v[col] > c2L)
				l = foreG;
			// else if (*dL < fThS)  // detect shadow areas
			*lPtr++ = l;

		}
	}

	// fill gaps
	outBGR *= 0.0;
	auxImg *= 0.0;

	for (int row = 0; row < outBGR.rows - iMaxGap; row++) {
		for (int col = 0; col < outBGR.cols; col++) {
			for (int len = iMinGap; len < iMaxGap; len++)
				if ((loImg.at<uchar> (row, col) > 0 &&
					 hiImg.at<uchar> (row + len - 1, col) > 0) ||
					(hiImg.at<uchar> (row, col) > 0 &&
					 loImg.at<uchar> (row + len - 1, col) > 0)) {
					for (int k = 0; k < len; k++)
						auxImg.at<uchar> (row + k, col) = COLOR_FG1;
					len = iMaxGap;
				}
		}
	}

	int w = hGap;
	for (int row = w; row < outBGR.rows - w; row++) {
		for (int col = w; col < outBGR.cols - w; col++) {
			if (auxImg.at<uchar> (row, col) > 0) {
				outBGR.at<uchar> (row, col) = COLOR_FG1;
				for (int len = 1; len < w; len++)
					if (auxImg.at<uchar> (row, col+len) > 0) {
						for (int k = 0; k<len; k++)
							auxImg.at<uchar> (row, col+k) = COLOR_FG1;
						len = w;
				}
				for (int i = -w; i <= w; i++)
					for (int j = -w; j <= w; j++)
						if (auxImg.at<uchar> (row + i, col + j) == 0
								&& loImg.at<uchar> (row + i, col + j) > 0)
							outBGR.at<uchar> (row + i, col + j) = COLOR_FG2;
			}
		}
	}

#if 0
	erode(outBGR, outBGR, Mat() );
//	dilate(outBGR, outBGR, Mat() );
	//cvDilate( outBGR, outBGR, NULL, 1);
#endif

}

/*
 ************************************************************
 BGSeg
 Uses the background model stored at
 bgmImg : mean Luv
 luvP   : Luv planes of bgmImg

 vertical pixels nearby a hi-threshold, higher than a low-threshold,
 are grouped.

 */

void BGS::BGSegR(Mat& inBGR, Mat& outBGR)
{
	const uchar foreG = COLOR_FG1;
	const uchar backG = COLOR_BG;
	float c2H = hiThC * hiThC;
	float c2L = loThC * loThC;
	uchar h, l;

	GaussianBlur(inBGR, inBGR, Size(5, 5), 1.0, 1.0);
	Normalize(inBGR, normImg); // planes in Luv format in outP
	divide (normImg,bgmImg, cdifImg);
//	cdifImg = cdifImg.mul(cdifImg);
//	GaussianBlur(cdifImg, cdifImg, Size(7,7), 1.5, 3);
	split(cdifImg, outP);

	/// Compute low and high thresholded images
	for (int row = 0; row < inBGR.rows; row++) {
		float *dL =  outP[0].ptr<float> (row);
		float *d2u = outP[1].ptr<float> (row);
		float *d2v = outP[2].ptr<float> (row);
		uchar *hPtr = hiImg.ptr<uchar> (row);
		uchar *lPtr = loImg.ptr<uchar> (row);

		for (int col = 0; col < inBGR.cols; col++) {
			h = backG;
			if (dL[col] > 1 + hiThL || dL[col] < 1 - hiThL ||
				d2u[col] > 1 + hiThC || d2u[col] < 1 - hiThC ||
				d2v[col] > 1 + hiThC || d2v[col] < 1 - hiThC)
				h = foreG;
			// else if (*dL < fThS)  // detect shadow areas
			*hPtr++ = h;

			l = backG;
			if (dL[col] > 1 + loThL || dL[col] < 1 - loThL ||
				d2u[col] > 1 + loThC || d2u[col] < 1 - loThC ||
				d2v[col] > 1 + loThC || d2v[col] < 1 - loThC)
				l = foreG;
			*lPtr++ = l;
		}
	}

	// fill gaps
	outBGR *= 0.0;
	auxImg *= 0.0;

	for (int row = 0; row < outBGR.rows - iMaxGap; row++) {
		for (int col = 0; col < outBGR.cols; col++) {
			for (int len = iMinGap; len < iMaxGap; len++)
				if ((loImg.at<uchar> (row, col) > 0 &&
					 hiImg.at<uchar> (row + len - 1, col) > 0) ||
					(hiImg.at<uchar> (row, col) > 0 &&
					 loImg.at<uchar> (row + len - 1, col) > 0)) {
					for (int k = 0; k < len; k++)
						auxImg.at<uchar> (row + k, col) = COLOR_FG1;
					len = iMaxGap;
				}
		}
	}

	int w = hGap;
	for (int row = w; row < outBGR.rows - w; row++) {
		for (int col = w; col < outBGR.cols - w; col++) {
			if (auxImg.at<uchar> (row, col) > 0) {
				outBGR.at<uchar> (row, col) = COLOR_FG1;
				for (int len = 1; len < w; len++)
					if (auxImg.at<uchar> (row, col+len) > 0) {
						for (int k = 0; k<len; k++)
							auxImg.at<uchar> (row, col+k) = COLOR_FG1;
						len = w;
				}
				for (int i = -w; i <= w; i++)
					for (int j = -w; j <= w; j++)
						if (auxImg.at<uchar> (row + i, col + j) == 0
								&& loImg.at<uchar> (row + i, col + j) > 0)
							outBGR.at<uchar> (row + i, col + j) = COLOR_FG2;
			}
		}
	}

#if 0
	erode(outBGR, outBGR, Mat() );
//	dilate(outBGR, outBGR, Mat() );
	//cvDilate( outBGR, outBGR, NULL, 1);
#endif

}
