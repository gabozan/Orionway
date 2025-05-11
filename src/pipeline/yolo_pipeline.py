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
from pathlib import Path

# ==================================================================================================================== #

def detect_objects(image_path: Path) -> Results:
    """
    Detecta objectes en una imatge donada.

    Paràmetres:
        image_path (Path): Ruta a la imatge original.

    Retorna:
        Results: Resultats de la predicció del model YOLO.
    """
    model = load_model(MODEL_OBJECT_DETECTOR)
    img = load_image(image_path)
    processed_img, _, _ = letterbox(img, (640, 640))
    processed_img_path = PROCESSED_DIR / f"{image_path.stem}.jpg"
    save_image(processed_img_path, processed_img)
    objects = predict_image(model, processed_img_path)
    return objects