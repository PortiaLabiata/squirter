#include <Arduino.h>
#include "io.hpp"

void setup() {
    io::setup();
    Serial.println("[main] Starting sketch");

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}
void loop() {
    io::loop();
    const auto &regs = io::regs();
    if (regs.armed && regs.pump_on) {
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
        digitalWrite(LED_BUILTIN, LOW);
    }        
}
