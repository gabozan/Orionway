########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from enum import IntEnum
from ArduinoIO import ArduinoIO
import time
import random

# ==================================================================================================================== #

class RobotState(IntEnum):
    ATURAT = 0
    RECONEIX = 1
    AVANCA = 2
    GIRA = 3
    PETICIO = 4
    ZEBRA_ESPERA = 5
    ZEBRA_UBICA = 6
    ZEBRA_AVANCA = 7
    APROPAMENT = 8

# ==================================================================================================================== #

estat = RobotState.ATURAT
gira_seguent = RobotState.ATURAT
arduino = ArduinoIO()

# ==================================================================================================================== #

def getTaps():
    return random.choice([False, False, False, True])

def get_buttons():
    return random.choice(["NONE", "BUTTON1_SINGLE_CLICK"])

def stop_motors():
    arduino.send_instruction(RobotState.ATURAT.value)

def executar_avanca():
    arduino.send_instruction(RobotState.AVANCA.value)

# ==================================================================================================================== #

def loop():
    global estat
    try:
        while True:
            print(f"Estat actual: {estat.name}")

            if estat == RobotState.ATURAT:
                arduino.send_instruction(RobotState.ATURAT.value)
                if getTaps():
                    estat = RobotState.APROPAMENT

            elif estat == RobotState.AVANCA:
                executar_avanca()
                btn = get_buttons()
                if btn != "NONE":
                    estat = RobotState.PETICIO

            elif estat == RobotState.PETICIO:
                stop_motors()
                btn = get_buttons()
                if btn == "BUTTON1_SINGLE_CLICK":
                    estat = RobotState.AVANCA
                else:
                    estat = RobotState.ATURAT
            time.sleep(0.5)

    except KeyboardInterrupt:
        print("\nInterrupcio")
    finally:
        arduino.close()
        print("Port tancat correctament")


if __name__ == "__main__":
    loop()