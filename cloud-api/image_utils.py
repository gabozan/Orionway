########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

import cv2
import numpy as np

# ==================================================================================================================== #

def load_image_from_bytes(file_bytes: bytes) -> np.ndarray:
    """
    Carrega una imatge a partir de bytes en format BGR.

    Paràmetres:
        file_bytes (bytes): Contingut binari de la imatge.

    Retorna:
        np.ndarray: Imatge carregada en format BGR.
    """
    np_arr = np.frombuffer(file_bytes, np.uint8)
    image = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    return image