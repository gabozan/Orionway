from config import IMAGE_SIZE
from yolo_inference import predict_frame

def robot_vision_pipeline(model, path_frame, debug=False, model_name="model1"):
    frame = preprocess_frame(path_frame, IMAGE_SIZE)              # Preprocesament del frame
    results = predict_frame(model, frame, debug, model_name)      # Inferència del frame