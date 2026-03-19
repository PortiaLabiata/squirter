import time
import cmd
from pymodbus.client import ModbusSerialClient

class MBShell(cmd.Cmd):
    intro = 'ModbusShell. Type \'help\' for help'
    prompt = 'MB> '
    file = None

    def __init__(self):
        super().__init__()

    def do_connect(self, arg):
        args = arg.split()
        self.client = ModbusSerialClient(
            port=args[0],
            baudrate=int(args[1]),
            bytesize=8,
            parity='N',
            stopbits=1,
            timeout=2,
            retries=1)
        self.client.connect()
        time.sleep(3)

    def do_setid(self, arg):
        self.devid = int(arg)

    def do_wreg(self, arg):
        args = arg.split()
        reg, val = int(args[0], 16), int(args[1])
        if not self._check_client():
            return False
        self.client.write_register(reg, val, device_id=self.devid)

    def do_rreg(self, arg):
        reg = int(arg, 16)
        if not self._check_client():
            return False
        print(
            self.client.read_holding_registers(reg, device_id=self.devid))

    def do_rcoil(self, arg):
        reg = int(arg, 16)
        if not self._check_client():
            return False
        print(
            self.client.read_coils(reg, device_id=self.devid))

    def _check_client(self):
        if self.client is None:
            print('Error: not connected')
            return False
        else:
            return True

    def do_arm(self, arg):
        if not self._check_client():
            return False
        self.client.write_coil(0, True, device_id=self.devid)
        
    def do_on(self, arg):
        if not self._check_client():
            return False
        self.client.write_coil(1, True, device_id=self.devid)

    def do_disarm(self, arg):
        if not self._check_client():
            return False
        self.client.write_coil(0, False, device_id=self.devid)

    def do_off(self, arg):
        if not self._check_client():
            return False
        self.client.write_coil(1, False, device_id=self.devid)

    def do_reboot(self, arg):
        if not self._check_client():
            return False
        self.client.write_coil(2, True, device_id=self.devid)

    def preloop(self):
        self.client = None
        self.devid = 1

    def postloop(self):
        if self.client is not None:
            self.client.close()

    def do_exit(self, arg):
        return True

if __name__ == '__main__':
    cli = MBShell()
    cli.cmdloop()

