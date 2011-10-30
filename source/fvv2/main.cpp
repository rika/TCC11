#ifdef G_OS_WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#include <gtkmm.h>
#include <gtkglmm.h>

#include "ModelViewer.hpp"

int main(int argc, char **argv) {
  srand((unsigned)time(0));

  Gtk::Main kit(argc, argv);

  Gtk::GL::init(argc, argv);

  printf("TV3D - Model Viewer\n\n");

  string dataFile;
  if (argc >= 2) {
    dataFile = string(argv[1]);
  } else {
    printf("ERROR: Please specify data file.\n\n");

    printf("USAGE: main [datafile]\n");
    printf(" where [datafile] is the path to the data file\n\n");

    return 1;
  }

  ModelViewer app(dataFile);
  kit.run(app);

  return 0;
}
