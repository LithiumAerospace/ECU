#ifndef ECU_H
#define ECU_H

#if defined(ARDUINO)
#include <packet.h>
#else
#include "lib/packet/packet.h"
#endif
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

	void(*caller)(char *);

	Packet* getInfo();
	void sendTelem();
	void ack(Packet* p);

	int deltatelem = 0;
	int deltaign = 0;

	void setState(State new_state);
public:
	ECU(void(*s)(char *));

	void tick(int delta);
	void handlePacket(Packet* p);
};

#endif /* ECU_H */
