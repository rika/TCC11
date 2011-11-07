CFLAGS = `pkg-config opencv --cflags`
LIBS   = `pkg-config opencv --libs`
CC     = g++

SRCS = background.cpp bgpets.cpp BGS.cpp
OBJS = background.o bgpets.o BGS.o

all: BGPets background

BGPets: bgpets.o BGS.o
	${CC} ${CFLAGS} -o $@ $^ ${LIBS}

background: background.o BGS.o
	${CC} ${CFLAGS} -o $@ $^ ${LIBS}

.cpp.o:
	${CC} ${CFLAGS} -c $<

depend:
	makedepend ${SRCS}

clean:
	rm BGPets background *.o

background.o: BGS.h
bgpets.o: BGS.h

