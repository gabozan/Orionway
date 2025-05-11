########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from config import IMAGE_SIZE, DEVICE, CONFIDENCE, PREDICTIONS_DIR
from ultralytics import YOLO
from ultralytics.engine.results import Results
import numpy as np
from pathlib import Path
from typing import Union

# ==================================================================================================================== #

def load_model(path: str) -> YOLO:
    """
    Carrega un model YOLO des de la ruta especificada i llença un error si es produeix un error al carregar el model.

    Paràmetres:
        path (str): Ruta cap a l'arxiu del model (.pt).

    Retorna:
        YOLO: Instància del model carregat.
    """
    try:
        return YOLO(path)
    except Exception as e:
        raise RuntimeError(f"No es pot carregar el model en {path}") from e


def predict_image(model: YOLO,
                  frame: Union[np.ndarray, str],
                  debug=False,
                  model_name: str = None
                  ) -> Results:
    """
    Realitza la predicció sobre una imatge utilitzant un model YOLO carregat.

    Paràmetres:
        model (YOLO): Model de YOLO entrenat.
        frame (np.ndarray o str): Imatge o ruta a la imatge.
        debug (bool): Si és True, guarda les imatges amb les prediccions.
        model_name (str): Nom del subdirectori on es guarda la predicció en mode debug.

    Retorna:
        Results: Objecte de resultats de YOLO amb caixes, classes i confiances.
    """
    project_dir = str(PREDICTIONS_DIR / model_name) if debug and model_name else None
    results = model.predict(
        source=frame,
        imgsz=IMAGE_SIZE,
        device=DEVICE,
        conf=CONFIDENCE,
        save=debug,
        save_txt=debug,
        save_conf=debug,
        project=project_dir,
        name="." if debug else None,
        exist_ok=True
    )
    return results[0]