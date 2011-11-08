CFLAGS = `pkg-config opencv --cflags`
LIBS   = `pkg-config opencv --libs`
CC     = g++

SRCS = main.cpp Object.cpp FilterData.cpp Tracker.cpp stats.cpp
MAIN_OBJS = main.o Object.o FilterData.o Tracker.o

all: main stats

run:
	./main

main: ${MAIN_OBJS}
	${CC} ${CFLAGS} -o $@ $^ ${LIBS}

stats: stats.o FilterData.o Object.o
	${CC} ${CFLAGS} -o $@ $^ ${LIBS}

.cpp.o:
	${CC} ${CFLAGS} -c $<

depend:
	makedepend ${SRCS}

clean:
	rm main stats *.o

stats.o: Object.hpp FilterData.hpp
main.o: Object.hpp FilterData.hpp Tracker.hpp
Object.o: Object.hpp
FilterData.o: Object.hpp FilterData.hpp
Tracker.o: Object.hpp FilterData.hpp
