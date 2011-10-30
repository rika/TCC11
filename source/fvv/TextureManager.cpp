#include "TextureManager.hpp"

#include <iostream>
#include <fstream>

using namespace std;

TextureManager::TextureManager() {
  colorCalibrate = false;

  srcImage = 0;
  maskImage = 0;

  ifstream file;
  file.open("result.lut");
  for (int c = 0; c < 3; c++) {
    for (int k = 0; k < 256; k++) {
      int value;
      file >> value;
      LUT[c][k] = value;
    }
  }
  file.close();
}

TextureManager::~TextureManager() {

}

void TextureManager::bindTexture(string key) {
  map<string, GLuint>::iterator it = textureIDs.find(key);

  if (it != textureIDs.end()) {
    GLuint textureID = textureIDs[key];
    glBindTexture(GL_TEXTURE_2D, textureID);    
  }
}

GLuint TextureManager::getTextureID(string key) {
  GLuint textureID = -1;
  map<string, GLuint>::iterator it = textureIDs.find(key);

  if (it != textureIDs.end()) {
    textureID = textureIDs[key];
  }

  return textureID;
}

void TextureManager::loadTexture(string filename, string id) {
  if (srcImage) {
    cvReleaseImage(&srcImage);
    srcImage = 0;
  }

  srcImage = cvLoadImage(filename.c_str());

  GLuint textureID = getTextureID(id);
  if (textureID == -1) {
    glGenTextures(1, &textureID);
    textureIDs[id] = textureID;    
  }

  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, srcImage->width, srcImage->height, GL_BGR_EXT, GL_UNSIGNED_BYTE, srcImage->imageData );
}

void TextureManager::loadTextureFB(string image, string mask, string id) {
  int border = 32;

  if (srcImage) {
    cvReleaseImage(&srcImage);
    srcImage = 0;
  }
  if (maskImage) {
    cvReleaseImage(&maskImage);
    maskImage = 0;
  }
  srcImage = cvLoadImage(image.c_str());
  maskImage = cvLoadImage(mask.c_str());

  int imageStep       = srcImage->widthStep / sizeof(unsigned char);
  int imageChannels   = srcImage->nChannels;
  unsigned char *imageData = (unsigned char *)srcImage->imageData;

  int maskStep        = maskImage->widthStep / sizeof(unsigned char);
  int maskChannels    = maskImage->nChannels;
  unsigned char *maskData  = (unsigned char *)maskImage->imageData;

  GLubyte *textureDataF = new GLubyte[srcImage->width * srcImage->height * 4];
  GLubyte *textureDataB = new GLubyte[srcImage->width * srcImage->height * 4];
  for (int i = 0; i < srcImage->height; i++) {
    for (int j = 0; j < srcImage->width; j++) {
      unsigned char r = imageData[i * imageStep + j * imageChannels + 2];
      unsigned char g = imageData[i * imageStep + j * imageChannels + 1];
      unsigned char b = imageData[i * imageStep + j * imageChannels + 0];
      unsigned char a = maskData[i * maskStep + j * maskChannels + 0];
      
      unsigned int offset = (i * srcImage->width * 4) + (j * 4);
      if (colorCalibrate) {
	textureDataF[offset + 0] = textureDataB[offset + 0] = LUT[2][r];
	textureDataF[offset + 1] = textureDataB[offset + 1] = LUT[1][g];
	textureDataF[offset + 2] = textureDataB[offset + 2] = LUT[0][b];
      } else {
	textureDataF[offset + 0] = textureDataB[offset + 0] = r;
	textureDataF[offset + 1] = textureDataB[offset + 1] = g;
	textureDataF[offset + 2] = textureDataB[offset + 2] = b;
      }
      textureDataF[offset + 3] = a;

      int vDist = min(i - 0, srcImage->height - i);
      int hDist = min(j - 0, srcImage->width - j);
      int dist = min(vDist, hDist);

      if (dist <= border) {
	double alpha = ((double)dist / (double)border) * 255.0;
	
	textureDataB[offset + 3] = (unsigned char)alpha;
      } else {
	textureDataB[offset + 3] = 255 - a;
      }
    }
  }

  stringstream keyF;
  keyF << id << "foreground";
  GLuint fTexID = getTextureID(keyF.str());

  stringstream keyB;
  keyB << id << "background";
  GLuint bTexID = getTextureID(keyB.str());

  if (fTexID == -1) {
    glGenTextures(1, &fTexID);
    textureIDs[keyF.str()] = fTexID;
  }

  if (bTexID == -1) {
    glGenTextures(1, &bTexID);
    textureIDs[keyB.str()] = bTexID;
  }
  
  glBindTexture(GL_TEXTURE_2D, fTexID);    
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
	       srcImage->width, srcImage->height, 0, 
	       GL_RGBA, GL_UNSIGNED_BYTE, textureDataF);

  glBindTexture(GL_TEXTURE_2D, bTexID);    
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
	       srcImage->width, srcImage->height, 0, 
	       GL_RGBA, GL_UNSIGNED_BYTE, textureDataB);

  delete textureDataF;
  delete textureDataB;
}

