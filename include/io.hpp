#ifndef IO_HPP
#define IO_HPP

#include "ModbusRTUServer.h"
#include <stdint.h>
#include <Arduino.h>
#include <ArduinoModbus.h>
#include <EEPROM.h>

namespace io {

class register_base_c {
public:
    register_base_c() = default;
    ~register_base_c() = default;

    enum class kind_e {
        coil,
        discrete,
        input,
        holding,
    };      

    virtual size_t size() const { return 0; }
    
    kind_e kind() const { return _kind; }

    static constexpr size_t n_coils = 3;
    static constexpr size_t n_discr = 0;
    static constexpr size_t n_input = 0;
    static constexpr size_t n_holdg = 3;

    static size_t kind_offset(kind_e k) {
        switch (k) {
        case kind_e::coil:
            return sizeof(uint16_t);
        case kind_e::discrete:
            return sizeof(uint16_t) + n_coils*sizeof(bool);
        case kind_e::input:
            return sizeof(uint16_t) + (n_coils + n_discr)*sizeof(bool);
        case kind_e::holding:
            return sizeof(uint16_t) + (n_coils + n_discr) * sizeof(bool) +
                   n_input * sizeof(uint16_t);            
        }        
        return 0;
    }      
    
protected:
    kind_e _kind;
    bool _non_volatile;
};

template <typename T, register_base_c::kind_e K>
class register_c : public register_base_c {
    T _value;
    size_t _address;
    size_t _offset;

    void _mb_propagate() {
        switch (K) {
        case register_base_c::kind_e::coil:
            ModbusRTUServer.coilWrite(_address, _value);
            break;
        case register_base_c::kind_e::discrete:
            ModbusRTUServer.discreteInputWrite(_address, _value);
            break;
        case register_base_c::kind_e::input:
            ModbusRTUServer.inputRegisterWrite(_address, _value);
            break;
        case register_base_c::kind_e::holding:
            ModbusRTUServer.holdingRegisterWrite(_address, _value);
            break;
        default:
            break;
        }          
    }     
public:
    register_c(size_t address, T def, bool non_volatile) {
        _kind = K;
        _non_volatile = non_volatile;
        _value = def;
        _address = address;
        _offset = size() * _address + kind_offset(_kind);
    }

    void setup(bool first_time) {
        if (_non_volatile) {
            if (first_time) {
                EEPROM.put(_offset, _value);
            } 
            EEPROM.get(_offset, _value);
        }
        Serial.print(_address); Serial.print(" ");
        Serial.println(_value);
        _mb_propagate();
    }

    void set(T v) {
        if (v != _value && _non_volatile) {
            EEPROM.put(_offset, v);
        }
        _value = v;
        _mb_propagate();
    }

    operator T() const { return _value; }
    size_t size() const override { return sizeof(T); }
};

// FIXME: fragile AF
using register_coil_c = register_c<bool, register_base_c::kind_e::coil>;
using register_disc_c = register_c<bool, register_base_c::kind_e::discrete>;
using register_inpt_c = register_c<uint16_t, register_base_c::kind_e::input>;
using register_hold_c = register_c<uint16_t, register_base_c::kind_e::holding>;

struct registers_s {
    enum index_e {
        armed_idx = 0x00,
        pump_on_idx = 0x01,
        reboot_idx = 0x02,

        bitrate_idx = 0x00,
        id_idx = 0x01,
        accel_idx = 0x02,
    };      

    register_coil_c armed {armed_idx, false, false};
    register_coil_c pump_on {pump_on_idx, false, false};
    register_coil_c reboot {reboot_idx, false, false};

    register_hold_c bitrate {bitrate_idx, 9600, true};
    register_hold_c id {id_idx, 1, true};
    register_hold_c accel {accel_idx, 1, true};
};    

void setup();
void loop();
const registers_s& regs();

}

#endif
