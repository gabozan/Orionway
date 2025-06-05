########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from config import IMAGE_SIZE, DEVICE, CONFIDENCE, BASE_PATH
from ultralytics import YOLO
from ultralytics.engine.results import Results
from yolov5.models.common import AutoShape
import numpy as np
from pathlib import Path
from typing import Union
import torch

# ==================================================================================================================== #

def load_model(path: Union[str, Path]) -> YOLO:
    """
    Carrega un model YOLO des de la ruta especificada i llença un error si es produeix un error al carregar el model.

    Paràmetres:
        path (str o Path): Ruta cap a l'arxiu del model (.pt).

    Retorna:
        YOLO: Instància del model carregat.
    """
    try:
        return YOLO(str(path))
    except Exception as e:
        raise RuntimeError(f"No es pot carregar el model en {path}") from e


def load_modelv5(path: Union[str, Path]):
    """
    Carrega un model YOLOv5 usant torch.hub des de la ruta especificada.

    Paràmetres:
        path (str o Path): Ruta cap a l'arxiu del model (.pt).

    Retorna:
        model: Instància del model carregat amb torch.hub.
    """
    try:
        model = torch.hub.load(str(BASE_PATH / "yolov5"), 'custom', path=str(path), source='local')
        model.eval()
        model = AutoShape(model)
        return model
    except Exception as e:
        raise RuntimeError(f"No es pot carregar el model en {path}") from e


def predict_image(model: YOLO,
                  frame: Union[np.ndarray, str, Path],
                  ) -> Results:
    """
    Realitza la predicció sobre una imatge utilitzant un model YOLO carregat.

    Paràmetres:
        model (YOLO): Model de YOLO entrenat.
        frame (np.ndarray, str o Path): Imatge o ruta a la imatge.

    Retorna:
        Results: Objecte de resultats de YOLO amb caixes, classes i confiances.
    """
    results = model.predict(
        source=frame,
        imgsz=IMAGE_SIZE,
        device=DEVICE,
        conf=CONFIDENCE
    )
    return results[0]