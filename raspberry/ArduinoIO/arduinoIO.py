import serial
import serial.tools.list_ports
import time
import threading

# Connexió al port
ports = serial.tools.list_ports.comports()
arduino_port = None
for port in ports:
    if "ACM" in port.device or "COM" in port.device:
        arduino_port = port.device
        break
arduino = serial.Serial(arduino_port, 115200)
time.sleep(2)

commands = ["Led off", "Led on"]
stop_event = threading.Event()

def write_thread():

    print("-"*20+"\nCommands:")
    for i, cmd in enumerate(commands):
            print(f"{i} - {cmd}")
    print(f"{len(commands)} - Exit\n"+"-"*20)

    while not stop_event.is_set():
        choice = input()
        if choice.isdigit() and 0 <= int(choice) < len(commands):
            arduino.write(choice.encode())
        elif choice.isdigit() and int(choice) == len(commands):
            print("Exiting...")
            stop_event.set()
            break
        else:
            print("Invalid command. Please try again.")

def read_thread():
    while not stop_event.is_set():
        if arduino.in_waiting > 0:
            try:
                value = int(arduino.readline().decode().strip())
                print("Read value:", value)
            except ValueError:
                pass

t1 = threading.Thread(target=write_thread)
t2 = threading.Thread(target=read_thread)

t1.start()
t2.start()

t1.join()
t2.join()

arduino.close()