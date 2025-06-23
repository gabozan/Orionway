########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from api_client import detect_objects
from audio_utils import generate_audio, play_audio

# ==================================================================================================================== #


def detect_object_in_hand(image_path: str) -> dict | None:
    """
    Detecta l'objecte que la persona porta a la mà, ignorant la detecció de la pròpia persona.

    Retorna:
        dict amb les claus:
            - 'name': nom de la classe de l'objecte detectat.
            - 'gender': 'm' o 'f' segons el gènere de la paraula.
        o None si no es detecta cap objecte
    """
    result = detect_objects(image_path)

    if not result["success"]:
        return None

    detections = result["data"]["detections"]
    if not detections:
        return None

    if detections[0]["class_name"] == "persona":
        if len(detections) > 1:
            detected_object = detections[1]
        else:
            return None
    else:
        detected_object = detections[0]

    return {
        "name": detected_object["class_name"],
        "gender": detected_object["gender"]
    }


def announce_object(detected_object: dict | None):
    """
    Genera i reprodueix l'àudio segons si s'ha detectat un objecte o no.

    Paràmetres:
        detected_object: resultat retornat per la funció detect_object_in_hand()
    """
    if detected_object is None:
        generate_audio("No s'ha detectat cap objecte a la mà.")
    else:
        name = detected_object["name"]
        article = "un" if detected_object["gender"] == "m" else "una"
        phrase = f"Portes {article} {name} a la mà."
        generate_audio(phrase)
    play_audio()