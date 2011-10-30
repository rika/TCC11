#ifndef _INC_BILLBOARDING_H
#define _INC_BILLBOARDING_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <cv.h>
#include <highgui.h>

#include <iostream>
#include <fstream>
#include <sstream> 
#include <vector>
#include <string>

#include "Vector.hpp"
#include "Camera.hpp"
#include "ResourceData.h"
#include "ResourceParser.h"
#include "Object.hpp"
#include "TextureManager.hpp"
#include "VDTM.hpp"

using namespace std;

const int maxCameras = 255;

class Billboarding {
public:
  Billboarding(TextureManager *texManager, 
	       ResourceData *scenarioData,
	       VDTM *vdtm);
  virtual ~Billboarding();

  void setup();
  void setViewpoint(Vector eye, Vector center);
  void renderFrame(int frame);
  void toggleDummyRender();
  void toggleForegroundMask();
  void renderVideoThumbs();
  
private:
  VDTM *vdtm;
  TextureManager *texManager;
  ResourceData *scenarioData;

  bool dummyRender;
  bool foregroundMask;

  double winWidth;
  double winHeight;

  CvMat *p;
  CvMat *r;
  CvMat *M;
  CvMat *Minv;
  ResourceParser resourceParser;

  vector<Vector> cameras;
  Camera camera;

  vector<Object> objects;

  vector<CvMat *> refToCam;
  vector<CvMat *> camToRef;
  CvMat *refToWorld;
  vector<vector<vector<double> > > correctionMat;

  CvMat *calculateHomography(const vector<pair<double, double> > &srcPoints, 
			     const vector<pair<double, double> > &destPoints);
  void setupBillboard(Vector object);
  void setupBillboardFalse(Vector object);
  void loadObjectData(int frame);
  void loadCorrectionMats();
  CvMat *loadHomography(string path);
  void renderBillboard(Vector a, Vector b, Vector c, Vector d,
		       Object object, int cam);
  void renderObjectDummy(Object object);
  void renderObject(Object object);
  void addCamera(Vector camera);
  void setupHomographies();
};

#endif
