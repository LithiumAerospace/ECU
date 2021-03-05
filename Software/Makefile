CC = g++

#LINKERFLAG = -lm

SRCS := test.cpp ECU.cpp lib/packet.cpp Sensors.cpp
OBJECTS := $(SRCS:%.cpp=%.o)

all: ${OBJECTS}
	${CC} ${OBJECTS} -o test.out

%.o: %.cpp
	${CC} -c $< -o $(<:%.cpp=%.o)

copy:
	@echo "Copying..."
	cp ../../../Code/packet/packet.cpp ./lib/
	cp ../../../Code/packet/packet.h ./lib/

test: all
	@./test

clean:
	@rm -v *.o
