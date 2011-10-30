#ifndef _INC_VDTM_HPP
#define _INC_VDTM_HPP

#include "Vector.hpp"
#include "ResourceData.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <cv.h>
#include <highgui.h>

#include <vector>
#include <map>
#include <sstream>

using namespace std;

const int numSegments = 16;
const int EPSILON = 1.0e-5;

class VDTM {
private:
  ResourceData *scenarioData;
  vector<Vector> cameras;

  CvMat *p;
  CvMat *r;
  CvMat *M;
  CvMat *Minv;

  Vector calcSphereVertex(Vector center, int horizSection, 
			  int vertSection);
  Vector linePlaneIntersection(Vector la, Vector lb,
			       Vector p0, Vector p1, Vector p2);
  int getNearestCamera(Vector p);
  
public:
  VDTM(ResourceData *scenarioData);
  ~VDTM();

  bool debug;

  vector<pair<int, double> > calcWeights(Vector viewingDir,
					 Vector pos);
};

#endif
