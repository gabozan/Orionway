########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from config import MODEL_OBJECT_DETECTOR, PROCESSED_DIR
from ultralytics.engine.results import Results
from yolo_utils import load_model, predict_image
from image_utils import load_image, save_image
from preprocess import letterbox
from postprocess import calculate_class_center
from typing import Optional
from pathlib import Path

# ==================================================================================================================== #

def detect_objects(image_path: Path) -> Optional[int]:
    """
    Detecta objectes en una imatge donada.

    Paràmetres:
        image_path (Path): Ruta a la imatge original.

    Retorna:
        int: Índex de la classe de l’objecte més proper al centre.
        None: Si no s’ha detectat cap objecte.
    """
    model = load_model(MODEL_OBJECT_DETECTOR)
    img = load_image(image_path)
    processed_img, _, _ = letterbox(img, (640, 640))
    processed_img_path = PROCESSED_DIR / f"{image_path.stem}.jpg"
    save_image(processed_img_path, processed_img)
    objects = predict_image(model, processed_img_path)
    cls = calculate_class_center(objects, processed_img)
    return cls