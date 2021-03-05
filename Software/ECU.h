#ifndef ECU_H
#define ECU_H

#include <iostream>

#include "lib/packet.h"
#include "Sensors.h"

enum class State {
	IDLE = 0,
	IGNITION = 1,
	FIRING = 2
};

class ECU {
private:
	State state;
	State old_state;
	Sensors* sensors;

	void(*caller)(std::string);

	Packet* getInfo();
	void sendTelem();
	void ack(Packet* p);

	int deltatelem = 0;
	int deltaign = 0;

	void setState(State new_state);
public:
	ECU(void(*s)(std::string));

	void tick(int delta);
	void handlePacket(Packet* p);
	void handlePacket(std::string s);
};

#endif /* ECU_H */
