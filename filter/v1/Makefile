CFLAGS = `pkg-config opencv --cflags`
LIBS   = `pkg-config opencv --libs`
CC     = g++

SRCS = main.cpp
OBJS = main.o

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

main.o: condensation.h
condensation.o: condensation.h SampleStat.h

