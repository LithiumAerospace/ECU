#ifndef PACKET_H
#define PACKET_H

#include <string>
#include <iostream>

enum PacketType {
  HEARTBEAT = 0x01,
  TELEMETRY = 0x02,
  IGNITION = 0x03,
  SHUTDOWN = 0x04,
  ACK = 0x05,
  CONF = 0x06,
  INFO = 0x07
};

struct {
  const char* type;
  char data;
} DecodeException;

class Packet {
private:
  PacketType type;
  std::string data;

public:
  Packet();

  int length() {return 10 + this->data.length();}

  PacketType get_type() {return this->type;}
  void set_type(PacketType type) {this->type = type;}

  std::string get_data() {return this->data;}
  void set_data(std::string data) {this->data = data;}

  std::string encode();

  static std::string fletcher16(std::string in);
  static Packet* decode(std::string data);
  static std::string hexs(int in);
};

#endif
