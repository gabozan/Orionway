########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from api_client import detect_zebrai
import matplotlib.pyplot as plt
import numpy as np
import cv2
from utils_crosswalk import *
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

def run_zebrai(image_path: str) -> float:
    
    img = plt.imread(image_path)
    gray = to_uint8(cv2.cvtColor(img, cv2.COLOR_BGR2GRAY))
    try:
        src = noise_filter(gray)
        thr = find_threshold(src)
        if not (0 <= thr <= 255):
            raise ValueError(f"Threshold invàlid: {thr}")
        bw = 255 * (src > thr).astype(np.uint8)
        edges = get_edges(bw)
        filtered_lines = get_filtered_lines(edges)
        filtered_lines = remove_duplicate_lines(filtered_lines, gray.shape)
        if is_empty(filtered_lines):
            raise Exception("No s'han trobat línies")
        
        p_izq, p_der = get_limits(filtered_lines, edges)
        midpoints = get_mid_points(filtered_lines, p_izq, p_der, gray.shape)
        angle_rad = get_angle(midpoints, gray.shape)

        return angle_rad
        
    except:
        return 0.0