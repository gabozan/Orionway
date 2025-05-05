from ultralytics import YOLO
from config import DEVICE, IMAGE_SIZE, CONFIDENCE, OUTPUT_PATH

########################################################################################################################

def load_model(model_path):
    return YOLO(model_path)


def predict_frame(model, frame, debug=False, model_name="model1"):
    results = model.predict(
        source=frame,
        device=DEVICE,
        imgsz=IMAGE_SIZE,
        conf=CONFIDENCE,
        save=debug,
        save_txt=debug,
        save_conf=debug,
        project=(OUTPUT_PATH + model_name) if debug and model_name else None,
        name="." if debug else None,
        exist_ok=True
    )
    return results[0]