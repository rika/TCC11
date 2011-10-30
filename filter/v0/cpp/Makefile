CFLAGS = `pkg-config opencv --cflags`
LIBS   = `pkg-config opencv --libs`
CC     = g++

SRCS = kalman.cpp condensation.cpp main.cpp

all: kalman main

run:
	./main gpoints.conf

main: main.o condensation.o
	${CC} ${CFLAGS} -o $@ $^ ${LIBS}

kalman: kalman.o
	${CC} ${CFLAGS} -o $@ $^ ${LIBS}

.cpp.o:
	${CC} ${CFLAGS} -c $<

depend:
	makedepend ${SRCS}

clean:
	rm *.o


main.o: condensation.h
condensation.o: condensation.h SampleStat.h

