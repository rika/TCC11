#include "SceneModel.hpp"

SceneModel::SceneModel(string scenarioDataPath) {
  texManager = new TextureManager();

  scenarioData = 0;
  billboarding = 0;
  vdtm = 0;

  videoThumbs = false;
  holeFilling = false;
  videoProjection = false;

  play = true;
  forward = true;
  up = false;
  down = false;
  front = false;
  back = false;
  left = false;
  right = false;
  fovUp = false;
  fovDown = false;
  riseSpeed = false;
  lowerSpeed = false;
  speed = 0.05;
  
  Glib::RefPtr<Gdk::GL::Config> glconfig;

  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_DOUBLE | 
				     Gdk::GL::MODE_RGBA |
				     Gdk::GL::MODE_DEPTH);
  
  if (!glconfig) {
    glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGBA |
				       Gdk::GL::MODE_DEPTH);

    if (!glconfig) {
      std::cerr << "Couldn't initialize specified video mode for the scene model" << std::endl;
      std::exit(1);
    }
  }

  set_gl_capability(glconfig);

  add_events(Gdk::BUTTON1_MOTION_MASK |
	     Gdk::BUTTON2_MOTION_MASK |
	     Gdk::BUTTON_PRESS_MASK |
	     Gdk::BUTTON_RELEASE_MASK |
	     //Gdk::KEY_PRESS_MASK |
	     //Gdk::KEY_RELEASE_MASK |
	     Gdk::POINTER_MOTION_MASK);  

  signal_button_press_event().connect(sigc::mem_fun(this, &SceneModel::on_button_press_event));
  signal_button_release_event().connect(sigc::mem_fun(this, &SceneModel::on_button_release_event));  
  //signal_key_press_event().connect(sigc::mem_fun(this, &SceneModel::on_key_press_event));
  //signal_key_release_event().connect(sigc::mem_fun(this, &SceneModel::on_key_release_event));
  signal_motion_notify_event().connect(sigc::mem_fun(this, &SceneModel::on_motion_notify_event));

  property_can_focus().set_value(true);

  // Reset camera movement parameters
  m_BeginX = 0.0;
  m_BeginY = 0.0;
  m_DX = m_DY = m_DZ = dFov = 0.0;

  angleX = angleY = angleZ = 0.0;

  vx = vy = vz = 0.0;

  ResourceParser resourceParser;
  scenarioData = resourceParser.parseDataFromFile(scenarioDataPath);
  startFrame = scenarioData->getIntValue("startingFrame");
  endFrame = scenarioData->getIntValue("endingFrame");
  frame = startFrame;

  double width = scenarioData->getRealValue("videoFrameWidth");
  double height = scenarioData->getRealValue("videoFrameHeight");
  this->set_size_request(width, height);

  // Point correspondences for tripod camera
  /*
  int numPoints = 4;
  double aWorldPoints[] = {0.00000, 0.00000, 0.00000,
			   1.00000, 0.00000, 0.00000,
			   1.00000, 1.00000, 0.00000,
			   0.00000, 1.00000, 0.00000};

  double aImagePoints[] = {230, 101,
			   129, 125,
			   203, 173,
			   269, 149};

  double aCameraMatrix[] = {320.0, 0.0, 160.0,
			    0.0, 240.0, 120.0,
			    0.0, 0.0, 1.0};


  int aNumViews[] = {4};
  CvMat imagePoints = cvMat(numPoints, 2, CV_64FC1, aImagePoints);
  CvMat worldPoints = cvMat(numPoints, 3, CV_64FC1, aWorldPoints);
  CvMat numViews = cvMat(1, 1, CV_32SC1, aNumViews); 
  CvMat cameraMatrix = cvMat(3, 3, CV_64FC1, aCameraMatrix);
  
  CvMat *distortion = cvCreateMat(4, 1, CV_64FC1);
  CvMat *rotation = cvCreateMat(1, 3, CV_64FC1);
  CvMat *translation = cvCreateMat(1, 3, CV_64FC1);

  cvFindExtrinsicCameraParams2(&worldPoints, &imagePoints, &cameraMatrix,
  			       NULL, rotation, translation);

  printf("%f\n", cvmGet(translation, 0, 0));
  printf("%f\n", cvmGet(translation, 0, 1));
  printf("%f\n", cvmGet(translation, 0, 2));
  */
}


SceneModel::~SceneModel() {
  if (billboarding) {
    delete billboarding;
  }

  if (scenarioData) {
    delete scenarioData;
  }

  if (texManager) {
    delete texManager;
  }

  if (vdtm) {
    delete vdtm;
  }
}

bool SceneModel::on_idle() {
  get_window()->invalidate_rect(get_allocation(), false);  
  get_window()->process_updates(false); 

  return true;
}

void SceneModel::idle_add() {
  if (!m_ConnectionIdle.connected()) {
    m_ConnectionIdle = 
      Glib::signal_idle().connect(sigc::mem_fun(this, &SceneModel::on_idle), GDK_PRIORITY_REDRAW);  
  }
}

void SceneModel::idle_remove() {
  if (m_ConnectionIdle.connected()) {
    m_ConnectionIdle.disconnect();  
  }
}

void SceneModel::on_realize() {
  Gtk::DrawingArea::on_realize();

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  if (!glwindow->gl_begin(get_gl_context())) {
    return; 
  }

  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);

  setupTextures();

  vdtm = new VDTM(scenarioData);

  billboarding = new Billboarding(texManager, scenarioData, vdtm);
  billboarding->setup();

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClearDepth(1.0);

  glViewport(0, 0, get_width(), get_height());

  glwindow->gl_end();

  // Sets the initial position of the camera and field of view
  camera.eye = Vector(-7.13,
		      13.45,
		      5.1); 

  camera.fov = 39.15;
}

bool SceneModel::on_configure_event(GdkEventConfigure *event) {
  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  
  if (!glwindow->gl_begin(get_gl_context())) {
    return false;
  }

  glViewport(0, 0, get_width(), get_height());
  
  glwindow->gl_end();

  return true;
}


bool SceneModel::on_expose_event(GdkEventExpose *event) {
  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  if (!glwindow->gl_begin(get_gl_context())) {
    return false;
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  render();
 
  if (glwindow->is_double_buffered()) {
    glwindow->swap_buffers();
  } else {
    glFlush();
  }

  glwindow->gl_end();

  return true;
}

bool SceneModel::on_button_press_event(GdkEventButton *event) {
  return false;
}

bool SceneModel::on_button_release_event(GdkEventButton *event) {
  return false;
}

bool SceneModel::on_key_press_event(GdkEventKey *event) {
  bool redraw = false;
 
  if (event->type == GDK_KEY_PRESS) {
    switch(event->keyval) {
    case GDK_r:
      forward = false;
      break;
      
    case GDK_f:
      forward = true;
      break;

    case GDK_Up:
      front = true;
      redraw = true;
      break;
      
    case GDK_Down:
      back = true; 
      redraw = true;
      break;
      
    case GDK_Left:
      left = true;
      redraw = true;
      break;
      
    case GDK_Right:
      right = true;
      redraw = true;
      break;
      
    case GDK_Page_Up:
      up = true;
      redraw = true;
      break;
      
    case GDK_Page_Down:
      down = true;
      redraw = true;
      break;
      
    case GDK_q:
      fovUp = true;
      redraw = true;
      break;
      
    case GDK_a:
      fovDown = true;
      redraw = true;
      break;

    case GDK_z:
      riseSpeed = true;
      break;

    case GDK_x:
      lowerSpeed = true;
      break;
    }
  }

  if (redraw) {
    idle_add();
    get_window()->invalidate_rect(get_allocation(), false);    
  }

  return false;
}

bool SceneModel::on_key_release_event(GdkEventKey *event) {
  bool redraw = false;

  if (event->type == GDK_KEY_RELEASE) {
    switch(event->keyval) {
    case GDK_c:
      if (texManager) {
	texManager->colorCalibrate = !texManager->colorCalibrate;
      }
      break;

    case GDK_d:
      if (billboarding) {
	billboarding->toggleDummyRender();
      }
      break;

    case GDK_t:
      videoThumbs = !videoThumbs;
      break;

    case GDK_h:
      holeFilling = !holeFilling;
      break;

    case GDK_p:
      videoProjection = !videoProjection;
      break;

    case GDK_m:
      if (billboarding) {
	billboarding->toggleForegroundMask();
      }
      break;

    case GDK_space:
      play = (play ? false : true);
      break;
      
    case GDK_Up:
      front = false;
      redraw = true;
      break;
      
    case GDK_Down:
      back = false;
      redraw = true;
      break;
      
    case GDK_Left:
      left = false;
      redraw = true;
      break;
      
    case GDK_Right:
      right = false;
      redraw = true;
      break;
      
    case GDK_Page_Up:
      up = false;
      redraw = true;
      break;
      
    case GDK_Page_Down:
      down = false;
      redraw = true;
      break;
      
    case GDK_q:
      fovUp = false;
      redraw = true;
      break;
      
    case GDK_a:
      fovDown = false;
      redraw = true;
      break;

    case GDK_z:
      riseSpeed = false;
      break;

    case GDK_x:
      lowerSpeed = false;
      break;
    }
  }

  if (redraw) {
    idle_add();
    get_window()->invalidate_rect(get_allocation(), false);    
  }

  return false;
}

bool SceneModel::on_motion_notify_event(GdkEventMotion *event) {
  float x = event->x;
  float y = event->y;
  bool redraw = false;

  if (event->state & GDK_BUTTON1_MASK) {
    m_DX = x - m_BeginX;
    m_DY = y - m_BeginY;
    
    angleX -= (m_DX / 16.0);
    angleY -= (m_DY / 16.0);
    if (angleY >= 90.0) {
      angleY = 89.9;
    }
    if (angleY <= -90.0) {
      angleY = -89.9;
    }

    redraw = true;
  }

  if (event->state & GDK_BUTTON3_MASK) {
    m_DZ = x - m_BeginX;

    angleZ -= (m_DZ / 16.0);

    redraw = true;
  }

  m_BeginX = x;
  m_BeginY = y;
    
  if (redraw) {
    get_window()->invalidate_rect(get_allocation(), false);
  }
  
  return false;
}

void SceneModel::setupTextures() {
  int numCameras = scenarioData->getIntValue("numCameras");

  vector<string> model = scenarioData->getStringListValue("model", ',');
  int numTextures = model.size();

  for (int i = 0; i < model.size(); i++) {
    stringstream key;
    key << "tex" << model[i];
    string texturePath = scenarioData->getStringValue(key.str());
    
    texManager->loadTexture(texturePath, key.str());
  }
}

void SceneModel::loadTextures(int frame) {
  char image[255];
  char mask[255];

  int numCameras = scenarioData->getIntValue("numCameras");
  
  string videoPath = scenarioData->getStringValue("videoPath");
  string maskPath = scenarioData->getStringValue("maskPath");

  for (int i = 0; i < numCameras; i++) {
    sprintf(image, videoPath.c_str(), i, frame);
    sprintf(mask, maskPath.c_str(), i, frame);

    stringstream key;
    key << "camera" << i;

    texManager->loadTextureFB(image, mask, key.str());
  }
}

void SceneModel::render() {
  if (riseSpeed) {
    speed += 0.01;
    if (speed > 0.15) {
      speed = 0.15;
    }
  }

  if (lowerSpeed) {
    speed -= 0.01;
    if (speed < 0.01) {
      speed = 0.01;
    }
  }

  vx = 0.0;
  if (left) {
    vx -= speed;
  }
  if (right) {
    vx += speed;
  }
  
  vy = 0.0;
  if (up) {
    vy += speed;
  }
  if (down) {
    vy -= speed;
  }

  vz = 0.0;
  if (front) {
    vz += speed;
  }
  if (back) {
    vz -= speed;
  }

  dFov = 0.0;
  if (fovUp) {
    dFov += speed;
  }
  if (fovDown) {
    dFov -= speed;
  }

  // Positioning camera on (0.0, 0.0, 0.0) for rotations
  Vector origin = Vector(0.0, 0.0, 0.0);
  camera.center = Vector(0.0, -1.0, 0.0);
  camera.up = Vector(0.0, 0.0, 1.0);

  // Sets the camera field of view
  camera.fov += dFov;

  // Rotating around the y axis 
  camera.rotate(Vector(0.0, 0.0, 1.0), angleX * (M_PI / 180.0));

  // Rotating around the new x axis
  Vector axis = Vector(camera.center.x - origin.x,
		       camera.center.y - origin.y,
		       camera.center.z - origin.z);
  axis = (axis.cross(camera.up)).normalize();
  camera.rotate(axis, angleY * (M_PI / 180.0));

  // Rotating around the new z axis
  axis = camera.center.normalize();
  camera.rotate(axis, angleZ * (M_PI / 180.0));

  // Translating the camera to its current position
  Vector move = camera.center.normalize();
  camera.eye = camera.eye.translate(move.mult(vz));

  move = camera.up.normalize();
  camera.eye = camera.eye.translate(move.mult(vy));

  move = (camera.center.cross(camera.up)).normalize();
  camera.eye = camera.eye.translate(move.mult(vx));

  camera.center = camera.center.translate(camera.eye);

  // Setting the camera parameters for rendering
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(camera.fov, 
		 (double)get_width() / (double)get_height(), 0.1, 50.0);
  gluLookAt(camera.eye.x, camera.eye.y, camera.eye.z,
            camera.center.x, camera.center.y, camera.center.z,
            camera.up.x, camera.up.y, camera.up.z);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glColor3f(1.0, 1.0, 1.0);  

  printf("Camera parameters\n");
  printf("----------------------------------------------------------------------\n");
  printf("Eye    %2.5f %2.5f %2.5f\n", camera.eye.x, camera.eye.y, camera.eye.z);
  printf("Center %2.5f %2.5f %2.5f\n", camera.center.x, camera.center.y, camera.center.z);
  printf("Up     %2.5f %2.5f %2.5f\n", camera.up.x, camera.up.y, camera.up.z);
  printf("FOV    %2.5f", camera.fov);
  printf("\n");
  printf("Speed  %f\n", speed);

  loadTextures(frame);

  renderModel();
  if (videoProjection) {
    renderVDModel();
  }

  billboarding->setViewpoint(camera.eye, camera.center);
  billboarding->renderFrame(frame);

  if (play) {
    if (forward) {
      frame++;
    } else {
      frame--;
    }
    if (frame > endFrame) {
      frame = startFrame;
    } else if (frame < startFrame) {
      frame = endFrame;
    }
  }

  if (videoThumbs) {
    billboarding->renderVideoThumbs();
  }
}

void SceneModel::renderModel() {
  vector<string> model = scenarioData->getStringListValue("model", ',');

  for (int i = 0; i < model.size(); i++) {
    stringstream vs[4];
    stringstream ts[4];
    vector<double> v[4];
    vector<double> t[4];
    for (int j = 0; j < 4; j++) {
      vs[j] << "v" << j << model[i];
      v[j] = scenarioData->getRealListValue(vs[j].str(), ',');

      ts[j] << "t" << j << model[i];
      t[j] = scenarioData->getRealListValue(ts[j].str(), ',');
    }
    
    stringstream tex;
    tex << "tex" << model[i];
    texManager->bindTexture(tex.str());

    glBegin(GL_QUADS);
    for (int j = 0; j < 4; j++) {
      glTexCoord2f(t[j][0], t[j][1]);
      glVertex3f(v[j][0], v[j][1], v[j][2]);
    }
    glEnd();
  }
}

void SceneModel::renderVDModel() {
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);
  glEnable(GL_TEXTURE_GEN_Q);

  GLfloat Splane[] = {1.f, 0.f, 0.f, 0.f}; 
  GLfloat Tplane[] = {0.f, 1.f, 0.f, 0.f}; 
  GLfloat Rplane[] = {0.f, 0.f, 1.f, 0.f}; 
  GLfloat Qplane[] = {0.f, 0.f, 0.f, 1.f};  
  
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

  glTexGenfv(GL_S, GL_EYE_PLANE, Splane);
  glTexGenfv(GL_T, GL_EYE_PLANE, Tplane);
  glTexGenfv(GL_R, GL_EYE_PLANE, Rplane);
  glTexGenfv(GL_Q, GL_EYE_PLANE, Qplane);

  if (holeFilling) {
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  vector<string> model = scenarioData->getStringListValue("model", ',');

  for (int i = 0; i < model.size(); i++) {
    stringstream vs[4];
    stringstream ts[4];
    vector<double> v[4];
    vector<double> t[4];
    for (int j = 0; j < 4; j++) {
      vs[j] << "v" << j << model[i];
      v[j] = scenarioData->getRealListValue(vs[j].str(), ',');

      ts[j] << "t" << j << model[i];
      t[j] = scenarioData->getRealListValue(ts[j].str(), ',');
    }

    Vector pos = Vector((v[0][0] + v[2][0]) / 2.0,
			(v[0][1] + v[2][1]) / 2.0,
			(v[0][2] + v[2][2]) / 2.0);

    Vector viewingDir = Vector(camera.eye.x - pos.x,
			       camera.eye.y - pos.y,
			       camera.eye.z - pos.z);

    vector<pair<int, double> > camsToRender = 
      vdtm->calcWeights(viewingDir, pos);
    
    for (int i = 0; i < camsToRender.size(); i++) {
      int cam = (camsToRender[i].first + 1);

      stringstream keyEye;
      keyEye << "camera" << cam << "Eye";
      
      stringstream keyCenter;
      keyCenter << "camera" << cam << "Center";
      
      stringstream keyUp;
      keyUp << "camera" << cam << "Up";
      
      stringstream keyFOV;
      keyFOV << "camera" << cam << "FOV";

      vector<double> cameraEye = 
	scenarioData->getRealListValue(keyEye.str(), ',');
      vector<double> cameraCenter = 
	scenarioData->getRealListValue(keyCenter.str(), ',');
      vector<double> cameraUp = 
	scenarioData->getRealListValue(keyUp.str(), ',');
      double cameraFOV = scenarioData->getRealValue(keyFOV.str());

      double width = scenarioData->getRealValue("videoFrameWidth");
      double height = scenarioData->getRealValue("videoFrameHeight");

      glMatrixMode(GL_TEXTURE);
      glLoadIdentity();
      glTranslatef(.5f, .5f, .0f);
      glScalef(.5f, -.5f, 1.f);
      gluPerspective(cameraFOV, 
		     width / height, 0.1, 50.0);
      gluLookAt(cameraEye[0],    cameraEye[1],    cameraEye[2],
		cameraCenter[0], cameraCenter[1], cameraCenter[2],
		cameraUp[0],     cameraUp[1],     cameraUp[2]);

      stringstream keyTex;
      keyTex << "camera" << (cam - 1) << "background";
      texManager->bindTexture(keyTex.str());

      double alpha = camsToRender[i].second;
      glColor4f(1.0, 1.0, 1.0, alpha);

      glBegin(GL_QUADS);
      for (int j = 0; j < 4; j++) {
	glTexCoord2f(t[j][0], t[j][1]);
	glVertex3f(v[j][0], v[j][1], v[j][2]);
      }
      glEnd();
    }
  }

  glDisable(GL_BLEND); 

  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);
  glDisable(GL_TEXTURE_GEN_Q);

  glLoadIdentity();
}
