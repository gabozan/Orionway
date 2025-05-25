########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from pathlib import Path

# ==================================================================================================================== #

# Rutes dels models YOLO
MODEL_HUMAN_DETECTOR = Path("../models/modelHumanDetector.pt")
MODEL_OBJECT_DETECTOR = Path("../models/modelObjectDetector.pt")

# Paràmetres de la inferència
IMAGE_SIZE = 640
DEVICE = "cpu"
CONFIDENCE = 0.25

# Directori de sortida del preprocessament de les imatges
PROCESSED_DIR = Path("../processed/")

# Directori de sortida de les prediccions
PREDICTIONS_DIR = Path("../predictions/")