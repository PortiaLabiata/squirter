#ifndef IO_HPP
#define IO_HPP

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

    static constexpr size_t n_coils = 2;
    static constexpr size_t n_discr = 0;
    static constexpr size_t n_input = 0;
    static constexpr size_t n_holdg = 2;

    static size_t kind_offset(kind_e k) {
        switch (k) {
        case kind_e::coil:
            return sizeof(uint32_t);
        case kind_e::discrete:
            return sizeof(uint32_t) + n_coils*sizeof(bool);
        case kind_e::input:
            return sizeof(uint32_t) + (n_coils + n_discr)*sizeof(bool);
        case kind_e::holding:
            return sizeof(uint32_t) + (n_coils + n_discr) * sizeof(bool) +
                   n_input * sizeof(int16_t);            
        }        
        return 0;
    }      
    
protected:
    kind_e _kind;
    bool _non_volatile;
};

template <typename T, register_base_c::kind_e K>
class register_c : public register_base_c {
    ModbusServer *_server;
    T _value;
    size_t _address;
    size_t _offset;

    void _mb_propagate() {
        switch (K) {
        case register_base_c::kind_e::coil:
            _server->coilWrite(_address, _value);
            break;
        case register_base_c::kind_e::discrete:
            _server->discreteInputWrite(_address, _value);
            break;
        case register_base_c::kind_e::input:
            _server->inputRegisterWrite(_address, _value);
            break;
        case register_base_c::kind_e::holding:
            _server->holdingRegisterWrite(_address, _value);
            break;
        default:
            break;
        }          
    }     
public:
    register_c(size_t address, T def, bool non_volatile,
               ModbusServer& server) {
        _server = &server;
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
            } else {
                EEPROM.get(_offset, _value);
            }                
        }
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
using register_inpt_c = register_c<int16_t, register_base_c::kind_e::input>;
using register_hold_c = register_c<int16_t, register_base_c::kind_e::holding>;

struct registers_s {
    register_coil_c armed {0x00, false, false, ModbusRTUServer};
    register_coil_c pump_on {0x01, false, false, ModbusRTUServer};
    register_hold_c bitrate {0x00, 9600, true, ModbusRTUServer};
    register_hold_c id {0x01, 1, true, ModbusRTUServer};
};    

void setup();
void loop();
const registers_s& regs();

}

#endif
