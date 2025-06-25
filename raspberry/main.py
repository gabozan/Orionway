########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

import time
from enum import IntEnum
from ArduinoIO import ArduinoIO
from camera_utils import capture_image
from crosswalk import detect_crosswalk, run_zebrai
from object_recognition import detect_object_in_hand, announce_object
from person_interaction import detect_person_info

# ==================================================================================================================== #

# ----------------------------------------------------------------------
# Definim els estats del robot
# ----------------------------------------------------------------------
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


def main():
    img_path = "./tmp/current.jpg"
    arduino = ArduinoIO()
    estat = RobotState.ATURAT
    estat_ant = None

    # Variables per controlar la freqüència de detecció de zebra
    last_zebra_check_time = 0.0
    zebra_check_interval = 3.0

    # Variables per controlar la freqüència de captura de fotos
    last_capture_time = 0.0
    photo_interval = 1.5

    # Variables per detectar persona
    last_person_check_time = 0.0
    person_check_interval = 1.5
    last_person = {'detected': False, 'x_offset': 0.0, 'is_close': False}

    # Variables per controlar la freqüència de enviament dels float
    last_offset_sent = None
    last_offset_time = 0.0
    offset_interval = 1.0
    offset_threshold = 0.05

    try:
        print("Iniciant el bucle principal...")
        while True:
            current_time = time.time()
            # Llegim instrucció de l'Arduino
            code = arduino.read_instruction()
            if code != -1:
                try:
                    estat = RobotState(code)
                    print(f"Rebut codi de l'Arduino: {estat.name} ({code})")
                except ValueError:
                    print(f"Codi desconegut rebut: {code}")

            # Control de freqüència de captura de fotos
            if current_time - last_capture_time >= photo_interval:
                capture_image(img_path)
                last_capture_time = current_time

            ############################################################################################################
            if estat == RobotState.ATURAT:
                if estat != estat_ant:
                    arduino.send_instruction(RobotState.ATURAT.value)
                    estat_ant = estat
            ############################################################################################################
            elif estat == RobotState.RECONEIX:
                obj = detect_object_in_hand(img_path)
                announce_object(obj)
                arduino.send_instruction(RobotState.ATURAT.value)
                estat = RobotState.ATURAT
            ############################################################################################################
            elif estat == RobotState.AVANCA:
                if current_time - last_zebra_check_time >= zebra_check_interval:
                    cross = detect_crosswalk(img_path)
                    last_zebra_check_time = current_time
                else:
                    cross = {"zebra": False}

                if cross.get("zebra", False):
                    print("Zebra detectada, canviant a ZEBRA_UBICA")
                    arduino.send_instruction(RobotState.ZEBRA_UBICA.value)
                    estat = RobotState.ZEBRA_UBICA
                else:
                    if estat != estat_ant:
                        arduino.send_instruction(RobotState.AVANCA.value)
                        estat_ant = estat
            ############################################################################################################
            elif estat == RobotState.ZEBRA_UBICA:
                print("Ubicant zebra. S'executa ZEBRAI")
                #run_zebrai(img_path) #FALTA IMPLEMENTAR!!!-----------------------------------------
                arduino.send_instruction(RobotState.ZEBRA_ESPERA.value)
                estat = RobotState.ZEBRA_ESPERA
            ############################################################################################################
            elif estat == RobotState.ZEBRA_ESPERA:
                if current_time - last_zebra_check_time >= zebra_check_interval:
                    cross = detect_crosswalk(img_path)
                    last_zebra_check_time = current_time
                else:
                    cross = {"traffic_light": "red"}
                zebra_lights = cross.get("traffic_light", None)
                if zebra_lights == "green" or zebra_lights is None:
                    print("Pas de zebra segur, avancem. Seguint ZEBRA_AVANCA")
                    arduino.send_instruction(RobotState.ZEBRA_AVANCA.value)
                    estat = RobotState.ZEBRA_AVANCA
                else:
                    if estat != estat_ant:
                        arduino.send_instruction(RobotState.ZEBRA_ESPERA.value)
                        estat_ant = estat
            ############################################################################################################
            elif estat == RobotState.ZEBRA_AVANCA:
                if current_time - last_zebra_check_time >= zebra_check_interval:
                    cross = detect_crosswalk(img_path)
                    last_zebra_check_time = current_time
                else:
                    cross = {"zebra": True}
                zebra = cross.get("zebra", False)
                if not zebra:
                    print("Creuament completat. Tornant a AVANCA")
                    arduino.send_instruction(RobotState.AVANCA.value)
                    estat = RobotState.AVANCA
                else:
                    if estat != estat_ant:
                        arduino.send_instruction(RobotState.ZEBRA_AVANCA.value)
                        estat_ant = estat
            ############################################################################################################
            elif estat == RobotState.PETICIO:
                if estat != estat_ant:
                    arduino.send_instruction(RobotState.PETICIO.value)
                    estat_ant = estat
            ############################################################################################################
            elif estat == RobotState.GIRA:
                if estat != estat_ant:
                    arduino.send_instruction(RobotState.GIRA.value)
                    estat_ant = estat
            ############################################################################################################
            elif estat == RobotState.APROPAMENT:
                if current_time - last_person_check_time >= person_check_interval:
                    person = detect_person_info(img_path)
                    last_person_check_time = current_time
                    last_person = person
                else:
                    person = last_person

                if not person.get('detected', False):
                    print("Cap persona detectada. Enviant ATURAT")
                    arduino.send_instruction(RobotState.ATURAT.value)
                    estat = RobotState.ATURAT
                else:
                    offset = person.get('x_offset', 0.0)
                    close = person.get('is_close', False)
                    if close:
                        print("Persona a prop. Enviant ATURAT")
                        arduino.send_instruction(RobotState.ATURAT.value)
                        estat = RobotState.ATURAT
                    else:
                        if (last_offset_sent is None or
                                abs(offset - last_offset_sent) > offset_threshold or
                                current_time - last_offset_time >= offset_interval):
                            print(f"Persona detectada amb offset {offset:.2f}. Ajustant trajectòria")
                            arduino.send_instruction(RobotState.APROPAMENT.value)
                            arduino.send_float(offset)
                            last_offset_sent = offset
                            last_offset_time = current_time
            ############################################################################################################
            else:
                print(f"Estat desconegut: {estat}. Tornant a ATURAT.")
                arduino.send_instruction(RobotState.ATURAT.value)
                estat = RobotState.ATURAT
            ############################################################################################################
            time.sleep(0.1)

    except KeyboardInterrupt:
        print("\nInterrupció per teclat.")
    finally:
        arduino.close()
        print("Sortida i tancament completat.")


if __name__ == "__main__":
    main()

