#ifndef RAMP_HPP
#define RAMP_HPP

#include <stdint.h>
#include <Arduino.h>

namespace ramp {

enum class stage_e {
    up,
    middle,
    down,
};  

void setup(uint8_t accel);
void loop();
void set_target(uint8_t target);
uint8_t duty();

}

#endif
