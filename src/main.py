import serial
import time

arduino = serial.Serial('/dev/ttyACM0', 9600)
time.sleep(2)


while True:
    command = input('on / off / exit:\n')
    if command == 'exit':
        break
    arduino.write(f'{command}\n'.encode())

arduino.close()
