#include "ECU.h"

ECU* ecu;

unsigned long last_millis = 0;

void handle(char* data) {
  Serial.write(data);
}

void setup() {
  Serial.begin(115200);
  ecu = new ECU(handle);
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long diff = currentMillis - last_millis;
  last_millis = currentMillis;
  ecu->tick(diff);
}
