########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from flask import Flask, request, jsonify
from config import MODEL_OBJECT_PERSON_DETECTOR, MODEL_ZEBRAI_DETECTOR
from image_utils import load_image_from_bytes
from yolo_utils import load_model, load_modelv5, predict_image
from postprocess import calculate_class_center, analyze_person, analyze_crosswalk_and_traffic_lights

# ==================================================================================================================== #

app = Flask(__name__)

model1 = load_model(MODEL_OBJECT_PERSON_DETECTOR)
model2 = load_modelv5(MODEL_ZEBRAI_DETECTOR)

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

@app.route("/predict/zebrai", methods=["POST"])
def detect_zebrai():
    if "file" not in request.files:
        return jsonify({"error": "No s'ha enviat cap imatge"}), 400

    file_bytes = request.files["file"].read()
    image = load_image_from_bytes(file_bytes)
    results = model2(image)
    class_detected = analyze_crosswalk_and_traffic_lights(results)
    return jsonify({
        "crosswalk": class_detected[0],
        "traffic_light_red": class_detected[1],
        "traffic_light_green": class_detected[2]
    })