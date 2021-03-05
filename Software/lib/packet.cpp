#include "packet.h"

#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <sstream>

Packet::Packet() {

}

std::string Packet::encode() {
  std::string chunk1;
  chunk1 += "#";
  chunk1 += this->hexs(this->length());
  chunk1 += this->hexs(this->type);
  chunk1 += this->data;
  return chunk1 + this->fletcher16(chunk1) + ';';
}

std::string Packet::fletcher16(std::string in)
{
  uint8_t const *data = reinterpret_cast<const uint8_t*>(&in[0]);
  size_t bytes = in.length();
  uint16_t sum1 = 0xff, sum2 = 0xff;

  while (bytes) {
          size_t tlen = bytes > 20 ? 20 : bytes;
          bytes -= tlen;
          do {
                  sum2 += sum1 += *data++;
          } while (--tlen);
          sum1 = (sum1 & 0xff) + (sum1 >> 8);
          sum2 = (sum2 & 0xff) + (sum2 >> 8);
  }
  /* Second reduction step to reduce sums to 8 bits */
  sum1 = (sum1 & 0xff) + (sum1 >> 8);
  sum2 = (sum2 & 0xff) + (sum2 >> 8);
  uint16_t out = sum2 << 8 | sum1;
  char a[4];
  sprintf(a, "%x", out);
  std::string b(a);
  if (b.length() % 2 != 0) {
    b.insert(0, "0");
  }
  return b;
}

std::string Packet::hexs(int in) {
  char len[2];
  sprintf(len, "%x", in);
  if (in <= 15) {
    len[1] = len[0];
    len[0] = '0';
  }
  std::string out;
  out += len[0];
  out += len[1];
  return out;
}

Packet* Packet::decode(std::string data) {
  Packet* out = new Packet();
  auto raw = data.c_str();
  // CHECK START BYTE
  if (raw[0] != '#') {
    throw 1;
  }
  // CHECK LENGTH
  std::string temp;
  temp += raw[1];
  temp += raw[2];
  int length = std::stoi(temp, 0, 16);
  if (length != data.length()) {
    throw 2;
  }
  // CHECK END BYTE
  if (raw[length-1] != ';') {
    throw 3;
  }
  // GET TYPE
  char typea[2];
  typea[0] = raw[3];
  typea[1] = raw[4];
  int type = strtol(typea, NULL, 16);
  out->set_type(static_cast<PacketType>(type));
  // CHECK CHECKSUM
  char old_checksum[4];
  old_checksum[0] = raw[length - 5];
  old_checksum[1] = raw[length - 4];
  old_checksum[2] = raw[length - 3];
  old_checksum[3] = raw[length - 2];
  std::string checksum = Packet::fletcher16(data.substr(0, length - 5));
  if (old_checksum != checksum) {
    throw 4;
  }
  // CONVERT DATA
  out->set_data(data.substr(5, length - 10));
  return out;
}
