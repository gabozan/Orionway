########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from api_client import detect_objects, detect_person, detect_zebrai
from audio_utils import generate_audio, play_audio

# ==================================================================================================================== #



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


