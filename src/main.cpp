#include <avr/wdt.h>
#include <Arduino.h>
#include "io.hpp"
#include "ramp.hpp"
#include "util.hpp"

static const uint8_t PUMP_OUT = 3;
void (*restart_func)() = NULL;

soft_timer_t pump_on_timer(1000);

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
    
    auto &regs = io::regs();

    if (regs.reboot) {
        Serial.println("[main] Restarting");
        Serial.flush();
        restart_func();
    }      

    auto pump_on_commanded = regs.armed && regs.pump_on;
    static auto pump_power_on = false;

    if (pump_on_commanded) {
        if (!pump_power_on) {
            pump_power_on = true;
            pump_on_timer.reset();
        }          
        ramp::set_target(255);
    } else {
        ramp::set_target(0);            
    }        

    if (pump_power_on && pump_on_timer.expired()) {
        regs.pump_on = false;
        pump_power_on = false;
    }        

    analogWrite(PUMP_OUT, ramp::duty());
}
