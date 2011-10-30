#ifndef _INC_MODELVIEWER_H
#define _INC_MODELVIEWER_H

#include <gtkmm.h>

#include "SceneModel.hpp"

class ModelViewer : public Gtk::Window {
public:
  ModelViewer(string dataFile);
  virtual ~ModelViewer();
  
protected:
  string dataFile;
  Gtk::VBox m_VBox;
  
  SceneModel *model;
};

#endif




  
