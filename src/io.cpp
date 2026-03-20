#include "io.hpp"

static io::registers_s _regs;

#define MAGIC ((uint16_t)0xDEAD)
namespace io {

void setup() {
    Serial.begin(9600);
    EEPROM.begin();

    bool first_time = false;
    uint16_t magic = 0;
    
    EEPROM.get(0, magic);
    if (magic != MAGIC) {
        Serial.println("[IO] EEPROM written first time");
        first_time = true;
        magic = MAGIC;
        EEPROM.put(0, magic);
    }        
    _regs.bitrate.setup(first_time);

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
    
    _regs.bitrate.setup(false);
    _regs.armed.setup(first_time);
    _regs.pump_on.setup(first_time);
    _regs.reboot.setup(first_time);
    _regs.id.setup(first_time);
    _regs.accel.setup(first_time);

    Serial.println("[IO] Initialized registers");
}

void loop() {
    auto& server = ModbusRTUServer;
    if (!server.poll()) {
        return;
    }

    _regs.armed.set(server.coilRead(registers_s::armed_idx));
    _regs.pump_on.set(server.coilRead(registers_s::pump_on_idx));
    _regs.reboot.set(server.coilRead(registers_s::reboot_idx));

    _regs.bitrate.set(server.holdingRegisterRead(registers_s::bitrate_idx));
    _regs.id.set(server.holdingRegisterRead(registers_s::id_idx));
    _regs.accel.set(server.holdingRegisterRead(registers_s::accel_idx));
}

registers_s& regs() { return _regs; }

}
