CFLAGS = `pkg-config opencv --cflags`
LIBS   = `pkg-config opencv --libs`
CC     = g++

SRCS = main.cpp Object.cpp FilterData.cpp
OBJS = main.o Object.o FilterData.o

all: main

run:
	./main

main: ${OBJS}
	${CC} ${CFLAGS} -o $@ $^ ${LIBS}

.cpp.o:
	${CC} ${CFLAGS} -c $<

depend:
	makedepend ${SRCS}

clean:
	rm main *.o

main.o: Object.hpp FilterData.hpp
Object.o: Object.hpp
FilterData.o: Object.hpp FilterData.hpp
