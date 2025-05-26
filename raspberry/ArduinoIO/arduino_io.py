import serial
import serial.tools.list_ports
import time
import struct

# Es pot utilitzar de dues maneres:
#
# 1:
#   arduino = ArduinoIO()
#   arduino.send_instruction(1)
#   ...
#   arduino.close()  ⚠️ NO OBLIDAR
#
# 2:
#   with ArduinoIO() as arduino:
#     arduino.send_instruction(1)
#     ...
#   Aquí es tanca automàticament

class ArduinoIO:
    def __init__(self, baudrate=115200):
        self.arduino = self._connect(baudrate)

    def _connect(self, baudrate):
        ports = serial.tools.list_ports.comports()
        for port in ports:
            if "ACM" in port.device or "COM" in port.device:
                try:
                    arduino = serial.Serial(port.device, baudrate)
                    time.sleep(2)
                    return arduino
                except serial.SerialException:
                    continue
        raise Exception("No s'ha trobat cap port Arduino disponible.")

    def close(self):
        if self.arduino and self.arduino.is_open:
            self.arduino.close()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def __del__(self):
        self.close()

    def send_instruction(self, code: int):
        self.arduino.write(bytes([0x01, code]))

    def send_float(self, value: float):
        self.arduino.write(bytes([0x02]))
        self.arduino.write(struct.pack('<f', value))

    def read_instruction(self):
        if self.arduino.in_waiting >= 2:
            header = self.arduino.read(1)[0]
            if header == 0x01 and self.arduino.in_waiting >= 1:
                return self.arduino.read(1)[0]
        return -1

    def read_float(self):
        if self.arduino.in_waiting >= 5:
            header = self.arduino.read(1)[0]
            if header == 0x02 and self.arduino.in_waiting >= 4:
                return struct.unpack('<f', self.arduino.read(4))[0]
        return None
