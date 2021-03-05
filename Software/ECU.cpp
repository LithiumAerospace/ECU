#include "ECU.h"

#include "conf.h"

#include <cstdio>

void digitalWrite(std::string n, bool v);

ECU::ECU(void(*s)(std::string)) :
		state(State::IDLE),
		old_state(State::IDLE),
		caller(s),
		sensors(new Sensors()) {

}

void ECU::tick(int delta) {
	//printf("%i\n", delta);
	sensors->update();
	deltatelem += delta;
	if (deltatelem >= TELEMRATEMS) {
		sendTelem();
		deltatelem = 0;
	}
	if (old_state != state) {
		switch (state) {
			case State::IDLE:
				digitalWrite("fuelvalve", false);
				digitalWrite("oxygenvalve", false);
				digitalWrite("igniter", false); // incase shutdown was sent before ignition finishes
				break;
			case State::IGNITION:
				digitalWrite("fuelvalve", true);
				digitalWrite("oxygenvalve", true);
				digitalWrite("igniter", true);
				break;
			case State::FIRING:
				digitalWrite("igniter", false);
				break;
		}
		old_state = state;
	}
	switch (state) {
		case State::IGNITION:
			deltaign += delta;
			if (deltaign >= IGNITIONTIME) {
				setState(State::FIRING);
				deltaign = 0;
			}
			break;
	}
}

void ECU::setState(State new_state) {
	old_state = state;
	state = new_state;
}

/*****************************************************************************/
/******************************** PACKET STUFF *******************************/
/*****************************************************************************/

void ECU::handlePacket(Packet* p) {
	switch (p->get_type()) {
		case PacketType::INFO:
			caller(this->getInfo()->encode());
			break;
		case PacketType::IGNITION:
			if (state == State::IGNITION || state == State::FIRING) {
				printf("Already running, ignoring ignition command\n");
				break;
			}
			setState(State::IGNITION);
			ack(p);
			break;
		case PacketType::SHUTDOWN:
			if (state == State::IDLE) {
				printf("Not running, ignoring shutdown command\n");
				break;
			}
			setState(State::IDLE);
			ack(p);
			break;
		default:
			printf("Unrecognized Packet Type\n");
	}
}

void ECU::handlePacket(std::string s) {
	this->handlePacket(Packet::decode(s));
}

Packet* ECU::getInfo() {
	Packet* out = new Packet();
	out->set_type(PacketType::INFO);
	std::string data;
	char temp[6];
	sprintf(temp, "%d", PROTOCOL_VERSION);
	data += temp;
	data += ",";
	sprintf(temp, "%d", ENG_MODEL);
	data += temp;
	data += ",";
	sprintf(temp, "%d", SERIAL_NUM);
	data += temp;
	data += ",";
	sprintf(temp, "%d", SOFTWARE_VERSION);
	data += temp;
	data += ",";
	sprintf(temp, "%d", HARDWARE_VERSION);
	data += temp;
	out->set_data(data);
	return out;
}

void ECU::sendTelem() {
	Packet* p = new Packet();
	p->set_type(PacketType::TELEMETRY);
	char buf[245];
	sprintf(buf, TELEMFORMAT,
		(int) state,
		(int) old_state,
		sensors->mock_value_1
	);
	p->set_data(std::string(buf));
	caller(p->encode());
}

void ECU::ack(Packet* p) {
	Packet* out = new Packet();
	out->set_type(PacketType::ACK);
	std::string temp = p->encode();
	out->set_data(temp.substr(temp.length() - 5, 4));
	caller(out->encode());
}
