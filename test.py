import time
from pymodbus.client import ModbusSerialClient

client = ModbusSerialClient(
    port='/dev/ttyUSB0',
    baudrate=9600,
    bytesize=8,
    parity='N',
    stopbits=1,
    timeout=2,
    retries=1
)

client.connect()

time.sleep(3)
state = False
while True:
    state = not state
    client.write_coil(1, state, device_id=1)
    time.sleep(1)

client.close()
