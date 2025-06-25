########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from api_client import detect_zebrai

# ==================================================================================================================== #

def detect_crosswalk(image_path: str) -> dict:
    """
    Detecta un pas de vianants i el color del semàfor associat a partir d'una imatge.

    Retorna:
        dict:
            - "zebra": True si s'ha detectat un pas de vianants.
            - "traffic_light": "green", "red" o None si no es pot determinar el color.
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

    return {
        "zebra": zebra,
        "traffic_light": traffic_light_color
    }


def run_zebrai():
    """
    ALBERT!!!!!!!!!!!!!!!!!!!!!!!!!
    AQUI VA EL CODIGO DE ZEBRAI DESPUES DE DETECTAR UN PASO DE ZEBRA CON LA FUNCION ANTERIOR, PUEDES USAR COMO PARAMETROS
    SI LO NECESITAS EL traffic_light YA QUE LO DEVUELVO EN EL RETURN. EN CUANTO AL RETURN DE ESTA FUNCION, DESPUES DE UBICAR
    AL ROBOT HABRIA QUE MANDARLE AL ARDUINO COMO QUEREMOS QUE SE UBIQUE, ESTO NOSE SI CON EL MISMO ESTADO LO PODRIAMOS GESTIONAR
    O NECESITAMOS UNO ADICIONAL.
    """
    return None