CFLAGS = `pkg-config opencv gtkglextmm-1.2 --cflags`
CC = g++ -O2 
LIBS = `pkg-config opencv gtkglextmm-1.2 --libs`
SRCS = main.cpp ModelViewer.cpp SceneModel.cpp Quaternion.cpp Camera.cpp Vector.cpp Point.cpp Boundary.cpp Edge.cpp Billboarding.cpp ResourceParser.cpp ResourceData.cpp Object.cpp TextureManager.cpp VDTM.cpp
OBJS = ModelViewer.o SceneModel.o Quaternion.o Camera.o Vector.o Point.o Boundary.o Edge.o Billboarding.o ResourceParser.o ResourceData.o Object.o TextureManager.o VDTM.o

main: main.o ${OBJS}
	${CC} ${CFLAGS} -o $@ main.o ${OBJS} ${LIBS}

.cpp.o:
	${CC} ${CFLAGS} -c $<

depend:
	makedepend ${SRCS}

clean:
	rm *.o *~ main Makefile.bak > /dev/null 2>&1




# DO NOT DELETE

main.o: ModelViewer.hpp SceneModel.hpp /usr/include/GL/gl.h
main.o: /usr/include/GL/glext.h /usr/include/inttypes.h
main.o: /usr/include/features.h /usr/include/sys/cdefs.h
main.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
main.o: /usr/include/gnu/stubs-32.h /usr/include/stdint.h
main.o: /usr/include/bits/wchar.h /usr/include/GL/glu.h
main.o: /usr/include/sys/time.h /usr/include/bits/types.h
main.o: /usr/include/bits/typesizes.h /usr/include/time.h
main.o: /usr/include/bits/time.h /usr/include/sys/select.h
main.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
main.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
main.o: /usr/include/bits/confname.h /usr/include/getopt.h Camera.hpp
main.o: Vector.hpp Quaternion.hpp Boundary.hpp Edge.hpp Point.hpp
main.o: /usr/include/GL/glut.h /usr/include/GL/freeglut_std.h
main.o: Billboarding.hpp ResourceData.h ResourceParser.h Object.hpp
main.o: TextureManager.hpp VDTM.hpp
ModelViewer.o: ModelViewer.hpp SceneModel.hpp /usr/include/GL/gl.h
ModelViewer.o: /usr/include/GL/glext.h /usr/include/inttypes.h
ModelViewer.o: /usr/include/features.h /usr/include/sys/cdefs.h
ModelViewer.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
ModelViewer.o: /usr/include/gnu/stubs-32.h /usr/include/stdint.h
ModelViewer.o: /usr/include/bits/wchar.h /usr/include/GL/glu.h
ModelViewer.o: /usr/include/sys/time.h /usr/include/bits/types.h
ModelViewer.o: /usr/include/bits/typesizes.h /usr/include/time.h
ModelViewer.o: /usr/include/bits/time.h /usr/include/sys/select.h
ModelViewer.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
ModelViewer.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
ModelViewer.o: /usr/include/bits/confname.h /usr/include/getopt.h Camera.hpp
ModelViewer.o: Vector.hpp Quaternion.hpp Boundary.hpp Edge.hpp Point.hpp
ModelViewer.o: /usr/include/GL/glut.h /usr/include/GL/freeglut_std.h
ModelViewer.o: Billboarding.hpp ResourceData.h ResourceParser.h Object.hpp
ModelViewer.o: TextureManager.hpp VDTM.hpp
SceneModel.o: SceneModel.hpp /usr/include/GL/gl.h /usr/include/GL/glext.h
SceneModel.o: /usr/include/inttypes.h /usr/include/features.h
SceneModel.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
SceneModel.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
SceneModel.o: /usr/include/stdint.h /usr/include/bits/wchar.h
SceneModel.o: /usr/include/GL/glu.h /usr/include/sys/time.h
SceneModel.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
SceneModel.o: /usr/include/time.h /usr/include/bits/time.h
SceneModel.o: /usr/include/sys/select.h /usr/include/bits/select.h
SceneModel.o: /usr/include/bits/sigset.h /usr/include/unistd.h
SceneModel.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
SceneModel.o: /usr/include/getopt.h Camera.hpp Vector.hpp Quaternion.hpp
SceneModel.o: Boundary.hpp Edge.hpp Point.hpp /usr/include/GL/glut.h
SceneModel.o: /usr/include/GL/freeglut_std.h Billboarding.hpp ResourceData.h
SceneModel.o: ResourceParser.h Object.hpp TextureManager.hpp VDTM.hpp
Quaternion.o: Quaternion.hpp
Camera.o: Camera.hpp Vector.hpp Quaternion.hpp
Vector.o: Vector.hpp
Point.o: Point.hpp
Boundary.o: Boundary.hpp Edge.hpp Point.hpp Vector.hpp /usr/include/GL/gl.h
Boundary.o: /usr/include/GL/glext.h /usr/include/inttypes.h
Boundary.o: /usr/include/features.h /usr/include/sys/cdefs.h
Boundary.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
Boundary.o: /usr/include/gnu/stubs-32.h /usr/include/stdint.h
Boundary.o: /usr/include/bits/wchar.h /usr/include/GL/glut.h
Boundary.o: /usr/include/GL/freeglut_std.h /usr/include/GL/glu.h
Edge.o: Edge.hpp Point.hpp Vector.hpp
Billboarding.o: Billboarding.hpp /usr/include/GL/gl.h /usr/include/GL/glext.h
Billboarding.o: /usr/include/inttypes.h /usr/include/features.h
Billboarding.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Billboarding.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
Billboarding.o: /usr/include/stdint.h /usr/include/bits/wchar.h
Billboarding.o: /usr/include/GL/glu.h /usr/include/GL/glut.h
Billboarding.o: /usr/include/GL/freeglut_std.h Vector.hpp Camera.hpp
Billboarding.o: Quaternion.hpp ResourceData.h ResourceParser.h Object.hpp
Billboarding.o: TextureManager.hpp VDTM.hpp
ResourceParser.o: ResourceParser.h ResourceData.h
ResourceData.o: ResourceData.h
Object.o: Object.hpp Vector.hpp
TextureManager.o: TextureManager.hpp /usr/include/GL/gl.h
TextureManager.o: /usr/include/GL/glext.h /usr/include/inttypes.h
TextureManager.o: /usr/include/features.h /usr/include/sys/cdefs.h
TextureManager.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
TextureManager.o: /usr/include/gnu/stubs-32.h /usr/include/stdint.h
TextureManager.o: /usr/include/bits/wchar.h /usr/include/GL/glu.h
TextureManager.o: /usr/include/GL/glut.h /usr/include/GL/freeglut_std.h
VDTM.o: VDTM.hpp Vector.hpp ResourceData.h /usr/include/GL/gl.h
VDTM.o: /usr/include/GL/glext.h /usr/include/inttypes.h
VDTM.o: /usr/include/features.h /usr/include/sys/cdefs.h
VDTM.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
VDTM.o: /usr/include/gnu/stubs-32.h /usr/include/stdint.h
VDTM.o: /usr/include/bits/wchar.h /usr/include/GL/glu.h
