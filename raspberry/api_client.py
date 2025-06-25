########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

import requests

# ==================================================================================================================== #

BASE_URL = "https://yolo-api-872268169156.europe-west1.run.app"

# ==================================================================================================================== #

def detect_objects(image_path: str):
    """
    Envia una imatge i retorna una llista d'objectes detectats ordenats per proximitat al centre.
    """
    url = f"{BASE_URL}/predict/objects"
    return send_image_request(url, image_path)


def detect_person(image_path: str):
    """
    Detecta si hi ha una persona, retorna la desviació horitzontal i si està a prop.
    """
    url = f"{BASE_URL}/predict/person"
    return send_image_request(url, image_path)


def detect_zebrai(image_path: str):
    """
    Detecta pas de vianants i semàfors en la imatge.
    """
    url = f"{BASE_URL}/predict/zebrai"
    return send_image_request(url, image_path)


def send_image_request(url: str, image_path: str) -> dict:
    """
    Funció interna per enviar una imatge a un endpoint.

    Retorna:
        dict: Diccionari amb les claus:
            - "success": True si la petició ha tingut èxit, False en cas contrari.
            - "data": Contingut JSON de la resposta si tot ha anat bé.
            - "error": Missatge d'error si hi ha hagut algun problema.
    """
    try:
        with open(image_path, 'rb') as img:
            files = {'file': img}
            response = requests.post(url, files=files)

        if response.status_code == 200:
            return {
                "success": True,
                "data": response.json()
            }
        else:
            return {
                "success": False,
                "error": f"Error {response.status_code}: {response.text}"
            }
    except Exception as e:
        return {
            "success": False,
            "error": f"Excepció en enviar la imatge a {url}: {e}"
        }