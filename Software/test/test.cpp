#include "../ECU.h"

#include <iostream>
#include <cstring>

void print_info(std::string info) {
	std::cout << "protocol_version 0x" << stoul(info.substr(0, info.find(",")), nullptr, 16) << '\n';
	info.erase(0, info.find(",") + 1);
	std::cout << "eng_model 0x" << stoul(info.substr(0, info.find(",")), nullptr, 16) << '\n';
	info.erase(0, info.find(",") + 1);
	std::cout << "serial_num 0x" << stoul(info.substr(0, info.find(",")), nullptr, 16) << '\n';
	info.erase(0, info.find(",") + 1);
	std::cout << "software_version 0x" << stoul(info.substr(0, info.find(",")), nullptr, 16) << '\n';
	info.erase(0, info.find(",") + 1);
	std::cout << "hardware_version 0x" << stoul(info.substr(0, info.find(",")), nullptr, 16) << '\n';
}

void handler(std::string data) {
	Packet* p = Packet::decode(data);
	switch (p->get_type()) {
		case PacketType::INFO:
			print_info(p->get_data());
			break;
		case PacketType::TELEMETRY:
			std::cout << "d:" << p->get_data() << '\n';
			break;
		case PacketType::ACK:
			std::cout << "ACK: " << p->get_data() << '\n';
			break;
		default:
			std::cout << "t:" << p->get_type() << '\n';
			std::cout << "d:" << p->get_data() << '\n';
	}
}

ECU* ecu;

void send(PacketType type) {
	Packet* p = new Packet();
	p->set_type(type);
	ecu->handlePacket(p->encode());
}

void digitalWrite(int pin, bool state) {
	std::cout << pin << " " << state << '\n';
}

int main(int argc, char const *argv[]) {
	ecu = new ECU(handler);
	send(PacketType::IGNITION);
	for (size_t i = 0; i < 30; i++) {
		if (i == 15 || i == 28) {
			send(PacketType::SHUTDOWN);
		}
		if (i == 18) {
			send(PacketType::IGNITION);
		}
		ecu->tick(20);
	}
	return 0;
}
