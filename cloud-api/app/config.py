########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from pathlib import Path

# ==================================================================================================================== #

BASE_PATH = Path(__file__).parent.resolve()

# Rutes dels models YOLO
MODEL_OBJECT_DETECTOR = BASE_PATH / "models" / "modelObjectDetector.pt"
MODEL_HUMAN_DETECTOR = BASE_PATH / "models" / "modelHumanDetector.pt"

# Paràmetres de la inferència
IMAGE_SIZE = 640
DEVICE = "cpu"
CONFIDENCE = 0.25