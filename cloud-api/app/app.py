########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from flask import Flask, request, jsonify
from config import MODEL_OBJECT_DETECTOR, MODEL_HUMAN_DETECTOR
from image_utils import load_image_from_bytes
from yolo_utils import load_model, predict_image
from postprocess import calculate_class_center

# ==================================================================================================================== #

app = Flask(__name__)

model1 = load_model(MODEL_OBJECT_DETECTOR)
#model2 = load_model(MODEL_HUMAN_DETECTOR)

# ==================================================================================================================== #

@app.route("/predict/objects", methods=["POST"])
def detect_objects():
    if "file" not in request.files:
        return jsonify({"error": "No s'ha enviat cap imatge"}), 400

    file_bytes = request.files["file"].read()
    image = load_image_from_bytes(file_bytes)
    results = predict_image(model1, image)
    detections = calculate_class_center(results, image)
    return jsonify({
        "detections": detections
    })