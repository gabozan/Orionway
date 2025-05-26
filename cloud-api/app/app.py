########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from flask import Flask, request, jsonify
from config import MODEL_OBJECT_PERSON_DETECTOR
from image_utils import load_image_from_bytes
from yolo_utils import load_model, predict_image
from postprocess import calculate_class_center, analyze_person

# ==================================================================================================================== #

app = Flask(__name__)

model1 = load_model(MODEL_OBJECT_PERSON_DETECTOR)

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

@app.route("/predict/person", methods=["POST"])
def detect_person():
    if "file" not in request.files:
        return jsonify({"error": "No s'ha enviat cap imatge"}), 400

    file_bytes = request.files["file"].read()
    image = load_image_from_bytes(file_bytes)
    results = predict_image(model1, image)
    detected, x_offset, is_close = analyze_person(results, image)
    return jsonify({
        "detected": detected,
        "x_offset": x_offset,
        "is_close": is_close
    })