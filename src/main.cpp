#include <Arduino.h>
#include "io.hpp"
#include "ramp.hpp"

static const uint8_t PUMP_OUT = 3;

void setup() {
    io::setup();
    ramp::setup(1);
    Serial.println("[main] Starting sketch");

    pinMode(PUMP_OUT, OUTPUT);
    analogWrite(PUMP_OUT, 0);
}
void loop() {
    io::loop();
    ramp::loop();
    
    const auto &regs = io::regs();
    if (regs.armed && regs.pump_on) {
        ramp::set_target(255);
    } else {
        ramp::set_target(0);
    }
    analogWrite(PUMP_OUT, ramp::duty());
}
