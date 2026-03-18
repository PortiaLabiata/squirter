#include "ramp.hpp"

static uint8_t _accel;
static ramp::stage_e _stage = ramp::stage_e::middle;

static uint8_t _target_duty = 0;
static uint8_t _current_duty = 0;

static uint32_t _prev_loop_ms = 0;

namespace ramp {

void setup(uint8_t accel) { _accel = accel; }

void loop() {
    auto _now_ms = millis();
    if (_now_ms - _prev_loop_ms < 1) {
        return;
    }      

    switch (_stage) {
    case stage_e::middle:
        break;
    case stage_e::up: 
        _current_duty += _accel * (_now_ms - _prev_loop_ms);
        if (_current_duty >= _target_duty) {
            _current_duty = _target_duty;
            _stage = stage_e::middle;
        }          
        break;
    case stage_e::down: 
        _current_duty -= _accel * (_now_ms - _prev_loop_ms);
        if (_current_duty <= _target_duty) {
            _current_duty = _target_duty;
            _stage = stage_e::middle;
        }            
        break;
    default:
        break;
    }

    _prev_loop_ms = millis();
}

void set_target(uint8_t target) {
    _target_duty = target;
    if (_target_duty < _current_duty) {
        _stage = stage_e::down;
    } else if (_target_duty > _current_duty) {
        _stage = stage_e::up;
    } else if (_target_duty == _current_duty) {
        _stage = stage_e::middle;
    }
}

uint8_t duty() { return _current_duty; }

}
