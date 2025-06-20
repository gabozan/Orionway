########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from api_client import detect_objects, detect_person, detect_zebrai
from audio_utils import generate_audio, play_audio

# ==================================================================================================================== #

# LEVON!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# AQUI VA LA FUNCION DE HACER FOTO
# def capture_image(path):
# LA IMAGEN SE GUARDA EN path QUE SERA UN ARCHIVO TEMPORAL QUE SE IRA CREANDO Y BORRANDO A MEDIDA QUE LO NECESITEMOS


def detect_object_in_hand(image_path: str):
    """
    Detecta l'objecte que la persona porta a la mà, ignorant la detecció de la pròpia persona.
    Si no es detecta cap objecte addicional, es genera un missatge informant que no porta res.
    """
    result = detect_objects(image_path)

    if not result["success"]:
        generate_audio("No s'ha pogut detectar cap objecte.")
        play_audio()
        return

    detections = result["data"]["detections"]
    if not detections:
        generate_audio("No hi ha cap objecte a la mà.")
        play_audio()
        return

    if detections[0]["class_name"] == "persona":
        if len(detections) > 1:
            detected_object = detections[1]
        else:
            generate_audio("No hi ha cap objecte a la mà.")
            play_audio()
            return
    else:
        detected_object = detections[0]

    name = detected_object["class_name"]
    article = "un" if detected_object["gender"] == "m" else "una"
    phrase = f"Portes {article} {name} a la mà."
    generate_audio(phrase)
    play_audio()


def detect_crosswalk_zebrai(image_path: str) -> dict:
    """
    Detecta un pas de vianants i el color del semàfor associat a partir d'una imatge.

    Retorna:
        dict:
            - "zebra": True si hi ha pas de vianants
            - traffic_light": "green", "red" o None si no es pot determinar
    """
    result = detect_zebrai(image_path)

    if not result["success"]:
        return {"zebra": False, "traffic_light": None}

    data = result["data"]

    zebra = bool(data.get("crosswalk", False))
    red = bool(data.get("traffic_light_red", False))
    green = bool(data.get("traffic_light_green", False))

    traffic_light_color = None
    if green:
        traffic_light_color = "green"
    elif red:
        traffic_light_color = "red"
    return {"zebra": zebra, "traffic_light": traffic_light_color}


def run_zebrai():
    """
    ALBERT!!!!!!!!!!!!!!!!!!!!!!!!!
    AQUI VA EL CODIGO DE ZEBRAI DESPUES DE DETECTAR UN PASO DE ZEBRA CON LA FUNCION ANTERIOR, PUEDES USAR COMO PARAMETROS
    SI LO NECESITAS EL traffic_light YA QUE LO DEVUELVO EN EL RETURN. EN CUANTO AL RETURN DE ESTA FUNCION, DESPUES DE UBICAR
    AL ROBOT HABRIA QUE MANDARLE AL ARDUINO COMO QUEREMOS QUE SE UBIQUE, ESTO NOSE SI CON EL MISMO ESTADO LO PODRIAMOS GESTIONAR
    O NECESITAMOS UNO ADICIONAL.
    """
    return None


def detect_and_go_person(image_path: str):
    return None