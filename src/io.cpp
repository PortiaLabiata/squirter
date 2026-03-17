#include "io.hpp"

static io::registers_s _regs;

namespace io {

void setup() {
    Serial.begin(_regs.bitrate);
    Serial.println("[IO] Setting up server");

    auto& server = ModbusRTUServer;
    if (server.begin(_regs.id, _regs.bitrate)) {
        Serial.println("[IO] Server OK");
    } else {
        Serial.println("[IO] Server ERROR");
        while (1)
            ;
    }

    server.configureCoils(0x00, register_base_c::n_coils);
    server.configureDiscreteInputs(0x00, register_base_c::n_discr);
    server.configureInputRegisters(0x00, register_base_c::n_input);
    server.configureHoldingRegisters(0x00, register_base_c::n_holdg);

    EEPROM.begin();

    bool first_time = false;
    uint16_t magic = 0;
    
    EEPROM.get(0, magic);
    if (magic != 0xDEAD) {
        Serial.println("[IO] EEPROM written first fime");
        first_time = true;
        magic = 0xDEAD;
        EEPROM.put(0, magic);
    }        
    
    _regs.armed.setup(first_time);
    _regs.pump_on.setup(first_time);
    _regs.bitrate.setup(first_time);
    _regs.id.setup(first_time);

    Serial.println("[IO] Initialized registers");
}

void loop() {
    auto& server = ModbusRTUServer;
    if (!server.poll()) {
        return;
    }

    _regs.armed.set(server.coilRead(0x00));
    _regs.pump_on.set(server.coilRead(0x01));

    _regs.bitrate.set(server.holdingRegisterRead(0x00));
    _regs.id.set(server.holdingRegisterRead(0x01));
}

const registers_s& regs() { return _regs; }

}
