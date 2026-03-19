#include <avr/wdt.h>
#include <Arduino.h>
#include "io.hpp"
#include "ramp.hpp"

static const uint8_t PUMP_OUT = 3;
void (*restart_func)() = NULL;

void setup() {
    io::setup();
    ramp::setup(io::regs().accel);
    Serial.println("[main] Starting sketch");

    pinMode(PUMP_OUT, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    analogWrite(PUMP_OUT, 0);
    digitalWrite(LED_BUILTIN, LOW);

    wdt_enable(WDTO_500MS);
}

void loop() {
    wdt_reset();
    io::loop();
    ramp::loop();
    
    const auto &regs = io::regs();

    if (regs.reboot) {
        restart_func();
    }      

    if (regs.armed && regs.pump_on) {
        ramp::set_target(255);
    } else {
        ramp::set_target(0);
    }
    analogWrite(PUMP_OUT, ramp::duty());
}
