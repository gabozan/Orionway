########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from api_client import detect_person

########################################################################################################################

def detect_person_info(image_path: str) -> dict:
    """
    Detecta si hi ha una persona en la imatge i retorna informació rellevant.

    Retorna:
        dict amb les claus:
            - 'detected': True si s'ha detectat una persona
            - 'x_offset': desviació horitzontal de la persona en píxels (positiu si està cap a la dreta)
            - 'is_close': True si la persona està a prop segons el model
    """
    result = detect_person(image_path)

    if not result["success"]:
        return {
            "detected": False,
            "x_offset": None,
            "is_close": False
        }

    data = result["data"]
    return {
        "detected": data.get("detected", False),
        "x_offset": data.get("x_offset"),
        "is_close": data.get("is_close", False)
    }
