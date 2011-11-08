CFLAGS = -Wall -O2 `pkg-config opencv --cflags`
LIBS   = `pkg-config opencv --libs` -lGL -lglut -lGLU
CC     = g++

SRCS = main.cpp Object.cpp FilterData.cpp Tracker.cpp stats.cpp animation_filter.cpp
MAIN_OBJS = Object.o FilterData.o Tracker.o

all: main stats animation_filter

run:
	./main

main: main.o ${MAIN_OBJS}
	${CC} ${CFLAGS} -o $@ $^ ${LIBS}

stats: stats.o FilterData.o Object.o
	${CC} ${CFLAGS} -o $@ $^ ${LIBS}

animation_filter: animation_filter.o ${MAIN_OBJS}
	${CC} ${CFLAGS} -o $@ $^ ${LIBS}

.cpp.o:
	${CC} ${CFLAGS} -c $<

depend:
	makedepend ${SRCS}

clean:
	rm main stats *.o

animation_filter.o: Object.hpp FilterData.hpp Tracker.hpp
stats.o: Object.hpp FilterData.hpp
main.o: Object.hpp FilterData.hpp Tracker.hpp
Object.o: Object.hpp
FilterData.o: Object.hpp FilterData.hpp
Tracker.o: Object.hpp FilterData.hpp
