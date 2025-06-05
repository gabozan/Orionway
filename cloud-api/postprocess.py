########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from translations import CLASS_TRANSLATIONS
from ultralytics.engine.results import Results
from typing import List, Dict, Tuple
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


def analyze_person(results: Results, image: np.ndarray) -> Tuple[bool, float, bool]:
    """
    Analitza si s'ha detectat una persona a la imatge i extreu informació rellevant.

    Paràmetres:
        results (Results): Resultats de la inferència del model YOLO.
        image (np.ndarray): Imatge original en format BGR.

    Retorna:
        (bool, float, bool):
            - Detecció de persona (True/False),
            - Desviació horitzontal respecte al centre de la imatge (en píxels),
            - True si la persona ocupa més del 50% de la superfície de la imatge (indicador de proximitat).
    """
    h, w = image.shape[:2]
    cx = w / 2

    for r in results:
        for box in r.boxes:
            if int(box.cls[0]) == 0:
                x1, y1, x2, y2 = box.xyxy[0].tolist()
                box_x = (x1 + x2) / 2
                x_deviation = box_x - cx
                box_w, box_h = x2 - x1, y2 - y1
                area_ratio = (box_w * box_h) / (w * h)
                big_person = area_ratio >= 0.7
                return True, x_deviation, big_person
    return False, np.inf, False


def analyze_crosswalk_and_traffic_lights(results) -> List[bool]:
    """
    Analitza la presència de pas de vianants i semàfors vermell i verd.

    - "Zebra_Cross" -> detecta pas de vianants
    - "R_Signal" -> detecta semàfor vermell
    - "G_Signal" -> detecta semàfor verd

    Retorna:
        List[bool]: [hi_ha_pas_de_vianants, hi_ha_sem_vermell, hi_ha_sem_verd]
    """
    detected = [False, False, False]
    names = results.pandas().xyxy[0]['name'].tolist()
    print(names)
    for name in names:
        if name == "Zebra_Cross":
            detected[0] = True
        elif name == "R_Signal":
            detected[1] = True
        elif name == "G_Signal":
            detected[2] = True
    return detected