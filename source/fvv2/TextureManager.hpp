#ifndef _INC_TEXTUREMANAGER_HPP
#define _INC_TEXTUREMANAGER_HPP

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <cv.h>
#include <highgui.h>

#include <map>
#include <string>
#include <sstream>

using namespace std;

class TextureManager {
private:
  map<string, GLuint> textureIDs;
  IplImage *srcImage;
  IplImage *maskImage;

  unsigned char LUT[3][256];

public:
  bool colorCalibrate;
  bool backgroundMask;

  TextureManager();
  ~TextureManager();

  void bindTexture(string key);
  GLuint getTextureID(string key);
  void loadTexture(string filename, string id);
  void loadTextureFB(string image, string mask, string id, int frame);
};

#endif
