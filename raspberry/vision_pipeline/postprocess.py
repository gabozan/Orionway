########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from ultralytics.engine.results import Results
from typing import Optional
import numpy as np

# ==================================================================================================================== #

def calculate_class_center(results: Results, image: np.ndarray) -> Optional[int]:
    """
    Retorna la classe de la detecció més propera al centre de la imatge.

    Paràmetres:
        results (Results): Resultat del model YOLO.
        image (np.ndarray): Imatge processada.

    Retorna:
        int: Índex de la classe de la caixa més propera al centre.
        None: Si no hi ha deteccions.
    """
    if len(results.boxes.xyxy) == 0:
        return None

    h, w = image.shape[:2]
    cx, cy = w / 2, h / 2

    min_dist = np.inf
    center_class = None

    for box, cls in zip(results.boxes.xyxy, results.boxes.cls):
        x1, y1, x2, y2 = box.tolist()
        box_cx = (x1 + x2) / 2
        box_cy = (y1 + y2) / 2
        dist = ((box_cx - cx) ** 2 + (box_cy - cy) ** 2) ** 0.5

        if dist < min_dist:
            min_dist = dist
            center_class = int(cls)
    return center_class