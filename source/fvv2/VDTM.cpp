#include "VDTM.hpp"

VDTM::VDTM(ResourceData *scenarioData) {
  this->scenarioData = scenarioData;

  int numCameras = scenarioData->getIntValue("numCameras");

  for (int i = 0; i < numCameras; i++) {
    stringstream keyEye;
    keyEye << "camera" << (i + 1) << "Eye";

    vector<double> camEye = scenarioData->getRealListValue(keyEye.str(), ',');
    
    stringstream keyCenter;
    keyCenter << "camera" << (i + 1) << "Center";
    
    vector<double> camCenter = 
      scenarioData->getRealListValue(keyCenter.str(), ',');

    cameras.push_back(Vector(camCenter[0] - camEye[0],
			     camCenter[1] - camEye[1],
			     camCenter[2] - camEye[2]));
  }

  p = cvCreateMat(3, 1, CV_32FC1);
  r = cvCreateMat(3, 1, CV_32FC1);
  M = cvCreateMat(3, 3, CV_32FC1);
  Minv = cvCreateMat(3, 3, CV_32FC1);

  debug = false;
}

VDTM::~VDTM() {
  cvReleaseMat(&p);
  cvReleaseMat(&r);
  cvReleaseMat(&M);
  cvReleaseMat(&Minv);
}

vector<pair<int, double> > VDTM::calcWeights(Vector viewingDir,
					     Vector pos) {
  Vector origin = Vector(0.0, 0.0, 0.0);
  Vector right = Vector(1.0, 0.0, 0.0);
  Vector up = Vector(0.0, 0.0, 1.0);

  viewingDir = viewingDir.normalize();

  Vector projViewDir = Vector(viewingDir.x, viewingDir.y, 0.0);
  projViewDir = projViewDir.normalize();

  double rotation  = acos(projViewDir.dot(right));
  double elevation = asin(viewingDir.z / viewingDir.length());

  if (debug) {
    printf("%f\n", elevation);
  }

  if (projViewDir.y < 0.0) {
    rotation = (M_PI - rotation) + M_PI;
  }

  double angleDelta = (2.0 * M_PI / numSegments);
  int horizSection = rotation / angleDelta;
  int vertSection = elevation / angleDelta;
  if (elevation < 0) {
    vertSection = vertSection - 1;
  }

  Vector p0;
  Vector p1;
  Vector p2;
  Vector w;

  // Calculates the coordinates of the vertices of the triangle intersected
  // by the current viewing direction line
  if (vertSection % 2 == 0) {
    Vector va = calcSphereVertex(pos, horizSection - 1, vertSection + 1);
    Vector vb = calcSphereVertex(pos, horizSection, vertSection + 1);
    Vector vc = calcSphereVertex(pos, horizSection + 1, vertSection + 1);
    Vector vd = calcSphereVertex(pos, horizSection, vertSection);
    Vector ve = calcSphereVertex(pos, horizSection + 1, vertSection);

    Vector la = pos;
    Vector lb = pos.translate(viewingDir.mult(10.0));

    if (debug) {
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0, 1.0, 0.0);
      glBegin(GL_LINES);
      glVertex3f(va.x, va.y, va.z);
      glVertex3f(vd.x, vd.y, vd.z);
      glVertex3f(vd.x, vd.y, vd.z);
      glVertex3f(vb.x, vb.y, vb.z);
      glVertex3f(vb.x, vb.y, vb.z);
      glVertex3f(va.x, va.y, va.z);

      glVertex3f(vb.x, vb.y, vb.z);
      glVertex3f(vd.x, vd.y, vd.z);
      glVertex3f(vd.x, vd.y, vd.z);
      glVertex3f(ve.x, ve.y, ve.z);
      glVertex3f(ve.x, ve.y, ve.z);
      glVertex3f(vb.x, vb.y, vb.z);

      glVertex3f(vb.x, vb.y, vb.z);
      glVertex3f(ve.x, ve.y, ve.z);
      glVertex3f(ve.x, ve.y, ve.z);
      glVertex3f(vc.x, vc.y, vc.z);
      glVertex3f(vc.x, vc.y, vc.z);
      glVertex3f(vb.x, vb.y, vb.z);

      glVertex3f(la.x, la.y, la.z);
      glVertex3f(lb.x, lb.y, 0.0);
      glEnd();
      glEnable(GL_TEXTURE_2D);
    }

    w = linePlaneIntersection(la, lb, vb, vd, ve);
    if (w.y + w.z > 1.0 + EPSILON || w.y < -EPSILON || w.z < -EPSILON) {
      w = linePlaneIntersection(la, lb, va, vd, vb);
      if (w.y + w.z > 1.0 + EPSILON || w.y < -EPSILON || w.z < -EPSILON) {
	w = linePlaneIntersection(la, lb, vb, ve, vc); 

	p0 = vb;
	p1 = ve;
	p2 = vc;
      } else {
	p0 = va;
	p1 = vd;
	p2 = vb;
      }
    } else {
      p0 = vb;
      p1 = vd;
      p2 = ve;
    }

    if (debug) {
      glBegin(GL_TRIANGLES);
      glVertex3f(p0.x, p0.y, p0.z);
      glVertex3f(p1.x, p1.y, p1.z);
      glVertex3f(p2.x, p2.y, p2.z);
      glEnd();
    }

  } else {
    Vector va = calcSphereVertex(pos, horizSection, vertSection + 1);
    Vector vb = calcSphereVertex(pos, horizSection + 1, vertSection + 1);
    Vector vc = calcSphereVertex(pos, horizSection - 1, vertSection);
    Vector vd = calcSphereVertex(pos, horizSection, vertSection);
    Vector ve = calcSphereVertex(pos, horizSection + 1, vertSection);

    if (debug) {
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0, 1.0, 1.0);
      glBegin(GL_LINES);
      glVertex3f(va.x, va.y, va.z);
      glVertex3f(vd.x, vd.y, vd.z);
      glVertex3f(vd.x, vd.y, vd.z);
      glVertex3f(vb.x, vb.y, vb.z);
      glVertex3f(vb.x, vb.y, vb.z);
      glVertex3f(va.x, va.y, va.z);

      glVertex3f(va.x, va.y, va.z);
      glVertex3f(vc.x, vc.y, vc.z);
      glVertex3f(vc.x, vc.y, vc.z);
      glVertex3f(vd.x, vd.y, vd.z);
      glVertex3f(vd.x, vd.y, vd.z);
      glVertex3f(va.x, va.y, va.z);

      glVertex3f(vb.x, vb.y, vb.z);
      glVertex3f(vd.x, vd.y, vd.z);
      glVertex3f(vd.x, vd.y, vd.z);
      glVertex3f(ve.x, ve.y, ve.z);
      glVertex3f(ve.x, ve.y, ve.z);
      glVertex3f(vb.x, vb.y, vb.z);
      glEnd();
      glEnable(GL_TEXTURE_2D);
    }

    Vector la = pos;
    Vector lb = pos.translate(viewingDir.mult(10.0));

    w = linePlaneIntersection(la, lb, va, vd, vb);
    if (w.y + w.z > 1.0 + EPSILON || w.y < -EPSILON || w.z < -EPSILON) {
      w = linePlaneIntersection(la, lb, va, vc, vd);
      if (w.y + w.z > 1.0 + EPSILON || w.y < -EPSILON || w.z < -EPSILON) {
	w = linePlaneIntersection(la, lb, vb, vd, ve); 

	p0 = vb;
	p1 = vd;
	p2 = ve;
      } else {
	p0 = va;
	p1 = vc;
	p2 = vd;
      }
    } else {
      p0 = va;
      p1 = vd;
      p2 = vb;
    }

    if (debug) {
      glBegin(GL_TRIANGLES);
      glVertex3f(p0.x, p0.y, p0.z);
      glVertex3f(p1.x, p1.y, p1.z);
      glVertex3f(p2.x, p2.y, p2.z);
      glEnd();
    }
  }

  if (debug) {
    printf("p0 (%.5f, %.5f, %.5f)\n", p0.x, p0.y, p0.z);
    printf("p1 (%.5f, %.5f, %.5f)\n", p1.x, p1.y, p1.z);
    printf("p2 (%.5f, %.5f, %.5f)\n", p2.x, p2.y, p2.z);
  }

  if (w.y > 1.0) {
    w.y = 1.0;
  }
  if (w.y < 0.0) {
    w.y = 0.0;
  }
  if (w.z > 1.0) {
    w.z = 1.0;
  }
  if (w.z < 0.0) {
    w.z = 0.0;
  }

  Vector vertexDir[3];

  vertexDir[0] = Vector(pos.x - p0.x,
			pos.y - p0.y,
			pos.z - p0.z);
  vertexDir[1] = Vector(pos.x - p1.x,
			pos.y - p1.y,
			pos.z - p1.z);
  vertexDir[2] = Vector(pos.x - p2.x,
			pos.y - p2.y,
			pos.z - p2.z);

  // Calculates which camera has the best view for each of the vertices of
  // triangle that contains the current viewing direction
  if (debug) {
    printf("Selecting cameras\n");
  }
  int selCameras[3];
  for (int i = 0; i < 3; i++) {
    selCameras[i] = getNearestCamera(vertexDir[i]);
    if (debug) {
      printf("Selected camera %d\n", selCameras[i]);
    }
  }
  if (debug) {
    printf("\n");
  }
  
  // Alpha is the blending weight for the camera corresponding to the vertex 0
  // of the triangle. w.y and w.z are weights for vertex 1 and vertex 2
  // respectively
  double alpha = 1.0 - w.y - w.z;

  // Accumulates weights for vertices that share the same camera
  vector<pair<int, double> > camsToRender;
  camsToRender.push_back(make_pair(selCameras[0], alpha));
  if (selCameras[1] == camsToRender[0].first) {
    camsToRender[0].second += w.y;
  } else {
    camsToRender.push_back(make_pair(selCameras[1], w.y));
  }
  
  if (selCameras[2] == camsToRender[0].first) {
    camsToRender[0].second += w.z;
  } else if (camsToRender.size() > 1) {
    if (selCameras[2] == camsToRender[1].first) {
      camsToRender[1].second += w.z;      
    }
  } else {
    camsToRender.push_back(make_pair(selCameras[2], w.z));
  }

  sort(camsToRender.begin(), camsToRender.end());

  if (debug) {
    for (int i = 0; i < camsToRender.size(); i++) {
      printf("Camera %d weight: %.5f\n", camsToRender[i].first, 
	     camsToRender[i].second);
    }
  }

  return camsToRender;
}

Vector VDTM::calcSphereVertex(Vector center, int horizSection, 
			      int vertSection) {
  int hS = (horizSection + numSegments) % numSegments;
  int vS = vertSection;

  double dAngle = 2.0 * M_PI / numSegments;

  double hAngle = 0.0;
  double vAngle = 0.0;

  if (vS % 2 == 0) {
    hAngle = hS * dAngle;
  } else {
    hAngle = (hS * dAngle) + 0.5 * dAngle;
  }
  vAngle = vS * dAngle;

  Vector result = Vector(1.0, 0.0, 0.0);
  result = Vector(result.x * cos(-vAngle) + result.z * sin(-vAngle),
		  result.y,
		  result.z * cos(-vAngle) - result.x * sin(-vAngle)); 

  result = Vector(result.x * cos(hAngle) - result.y * sin(hAngle),
		  result.x * sin(hAngle) + result.y * cos(hAngle),
		  result.z);

  result = result.translate(center);

  return result;
}

Vector VDTM::linePlaneIntersection(Vector la, Vector lb,
				   Vector p0, Vector p1, Vector p2) {
  cvmSet(M, 0, 0, la.x - lb.x);
  cvmSet(M, 1, 0, la.y - lb.y);
  cvmSet(M, 2, 0, la.z - lb.z);
  cvmSet(M, 0, 1, p1.x - p0.x);
  cvmSet(M, 1, 1, p1.y - p0.y);
  cvmSet(M, 2, 1, p1.z - p0.z);
  cvmSet(M, 0, 2, p2.x - p0.x);
  cvmSet(M, 1, 2, p2.y - p0.y);
  cvmSet(M, 2, 2, p2.z - p0.z);
  cvInvert(M, Minv);

  cvmSet(p, 0, 0, la.x - p0.x);
  cvmSet(p, 1, 0, la.y - p0.y);
  cvmSet(p, 2, 0, la.z - p0.z);
  cvMatMul(Minv, p, r);

  Vector result;

  result.x = cvmGet(r, 0, 0);
  result.y = cvmGet(r, 1, 0);
  result.z = cvmGet(r, 2, 0);

  return result;
}

int VDTM::getNearestCamera(Vector p) {
  int result = 0;
  vector<pair<double, int> > sortedCameras;

  p = p.normalize();
  for (int i = 0; i < cameras.size(); i++) {
    Vector dir = cameras[i];
    dir = dir.normalize();

    double dot = dir.dot(p);
    if (dot >= 0.0) {
      sortedCameras.push_back(make_pair(acos(dot), i));
    }
  }

  if (sortedCameras.size() > 0) {
    sort(sortedCameras.begin(), sortedCameras.end());
    result = sortedCameras[0].second;
  }
  
  return result;
}

