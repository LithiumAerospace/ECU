#include "ECU.h"

#include "conf.h"

#if defined(ARDUINO)
#include "Arduino.h"
#else
#include <cstdio>
#include <cstring>
#endif

void digitalWrite(int n, bool v);

ECU::ECU(void(*s)(char *)) :
		state(State::IDLE),
		old_state(State::IDLE),
		caller(s),
		sensors(new Sensors()) {

}

void ECU::tick(int delta) {
	sensors->update();
	deltatelem += delta;
	if (deltatelem >= TELEMRATEMS) {
		sendTelem();
		deltatelem = 0;
	}
	if (old_state != state) {
		switch (state) {
			case State::IDLE:
				digitalWrite(FV_PIN, false);
				digitalWrite(OV_PIN, false);
				digitalWrite(IGN_PIN, false); // incase shutdown was sent before ignition finishes
				break;
			case State::IGNITION:
				digitalWrite(FV_PIN, true);
				digitalWrite(OV_PIN, true);
				digitalWrite(IGN_PIN, true);
				break;
			case State::FIRING:
				digitalWrite(IGN_PIN, false);
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
			char buf[256];
			this->getInfo()->encode(buf);
			caller(buf);
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

Packet* ECU::getInfo() {
	Packet* out = new Packet();
	out->set_type(PacketType::INFO);
	char data[200];
	sprintf(data, "%d", PROTOCOL_VERSION);
	strcat(data, ",");
	sprintf(data + strlen(data), "%d", ENG_MODEL);
	strcat(data, ",");
	sprintf(data + strlen(data), "%d", SERIAL_NUM);
	strcat(data, ",");
	sprintf(data + strlen(data), "%d", SOFTWARE_VERSION);
	strcat(data, ",");
	sprintf(data + strlen(data), "%d", HARDWARE_VERSION);
	strcat(data, ",");
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
	p->set_data(buf);
	char out[256];
  p->encode(out);
  caller(out);
}

void ECU::ack(Packet* p) {
	Packet* out = new Packet();
	out->set_type(PacketType::ACK);
	char temp[256];
	p->encode(temp);
	char old_checksum[5];
  memcpy( old_checksum, &temp[strlen(temp) - 5], 4 );
  old_checksum[4] = '\0';
	out->set_data(old_checksum);
  char buf[256];
  out->encode(buf);
	caller(buf);
}
