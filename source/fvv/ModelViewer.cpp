#include "ModelViewer.hpp"

ModelViewer::ModelViewer(string dataFile) : m_VBox(false, 0) {
  set_title("TV3D - Model Viewer");
  set_reallocate_redraws(true);

  add(m_VBox);

  model = new SceneModel(dataFile);
  model->set_size_request(640, 480);
  //model.set_size_request(1000, 800);
  m_VBox.pack_start(*model);

  show_all();
}

ModelViewer::~ModelViewer() {
  delete model;
}

