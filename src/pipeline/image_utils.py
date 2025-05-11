########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

import cv2
import numpy as np
from typing import Union
from pathlib import Path

# ==================================================================================================================== #

def load_image(path: Union[str, Path]):
    """
    Carrega una imatge des del disc en BGR.

    Paràmetres:
        path (str o Path): Ruta de l'arxiu de la imatge.

    Retorna:
        np.ndarray: Imatge en format BGR.
    """
    img = cv2.imread(str(path))
    if img is None:
        raise FileNotFoundError(f"No s'ha pogut carregar la imatge a la ruta: {path}")
    return img


def save_image(path: Union[str, Path], img: np.ndarray):
    """
    Desa una imatge al disc.

    Paràmetres:
        path (str o Path): Ruta on es desarà la imatge.
        img (np.ndarray): Imatge en format BGR que es vol desar.

    Retorna:
        None
    """
    cv2.imwrite(str(path), img)