########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from pathlib import Path

# ==================================================================================================================== #

BASE_PATH = Path(__file__).parent.resolve()

# Rutes dels models YOLO
MODEL_OBJECT_PERSON_DETECTOR = BASE_PATH / "models" / "modelObjectDetector.pt"
MODEL_ZEBRAI_DETECTOR = BASE_PATH / "models" / "modelZebrAIDetector.pt"

# Paràmetres de la inferència
IMAGE_SIZE = 640
DEVICE = "cpu"
CONFIDENCE = 0.25