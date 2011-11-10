#include "Billboarding.hpp"

#include <algorithm>
#include <fstream>

using namespace std;

Billboarding::Billboarding(TextureManager *texManager,
			   ResourceData *scenarioData,
			   VDTM *vdtm,
               ObjectData *objectData) {
  this->texManager = texManager;
  this->scenarioData = scenarioData;
  this->vdtm = vdtm;
  this->objectData = objectData;

  dummyRender = true;
  foregroundMask = false;

  visibleSubjects.clear();
  for (int i = 0; i < 100; i++) {
    visibleSubjects[i] = true;
  }

  winWidth  = scenarioData->getRealValue("videoFrameWidth");
  winHeight = scenarioData->getRealValue("videoFrameHeight");

  int numCameras = scenarioData->getIntValue("numCameras");

  // Load homographies from reference plane to cameras
  for (int i = 1; i <= numCameras; i++) {
    stringstream key;
    key << "refToCam" << i;
    string path = scenarioData->getStringValue(key.str());

    CvMat *homography = loadHomography(path);
    
    refToCam.push_back(homography);
  }

  // Calculates homographies from each camera to reference plane
  for (int i = 0; i < numCameras; i++) {
    CvMat *homography = cvCreateMat(3, 3, CV_32FC1);
    cvInvert(refToCam[i], homography);

    camToRef.push_back(homography);
  }

  p = cvCreateMat(3, 1, CV_32FC1);
  r = cvCreateMat(3, 1, CV_32FC1);
  M = cvCreateMat(3, 3, CV_32FC1);
  Minv = cvCreateMat(3, 3, CV_32FC1);
}

Billboarding::~Billboarding() {
  for (int i = 0; i < 3; i++) {
    cvReleaseMat(&refToCam[i]);
    cvReleaseMat(&camToRef[i]);
  }
  
  cvReleaseMat(&refToWorld);

  cvReleaseMat(&p);
  cvReleaseMat(&r);
  cvReleaseMat(&M);
  cvReleaseMat(&Minv);
}

void Billboarding::toggleDummyRender() {
  this->dummyRender = !this->dummyRender;
}

void Billboarding::toggleForegroundMask() {
  this->foregroundMask = !this->foregroundMask;
}

void Billboarding::setup() {
  setupHomographies();

  int numCameras = scenarioData->getIntValue("numCameras");
  for (int k = 1; k <= numCameras; k++) {
    stringstream eyeKey;
    eyeKey << "camera" << k << "Eye";

    stringstream centerKey;
    centerKey << "camera" << k << "Center";
    
    vector<double> camEye = 
      scenarioData->getRealListValue(eyeKey.str(), ',');
    vector<double> camCenter = 
      scenarioData->getRealListValue(centerKey.str(), ',');

    Vector camera = Vector(camCenter[0] - camEye[0],
			   camCenter[1] - camEye[1],
			   camCenter[2] - camEye[2]);
    printf("Adding camera (%f, %f, %f)\n", camera.x, camera.y, camera.z); 
    addCamera(camera);
  }
}

void Billboarding::loadCorrectionMats() {
  printf("Loading correction mats...\n");
  
  ifstream file;
  int matWidth  = scenarioData->getRealValue("videoFrameWidth");
  int matHeight = scenarioData->getRealValue("videoFrameHeight");

  double reduction = scenarioData->getRealValue("cmReductionFactor");
  matWidth = matWidth / reduction;
  matHeight = matHeight / reduction;

  int numCameras = scenarioData->getIntValue("numCameras");

  for (int k = 0; k < numCameras; k++) {
    vector<vector<double> > mat;
    for (int i = 0; i < matHeight; i++) {
      vector<double> columns(matWidth, 0.0);
     
      mat.push_back(columns);
    }
    correctionMat.push_back(mat);
  }

  for (int k = 0; k < numCameras; k++) {
    stringstream key;
    key << "correctionMat" << (k + 1);
    string path = scenarioData->getStringValue(key.str());

    printf("Loading correction mat %s\n", path.c_str());
    file.open(path.c_str());
    for (int i = 0; i < matHeight; i++) {
      for (int j = 0; j < matWidth; j++) {
	file >> correctionMat[k][i][j];
      }
    }
    file.close();
  }
}

CvMat *Billboarding::loadHomography(string path) {
  printf("Loading homography %s\n", path.c_str());

  CvMat *homography = cvCreateMat(3, 3, CV_32FC1);

  ifstream file;

  file.open(path.c_str());
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      double value;
      file >> value;
      cvmSet(homography, i, j, value);
    }
  }
  file.close();

  return homography;
}

void Billboarding::setupHomographies() {
  vector<pair<double, double> > camPoints;
  vector<pair<double, double> > refPoints;
  vector<pair<double, double> > worldPoints;

  int whichCamera = scenarioData->getIntValue("whichCamera");
  for (int i = 0; i < 4; i++) {
    stringstream key;
    key << "p" << i << "camera";

    vector<double> point = scenarioData->getRealListValue(key.str(), ',');
    camPoints.push_back(make_pair(point[0], point[1]));
  }

  for (int i = 0; i < 4; i++) {
    stringstream key;
    key << "p" << i << "world";

    vector<double> point = scenarioData->getRealListValue(key.str(), ',');
    worldPoints.push_back(make_pair(point[0], point[1]));
  }

  for (int i = 0; i < camPoints.size(); i++) {
    cvmSet(p, 0, 0, camPoints[i].first);
    cvmSet(p, 1, 0, camPoints[i].second);
    cvmSet(p, 2, 0, 1.0);

    cvMatMul(camToRef[whichCamera], p, r);

    refPoints.push_back(make_pair(r->data.fl[0] / r->data.fl[2], 
				  r->data.fl[1] / r->data.fl[2]));
  }

  // Calculates homography from reference plane to world coordinates
  refToWorld = calculateHomography(refPoints, worldPoints);

  loadCorrectionMats();
}

CvMat *Billboarding::calculateHomography(const vector<pair<double, double> > &srcPoints, const vector<pair<double, double> > &destPoints) {
  unsigned int numPoints = srcPoints.size();

  CvMat *x1 = cvCreateMat(1, numPoints, CV_32FC2);
  CvMat *x2 = cvCreateMat(1, numPoints, CV_32FC2);
  
  for (unsigned int i = 0; i < numPoints; i++) {
    x1->data.fl[i*2] = srcPoints[i].first;
    x1->data.fl[i*2+1] = srcPoints[i].second;
    x2->data.fl[i*2] = destPoints[i].first;
    x2->data.fl[i*2+1] = destPoints[i].second;
  }
  
  CvMat *homography = cvCreateMat(3,3,CV_32FC1);
  cvFindHomography(x1, x2, homography);

  cvReleaseMat(&x1);
  cvReleaseMat(&x2);

  return homography;
}

void Billboarding::addCamera(Vector camera) {
  cameras.push_back(camera);
}

void Billboarding::setViewpoint(Vector eye, Vector center) {
  this->camera.eye = eye;
  this->camera.center = center;
}

void Billboarding::loadObjectData(int frame) {
  /*string path = scenarioData->getStringValue("objectDataPath");

  char buf[255];
  sprintf(buf, path.c_str(), frame);

  ResourceData *resourceData = resourceParser.parseDataFromFile(string(buf));

  vector<FVV::Object> tmpObjects;
  if (resourceData) {
    vector<int> subject = resourceData->getIntegerListValue("subject", ',');
    vector<double> h = resourceData->getRealListValue("h", ',');
    vector<int> cx = resourceData->getIntegerListValue("cx", ',');
    vector<int> cy = resourceData->getIntegerListValue("cy", ',');
    
    for (int i = 0; i < h.size(); i++) {
      int subjectNum = 0;
      if (subject.size() > 0) {
	subjectNum = subject[i];
      }
      tmpObjects.push_back(FVV::Object(subjectNum, h[i], cx[i], cy[i]));
    }
    
    delete resourceData;
  }

  if (tmpObjects.size() > 0) {
    objects.clear();
  }

  for (int i = 0; i < tmpObjects.size(); i++) {
    objects.push_back(tmpObjects[i]);
  }*/

  objects.clear();
  objects = objectData->getObjects(frame);

  /*
  for (int i = 0; i < tmpObjects.size(); i++) {
    bool add = true;
    for (int j = i + 1; j < tmpObjects.size(); j++) {
      double dx = (tmpObjects[i].position.x - tmpObjects[j].position.x);
      double dy = (tmpObjects[i].position.y - tmpObjects[j].position.y);
      double dist = sqrt((dx * dx) + (dy * dy));
      if (dist <= 8.0) {
	add = false;
      }
    }
    if (add) {
      objects.push_back(tmpObjects[i]);
    }
  }
  */
}

void Billboarding::renderVideoThumbs(int frame) {
  int numCameras = cameras.size();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  double scrWidth  = 640.0;
  double scrHeight = 480.0;
  glOrtho(0.0, scrWidth, 0.0, scrHeight, -1.0, 1.0);

  glColor3f(1.0, 1.0, 1.0);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      int cameraNumber = (i * 2) + j;
      if ((cameraNumber + 1) > numCameras) {
	continue;
      }

      stringstream key;
      key << "camera" << cameraNumber << "foreground" << frame;
      texManager->bindTexture(key.str());

      int x = j * (scrWidth / 2.0);
      int y = (1 - i) * (scrHeight / 2.0);
      
      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(x, y, 0.0);
      
      glTexCoord2f(1.0, 1.0);
      glVertex3f(x + (scrWidth / 2.0), y, 0.0);
      
      glTexCoord2f(1.0, 0.0);
      glVertex3f(x + (scrWidth / 2.0), y + (scrHeight / 2.0), 0.0);
      
      glTexCoord2f(0.0, 0.0);
      glVertex3f(x, y + (scrHeight / 2.0), 0.0);
      glEnd();
    }
  }

  return;
}

void Billboarding::renderFrame(int frame) {
  glMatrixMode(GL_MODELVIEW);

  // Loads objects position and height from segmentation data files
  loadObjectData(frame);

  Vector viewingDir = Vector(camera.eye.x - camera.center.x,
			     camera.eye.y - camera.center.y,
			     camera.eye.z - camera.center.z);
  viewingDir = viewingDir.normalize();

  // Sort all objects by distance to the current camera plane
  vector<pair<double, int> > zOrdered;
  for (int i = 0; i < objects.size(); i++) {
    if (visibleSubjects[objects[i].subject]) {
      cvmSet(p, 0, 0, objects[i].position.x);
      cvmSet(p, 1, 0, objects[i].position.y);
      cvmSet(p, 2, 0, 1.0);
      cvMatMul(refToWorld, p, r);    
      
      Vector objPos = Vector(r->data.fl[0] / r->data.fl[2],
			     r->data.fl[1] / r->data.fl[2],
			     0.0);
      
      double distToCam = objPos.dot(viewingDir);
      zOrdered.push_back(make_pair(distToCam, i));
    }
  }
  sort(zOrdered.begin(), zOrdered.end());

  // Render all objects starting from those farthest from the camera plane
  // to avoid rendering errors due to alpha blending and depth buffer

// CvMat * worldToRef = cvCreateMat(3, 3, CV_32FC1);
// cvInvert(refToWorld, worldToRef);
//
// cvmSet(p, 0, 0, 7.86);
// cvmSet(p, 1, 0, 6.67);
// cvmSet(p, 2, 0, 1.0);
// cvMatMul(worldToRef, p, r);
// cout << "r:";
// for (int i = 0; i < 3; i++)
//     cout << " " << r->data.fl[i];
// cout << endl;
// FVV::Object * obj1 = new FVV::Object(0, 1.0, (int)r->data.fl[0], (int)r->data.fl[1]);
//
// cvmSet(p, 0, 0, -10.29);
// cvmSet(p, 1, 0, 6.67);
// cvmSet(p, 2, 0, 1.0);
// cvMatMul(worldToRef, p, r);
// cout << "r2:";
// for (int i = 0; i < 3; i++)
//     cout << " " << r->data.fl[i];
// cout << endl;
// FVV::Object * obj2 = new FVV::Object(0, 1.0, (int)r->data.fl[0], (int)r->data.fl[1]);
//
// cvmSet(p, 0, 0, -10.29);
// cvmSet(p, 1, 0, -0.56);
// cvmSet(p, 2, 0, 1.0);
// cvMatMul(worldToRef, p, r);
// cout << "r3:";
// for (int i = 0; i < 3; i++)
//     cout << " " << r->data.fl[i];
// cout << endl;
// FVV::Object * obj3 = new FVV::Object(0, 1.0, (int)r->data.fl[0], (int)r->data.fl[1]);
//
// cvmSet(p, 0, 0, 7.86);
// cvmSet(p, 1, 0, -0.56);
// cvmSet(p, 2, 0, 1.0);
// cvMatMul(worldToRef, p, r);
// cout << "r3:";
// for (int i = 0; i < 3; i++)
//     cout << " " << r->data.fl[i];
// cout << endl;
// FVV::Object * obj4 = new FVV::Object(0, 1.0, (int)r->data.fl[0], (int)r->data.fl[1]);
//
// renderObjectDummy(*obj1);
// renderObjectDummy(*obj2);
// renderObjectDummy(*obj3);
// renderObjectDummy(*obj4);
// delete obj1;
// delete obj2;
// delete obj3;
// delete obj4;
  for (int i = 0; i < zOrdered.size(); i++) {
    if (dummyRender) {
      renderObjectDummy(objects[zOrdered[i].second]);
    } else {
      renderObject(objects[zOrdered[i].second], frame);
    }
  }
}

void Billboarding::renderObjectDummy(FVV::Object object) {
  cvmSet(p, 0, 0, object.position.x);
  cvmSet(p, 1, 0, object.position.y);
  cvmSet(p, 2, 0, 1.0);
  cvMatMul(refToWorld, p, r);

  Vector pos;
  pos.x = cvmGet(r, 0, 0) / cvmGet(r, 2, 0);
  pos.y = cvmGet(r, 1, 0) / cvmGet(r, 2, 0);
  pos.z = 0.0;

  glLoadIdentity();
  glTranslatef(pos.x, pos.y, pos.z);
  
  double width = 0.5;
  double height = 1.5;
  
  glColor3f(1.00, 0.00, 0.00);

  glBegin(GL_QUADS);
  glVertex3f( width / 2.0,  width / 2.0, height);
  glVertex3f(-width / 2.0,  width / 2.0, height);
  glVertex3f(-width / 2.0, -width / 2.0, height);
  glVertex3f( width / 2.0, -width / 2.0, height);
  glEnd();

  glColor3f(0.85, 0.00, 0.00);

  glBegin(GL_TRIANGLES);
  glVertex3f( width / 2.0, width / 2.0, height);
  glVertex3f(         0.0,         0.0,    0.0);
  glVertex3f(-width / 2.0, width / 2.0, height);
  glEnd();

  glColor3f(0.70, 0.00, 0.00);

  glBegin(GL_TRIANGLES);
  glVertex3f(-width / 2.0, width / 2.0, height);
  glVertex3f(         0.0,         0.0,    0.0);
  glVertex3f(-width / 2.0,-width / 2.0, height);
  glEnd();

  glColor3f(0.55, 0.00, 0.00);

  glBegin(GL_TRIANGLES);
  glVertex3f(-width / 2.0,-width / 2.0, height);
  glVertex3f(         0.0,         0.0,    0.0);
  glVertex3f( width / 2.0,-width / 2.0, height);
  glEnd();

  glColor3f(0.40, 0.00, 0.00);

  glBegin(GL_TRIANGLES);
  glVertex3f( width / 2.0,-width / 2.0, height);
  glVertex3f(         0.0,         0.0,    0.0);
  glVertex3f( width / 2.0, width / 2.0, height);
  glEnd();
}

void Billboarding::renderObject(FVV::Object object, int frame) {
  // Calculate the object position in the world coordinate system
  cvmSet(p, 0, 0, object.position.x);
  cvmSet(p, 1, 0, object.position.y);
  cvmSet(p, 2, 0, 1.0);
  cvMatMul(refToWorld, p, r);

  Vector pos;
  pos.x = cvmGet(r, 0, 0) / cvmGet(r, 2, 0);
  pos.y = cvmGet(r, 1, 0) / cvmGet(r, 2, 0);
  pos.z = 0.0;

  Vector origin = Vector(0.0, 0.0, 0.0);
  Vector right = Vector(1.0, 0.0, 0.0);
  Vector up = Vector(0.0, 0.0, 1.0);

  double stdWidth = scenarioData->getRealValue("billboardWidth");
  double stdHeight = scenarioData->getRealValue("billboardHeight");

  double height = object.height * stdHeight;
  double width = stdWidth * height;

  // Calculates the billboard quad dimensions
  Vector a = (origin.translate(right.mult(width * -0.5))).translate(up.mult(-0.1 * height));
  Vector b = (origin.translate(right.mult(width * 0.5))).translate(up.mult(-0.1 * height));
  Vector c = (origin.translate(right.mult(width * 0.5))).translate(up.mult(height));
  Vector d = (origin.translate(right.mult(width * -0.5))).translate(up.mult(height));

  Vector viewingDir = Vector(camera.eye.x - pos.x,
			     camera.eye.y - pos.y,
			     camera.eye.z - pos.z);

  vector<pair<int, double> > camsToRender = vdtm->calcWeights(viewingDir,
							      pos);

  // Calculates the model view matrix for the billboard
  setupBillboardFalse(pos);

  // Needed for blending of multiple camera images
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_BLEND); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Renders each camera image on the billboard using the correct
  // blending mode for the situation

  if (foregroundMask) {
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    renderBillboard(a, b, c, d, object, camsToRender[0].first, frame);

  /*
    if (camsToRender.size() == 1) {
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glColor4f(0.0, 0.0, 0.0, camsToRender[0].second);
      renderBillboard(a, b, c, d, object, camsToRender[0].first, frame);        
    } else {
      for (int i = 0; i < camsToRender.size(); i++) {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
	glColor4f(0.0, 0.0, 0.0, camsToRender[i].second);
	renderBillboard(a, b, c, d, object, camsToRender[i].first, frame);    
      }
    }
  */
  } else {
    glDisable(GL_BLEND);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    renderBillboard(a, b, c, d, object, camsToRender[0].first, frame);
    glEnable(GL_BLEND);

    /*
    if (camsToRender.size() == 1) {
      glDisable(GL_BLEND);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glColor4f(0.0, 0.0, 0.0, camsToRender[0].second);
      renderBillboard(a, b, c, d, object, camsToRender[0].first, frame);        
      glEnable(GL_BLEND);
    } else {
      for (int i = 0; i < camsToRender.size(); i++) {
	if (i == 0) {
	  glDisable(GL_BLEND);
	  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);	 
	  glColor4f(camsToRender[i].second, 
		    camsToRender[i].second,
		    camsToRender[i].second, 1.0);
	  renderBillboard(a, b, c, d, object, camsToRender[i].first, frame);   
	  glEnable(GL_BLEND);
	} else {
	  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	  glColor4f(0.0, 0.0, 0.0, camsToRender[i].second);
	  renderBillboard(a, b, c, d, object, camsToRender[i].first, frame);    
	}
      }
    }
    */
  }

  glDisable(GL_BLEND);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void Billboarding::setupBillboard(Vector object) {
  Vector lookAt;
  Vector objToCamProj;
  Vector objToCam;
  Vector upAux;
  
  double angleCosine;

  glLoadIdentity();

  glTranslatef(object.x, object.y, object.z);
  
  // objToCamProj is the vector in world coordinates from the 
  // local origin to the camera projected in the XZ plane
  objToCamProj.x = camera.eye.x - object.x;
  objToCamProj.y = camera.eye.y - object.y;
  objToCamProj.z = 0.0;
  
  // This is the original lookAt vector for the object 
  // in world coordinates
  lookAt.x = 0.0;
  lookAt.y = -1.0;
  lookAt.z = 0.0;
  
  // normalize both vectors to get the cosine directly afterwards
  objToCamProj = objToCamProj.normalize();
  
  // easy fix to determine wether the angle is negative or positive
  // for positive angles upAux will be a vector pointing in the 
  // positive y direction, otherwise upAux will point downwards
  // effectively reversing the rotation.
  upAux = lookAt.cross(objToCamProj);

  // compute the angle
  angleCosine = lookAt.dot(objToCamProj);
  
  // perform the rotation. The if statement is used for stability reasons
  // if the lookAt and objToCamProj vectors are too close together then 
  // |angleCosine| could be bigger than 1 due to lack of precision
  glRotatef(acos(angleCosine) * 180.0 / M_PI, upAux.x, upAux.y, upAux.z);
  
  // so far it is just like the cylindrical billboard. The code for the 
  // second rotation comes now
  // The second part tilts the object so that it faces the camera
  
  // objToCam is the vector in world coordinates from 
  // the local origin to the camera

  objToCam.x = camera.eye.x - object.x;
  objToCam.y = camera.eye.y - object.y;
  objToCam.z = camera.eye.z - object.z;
  
  // Normalize to get the cosine afterwards
  objToCam = objToCam.normalize();
  
  // Compute the angle between objToCamProj and objToCam, 
  //i.e. compute the required angle for the lookup vector
  angleCosine = objToCamProj.dot(objToCam);
  
  // Tilt the object. The test is done to prevent instability 
  // when objToCam and objToCamProj have a very small
  // angle between them
  //glRotatef(acos(angleCosine) * 180.0 / M_PI, -1.0, 0, 0);	
}

void Billboarding::setupBillboardFalse(Vector object) {
  Vector lookAt;
  Vector objToCamProj;
  Vector objToCam;
  Vector upAux;
  
  double angleCosine;

  glLoadIdentity();

  glTranslatef(object.x, object.y, object.z);
  
  // objToCamProj is the vector in world coordinates from the 
  // object to the camera projected in the XZ plane
  objToCamProj.x = camera.eye.x - camera.center.x;
  objToCamProj.y = camera.eye.y - camera.center.y;
  objToCamProj.z = 0.0;
  
  // This is the original lookAt vector for the object 
  // in world coordinates
  lookAt.x = 0.0;
  lookAt.y = -1.0;
  lookAt.z = 0.0;
  
  // normalize both vectors to get the cosine directly afterwards
  objToCamProj = objToCamProj.normalize();
  
  // easy fix to determine wether the angle is negative or positive
  // for positive angles upAux will be a vector pointing in the 
  // positive y direction, otherwise upAux will point downwards
  // effectively reversing the rotation.
  upAux = lookAt.cross(objToCamProj);

  // compute the angle
  angleCosine = lookAt.dot(objToCamProj);
  
  // perform the rotation. The if statement is used for stability reasons
  // if the lookAt and objToCamProj vectors are too close together then 
  // |angleCosine| could be bigger than 1 due to lack of precision
  glRotatef(acos(angleCosine) * 180.0 / M_PI, upAux.x, upAux.y, upAux.z);
  
  // so far it is just like the cylindrical billboard. The code for the 
  // second rotation comes now
  // The second part tilts the object so that it faces the camera
  
  // objToCam is the vector in world coordinates from 
  // the local origin to the camera

  objToCam.x = camera.eye.x - camera.center.x;
  objToCam.y = camera.eye.y - camera.center.y;
  objToCam.z = camera.eye.z - camera.center.z;
  
  // Normalize to get the cosine afterwards
  objToCam = objToCam.normalize();
  
  // Compute the angle between objToCamProj and objToCam, 
  //i.e. compute the required angle for the lookup vector
  angleCosine = objToCamProj.dot(objToCam);

  double angleSine = asin(objToCam.z / objToCam.length());
  
  // Tilt the object. The test is done to prevent instability 
  // when objToCam and objToCamProj have a very small
  // angle between them
  //glRotatef(acos(angleSine) * 180.0 / M_PI, -1.0, 0, 0);	
  glRotatef(angleSine * 180.0 / M_PI, -1.0, 0, 0);
}

void Billboarding::renderBillboard(Vector a, Vector b, Vector c, Vector d,
				   FVV::Object object, int cam, int frame) {
  double reduction = scenarioData->getRealValue("cmReductionFactor");
  double stdTexWidth = scenarioData->getRealValue("stdTexWidth");
  double stdTexHeight = scenarioData->getRealValue("stdTexHeight");

  cvmSet(p, 0, 0, object.position.x);
  cvmSet(p, 1, 0, object.position.y);
  cvmSet(p, 2, 0, 1.0);
  cvMatMul(refToCam[cam], p, r);

  double texX = (r->data.fl[0] / r->data.fl[2]);
  double texY = (r->data.fl[1] / r->data.fl[2]);

  if (texY < 0.0) {
    texY = 0.0;
  }

  if (texY >= correctionMat[0].size()) {
    texY = correctionMat[0].size() - 1;
  }

  if (texX < 0.0) {
    texX = 0.0;
  }

  if (texX >= correctionMat[0][0].size()) {
    texX = correctionMat[0][0].size() - 1;
  }
  
  double texHeight = correctionMat[cam][(int)texY][(int)texX] * stdTexHeight;
  double texWidth = stdTexWidth * texHeight;

  //texHeight = reduction * (object.height + 0.15) * (texHeight / winHeight);
  texHeight = reduction * (object.height) * (texHeight / winHeight);
  texWidth = (texWidth / winWidth);
  texX = (texX * reduction / winWidth);
  texY = (texY * reduction / winHeight);

  stringstream key;
  key << "camera" << cam << "foreground" << frame;
  texManager->bindTexture(key.str());
  
  Vector texA = Vector(texX - 0.5 * texWidth,
		       texY + 0.1 * texHeight,
		       0.0);

  Vector texB = Vector(texX + 0.5 * texWidth,
		       texY + 0.1 * texHeight,
		       0.0);

  Vector texC = Vector(texX + 0.5 * texWidth,
		       texY - texHeight,
		       0.0);

  Vector texD = Vector(texX - 0.5 * texWidth,
		       texY - texHeight,
		       0.0);

  bool isVisible = true;
  if (texA.x < 0.0f || texA.y > 1.0f ||
      texB.x > 1.0f || texB.y > 1.0f ||
      texC.x > 1.0f || texC.y < 0.0f ||
      texD.x < 0.0f || texD.y < 0.0f) {
    isVisible = false;
  }

  if (isVisible) {
    glBegin(GL_QUADS);
    
    glTexCoord2f(texX - 0.5 * texWidth, texY + 0.1 * texHeight);
    glVertex3f(a.x, a.y, a.z);
    
    glTexCoord2f(texX + 0.5 * texWidth, texY + 0.1 * texHeight);
    glVertex3f(b.x, b.y, b.z);
    
    glTexCoord2f(texX + 0.5 * texWidth, texY - texHeight);
    glVertex3f(c.x, c.y, c.z); 
    
    glTexCoord2f(texX - 0.5 * texWidth, texY - texHeight);
    glVertex3f(d.x, d.y, d.z);
    
    glEnd(); 
  }
}

void Billboarding::setSubjectVisibility(int subject, bool visibility) {
  visibleSubjects[subject] = visibility;
}

void Billboarding::generateBillboardData() {
  cout << "-------------------------   Generating billboard data" << endl;

  ofstream data;
  data.open("billboards.dat");

  Vector origin = Vector(0.0, 0.0, 0.0);
  Vector right = Vector(1.0, 0.0, 0.0);
  Vector up = Vector(0.0, 0.0, 1.0);

  int startFrame = scenarioData->getIntValue("startingFrame");
  int endFrame = scenarioData->getIntValue("endingFrame");

  double stdWidth = scenarioData->getRealValue("billboardWidth");
  double stdHeight = scenarioData->getRealValue("billboardHeight");

  data << endFrame - startFrame + 1 << " " << cameras.size() << endl;

  for (int frame = startFrame; frame <= endFrame; frame++) {
    data << frame << endl;
    loadObjectData(frame);

    for (int cam = 0; cam < cameras.size(); cam++) {
      data << cam << endl;
      
      // Sort all objects by distance to the current camera plane
      vector<pair<double, int> > zOrdered;
      for (int i = 0; i < objects.size(); i++) {
	if (visibleSubjects[objects[i].subject]) {
	  cvmSet(p, 0, 0, objects[i].position.x);
	  cvmSet(p, 1, 0, objects[i].position.y);
	  cvmSet(p, 2, 0, 1.0);
	  cvMatMul(refToWorld, p, r);    
	  
	  Vector objPos = Vector(r->data.fl[0] / r->data.fl[2],
				 r->data.fl[1] / r->data.fl[2],
				 0.0);
	  
	  double distToCam = objPos.dot(cameras[cam]);
	  zOrdered.push_back(make_pair(distToCam, i));
	}
      }
      sort(zOrdered.begin(), zOrdered.end());

      data << zOrdered.size() << endl;
      for (int obj = 0; obj < zOrdered.size(); obj++) {
	FVV::Object object = objects[zOrdered[obj].second];
	
	// Calculate the object position in the world coordinate system
	cvmSet(p, 0, 0, object.position.x);
	cvmSet(p, 1, 0, object.position.y);
	cvmSet(p, 2, 0, 1.0);
	cvMatMul(refToWorld, p, r);
	
	Vector pos;
	pos.x = cvmGet(r, 0, 0) / cvmGet(r, 2, 0);
	pos.y = cvmGet(r, 1, 0) / cvmGet(r, 2, 0);
	pos.z = 0.0;
	
	double height = object.height * stdHeight;
	double width = stdWidth * height;
	
	// Calculates the billboard quad dimensions
	Vector a = (origin.translate(right.mult(width * -0.5))).translate(up.mult(-0.1 * height));
	Vector b = (origin.translate(right.mult(width * 0.5))).translate(up.mult(-0.1 * height));
	Vector c = (origin.translate(right.mult(width * 0.5))).translate(up.mult(height));
	Vector d = (origin.translate(right.mult(width * -0.5))).translate(up.mult(height));
	
	double reduction = scenarioData->getRealValue("cmReductionFactor");
	double stdTexWidth = scenarioData->getRealValue("stdTexWidth");
	double stdTexHeight = scenarioData->getRealValue("stdTexHeight");
	
	cvmSet(p, 0, 0, object.position.x);
	cvmSet(p, 1, 0, object.position.y);
	cvmSet(p, 2, 0, 1.0);
	cvMatMul(refToCam[cam], p, r);
	
	double texX = (r->data.fl[0] / r->data.fl[2]);
	double texY = (r->data.fl[1] / r->data.fl[2]);
	
	if (texY < 0.0) {
	  texY = 0.0;
	}
	
	if (texY >= correctionMat[0].size()) {
	  texY = correctionMat[0].size() - 1;
	}
	
	if (texX < 0.0) {
	  texX = 0.0;
	}
	
	if (texX >= correctionMat[0][0].size()) {
	  texX = correctionMat[0][0].size() - 1;
	}
	
	double texHeight = correctionMat[cam][(int)texY][(int)texX] * stdTexHeight;
	double texWidth = stdTexWidth * texHeight;
	
	//texHeight = reduction * (object.height + 0.15) * (texHeight / winHeight);
	texHeight = reduction * (object.height) * (texHeight / winHeight);
	texWidth = (texWidth / winWidth);
	texX = (texX * reduction / winWidth);
	texY = (texY * reduction / winHeight);
	
	stringstream key;
	key << "camera" << cam << "foreground" << frame;
	texManager->bindTexture(key.str());
	
	Vector texA = Vector(texX - 0.5 * texWidth,
			     texY + 0.1 * texHeight,
			     0.0);
	
	Vector texB = Vector(texX + 0.5 * texWidth,
			     texY + 0.1 * texHeight,
			     0.0);
	
	Vector texC = Vector(texX + 0.5 * texWidth,
			     texY - texHeight,
			     0.0);
	
	Vector texD = Vector(texX - 0.5 * texWidth,
			     texY - texHeight,
			     0.0);
	
	data << object.subject << " " << pos.x << " " << pos.y << " " << texA.x << " " << texD.y << " " << texB.x << " " << texB.y << endl;
      }
    }
  }

  data.close();
}
