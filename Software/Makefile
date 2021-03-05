CC = g++

#LINKERFLAG = -lm

SRCS := test.cpp ECU.cpp lib/packet/packet.cpp Sensors.cpp
OBJECTS := $(SRCS:%.cpp=%.o)

all: ${OBJECTS}
	${CC} ${OBJECTS} -o test.out

%.o: %.cpp
	${CC} -c $< -o $(<:%.cpp=%.o)

test: all
	@./test

clean:
	@rm -v *.o
