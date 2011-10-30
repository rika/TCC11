#ifndef _INC_SCENEMODEL_H
#define _INC_SCENEMODEL_H

#include <gtkmm.h>
#include <gtkglmm.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <cv.h>
#include <highgui.h>

#include <iostream>

#include "Camera.hpp"
#include "Vector.hpp"
#include "Boundary.hpp"
#include "Edge.hpp"
#include "Point.hpp"
#include "Billboarding.hpp"
#include "TextureManager.hpp"
#include "VDTM.hpp"

#include "ResourceParser.h"
#include "ResourceData.h"

const int MAX_TEXTURES     = 200;

const int TEX_FLOOR        = 0;
const int TEX_WALL         = 1;

class SceneModel : public Gtk::DrawingArea, 
		   public Gtk::GL::Widget<SceneModel> {
public:
  SceneModel(string scenarioDataPath);
  virtual ~SceneModel();
  
protected:
  sigc::connection m_ConnectionIdle;

  virtual bool on_idle();
  void idle_add();
  void idle_remove();

  virtual void on_realize();
  virtual bool on_configure_event(GdkEventConfigure *event);
  virtual bool on_expose_event(GdkEventExpose *event);

  virtual bool on_button_press_event(GdkEventButton *event);
  virtual bool on_button_release_event(GdkEventButton *event);
  virtual bool on_key_press_event(GdkEventKey *event);
  virtual bool on_key_release_event(GdkEventKey *event);
  virtual bool on_motion_notify_event(GdkEventMotion *event);

  virtual void setupTextures();
  virtual void loadTextures(int frame);
  virtual void render();

  void renderModel();
  void renderVDModel();

private:
  TextureManager *texManager;
  ResourceData *scenarioData;
  VDTM *vdtm;

  bool videoThumbs;
  bool holeFilling;
  bool videoProjection;

  bool play;
  bool forward;
  int frame;
  int startFrame;
  int endFrame;

  bool front;
  bool back;
  bool up;
  bool down;
  bool left;
  bool right;
  bool fovUp;
  bool fovDown;
  bool riseSpeed;
  bool lowerSpeed;

  float m_BeginX;
  float m_BeginY;
  float m_DX;
  float m_DY;
  float m_DZ;

  float dFov;

  float vx;
  float vy;
  float vz;

  float angleX;
  float angleY;
  float angleZ;

  float speed;

  Camera camera;
  Billboarding *billboarding;
};

#endif



