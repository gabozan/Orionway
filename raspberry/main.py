########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from enum import IntEnum
from ArduinoIO import ArduinoIO
import time
from functions import detect_object_in_hand

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

# ----------------------------------------------------------------------
# Estat global i instància de l'ArduinoIO
# ----------------------------------------------------------------------
estat = RobotState.ATURAT
gira_seguent = RobotState.ATURAT
arduino = ArduinoIO()

# ==================================================================================================================== #

def loop():
    global estat
    try:
        print("Connectant amb l'Arduino i iniciant loop...")
        time.sleep(0.5)

        while True:
            # Llegim si l'Arduino ens envia algun codi
            state = arduino.read_instruction()
            if state != -1:
                try:
                    nou_estat = RobotState(state)
                    # Només canviem estat si realment és vàlid
                    print(f"Rebut codi des de l'Arduino: {nou_estat.name} ({state})")
                    estat = nou_estat
                except ValueError:
                    print(f"Codi rebut desconegut: {state}")

            # Ara gestionem el nostre estat Raspberry
            if estat == RobotState.ATURAT:
                # Enviem instrucció ATURAT a Arduino per si de cas
                arduino.send_instruction(RobotState.ATURAT.value)
                time.sleep(0.1)
            elif estat == RobotState.RECONEIX:
                # ESTAT: RECONEIX
                # La Raspberry ha de fer la detecció d'objectes
                print("Reconeixent l'objecte...")
                objects = detect_object_in_hand()
                print(f"- detectarObjetos() = {objects}")
                # Un cop has acabat la fase de reconeixement envíem ATURAT per dir a l'Arduino que ha acabat correctament
                print("Reconeixement finalitzat. Avís a l'Arduino: ATURAT")
                arduino.send_instruction(RobotState.ATURAT.value)
                # Tornem a ATURAT
                estat = RobotState.ATURAT
                time.sleep(0.1)
            elif estat == RobotState.AVANCA:
                # ESTAT: AVANCA
                # La Raspberry monitoritza la càmera per detectar ZEBRAI
                zebra_detectada = detectarZebra()
                if zebra_detectada:
                    print("Zebra detectada. Enviant ZEBRA_UBICA a l'Arduino...")
                    arduino.send_instruction(RobotState.ZEBRA_UBICA.value)
                    estat = RobotState.ZEBRA_UBICA
                else:
                    # Si no hi ha zebra, podem enviem un missatge per seguir avançant
                    arduino.send_instruction(RobotState.AVANCA.value)
                    print("Segueix avançant...")
                time.sleep(0.1)
            elif estat == RobotState.PETICIO:
                # ESTAT: PETICIO
                print("Arduino està en PETICIO. Mentrestant, Raspberry no fa res extra.")
                time.sleep(0.1)
            elif estat == RobotState.GIRA:
                # ESTAT: GIRA
                # Aquí Arduino gestiona el gir ("rotate(angle)") sense intervenció de la Raspberry
                print("Arduino està girant. Raspberry no intervé en el gir.")
                time.sleep(0.1)
            elif estat == RobotState.ZEBRA_UBICA:
                # ESTAT: ZEBRA_UBICA
                # Ara la Raspberry ha de localitzar amb precisió la zebra
                # Un cop ho tingui, envia ZEBRA_ESPERA per avisar l'Arduino
                print("Localitzant zebra amb visió...")
                # TODO: Inicio codigo ZEBRAI
                time.sleep(0.5)
                # TODO: Fin codigo ZEBRAI
                print("Zebra ubicada. Enviant ZEBRA_ESPERA a Arduino.")
                arduino.send_instruction(RobotState.ZEBRA_ESPERA.value)
                estat = RobotState.ZEBRA_ESPERA
                time.sleep(0.1)
            elif estat == RobotState.ZEBRA_ESPERA:
                # ESTAT: ZEBRA_ESPERA
                # TODO: Inicio codigo ZEBRAI
                time.sleep(0.5)
                # TODO: Fin codigo ZEBRAI
                if not zebra_encara:
                    arduino.send_instruction(RobotState.ZEBRA_AVANCA.value)
                    estat = RobotState.ZEBRA_AVANCA
                else:
                    time.sleep(0.2)
            elif estat == RobotState.ZEBRA_AVANCA:
                # ESTAT: ZEBRA_AVANCA
                # Arduino mou cap endavant per acabar de creuar.
                # un cop la zebra (línia o vianants) ja no és detectable, enviem AVANCA.
                print("Arduino avançant sobre la zebra. Comprovo si ja ha creuat.")
                zebra_encara = detectarZebra()
                if not zebra_encara:
                    print("Zebra completada. Enviant AVANCA a Arduino.")
                    arduino.send_instruction(RobotState.AVANCA.value)
                    estat = RobotState.AVANCA
                else:
                    # Esperem una micona més
                    time.sleep(0.2)
            elif estat == RobotState.APROPAMENT:
                # ESTAT: APROPAMENT
                # Funcio per aproximar-nos a una persona
                print("Buscant persona per apropar-me...")
                persona_vista = detectarPersona()
                if persona_vista:
                    # Quan detectem la persona prou a prop, enviem ATURAT perquè l'Arduino pari
                    print("Persona detectada. Enviant ATURAT a Arduino per aturar-me.")
                    arduino.send_instruction(RobotState.ATURAT.value)
                    estat = RobotState.ATURAT
                else:
                    # Encara no la veiem: decidim si enviem ordres per avançar o girar lleugerament.
                    # Per ara enviem AVANCA per anar mirant a la gent...
                    print("Encara no veig persona. Enviant AVANCA per seguir buscant.")
                    arduino.send_instruction(RobotState.AVANCA.value)
                    # REMARCA: Arduino entrarà a l'estat AVANCA quan rebi aquest codi.
                    estat = RobotState.AVANCA
                time.sleep(0.1)
            else:
                # Si rebem algun estat desconegut, mandrem STOP
                print(f"Estat desconegut: {estat}. Tornant a ATURAT.")
                arduino.send_instruction(RobotState.ATURAT.value)
                estat = RobotState.ATURAT
                time.sleep(0.1)
    except KeyboardInterrupt:
        print("\nInterrupció per teclat.")
    finally:
        arduino.close()
        print("Port tancat correctament.")


if __name__ == "__main__":
    loop()