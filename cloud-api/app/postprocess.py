########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from translations import CLASS_TRANSLATIONS
from ultralytics.engine.results import Results
from typing import List, Dict
import numpy as np

# ==================================================================================================================== #

def calculate_class_center(results: Results, image: np.ndarray) -> List[Dict]:
    """
    Calcula la distància de cada detecció al centre de la imatge i les ordena de més propera a més llunyana.

    Paràmetres:
        results (Results): Resultats de la inferència del model YOLO.
        image (np.ndarray): Imatge original en format BGR.

    Retorna:
        List[Dict]: Llista de deteccions ordenades per proximitat al centre. Cada diccionari conté:
            - class_name (str): Nom de la classe en català.
            - gender (str): Gènere gramatical de la classe ('m' o 'f').
            - distance (float): Distància euclidiana entre el centre de la caixa i el centre de la imatge.
    """
    if len(results.boxes.xyxy) == 0:
        return []

    h, w = image.shape[:2]
    cx, cy = w / 2, h / 2

    detections = []
    for box, cls in zip(results.boxes.xyxy, results.boxes.cls):
        x1, y1, x2, y2 = box.tolist()
        box_cx = (x1 + x2) / 2
        box_cy = (y1 + y2) / 2
        dist = ((box_cx - cx) ** 2 + (box_cy - cy) ** 2) ** 0.5
        name, gender = CLASS_TRANSLATIONS[int(cls)]
        detections.append({
            "class_name": name,
            "gender": gender,
            "distance": dist
        })
    detections.sort(key=lambda d: d["distance"])
    return detections