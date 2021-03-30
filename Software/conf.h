#ifndef CONF_H
#define CONF_H

#define PROTOCOL_VERSION 0x01
#define ENG_MODEL 0x01
#define SERIAL_NUM 0x01
#define SOFTWARE_VERSION 0x01
#define HARDWARE_VERSION 0x01

#define TELEMRATE 100 // hz
#define TELEMRATEMS (1000 / TELEMRATE)

#define TELEMFORMAT "{\"state\":%i,\"old_state\":%i,\"mv1\":%i}"

#define IGNITIONTIME 100 // ms

#define OV_PIN 4
#define FV_PIN 5
#define IGN_PIN 6

#endif /* CONF_H */
