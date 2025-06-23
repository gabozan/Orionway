########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from picamera2 import Picamera2
from datetime import datetime

# ==================================================================================================================== #


def capture_image(path: str | None = None, warmup: float = 0.0) -> str:
    """
    Fa una fotografia amb la càmera Raspberry Pi y la guarda a 'path'.
    Si 'path' és None, es genera un nom de fitxer basat en la data i hora actual.
    El warmup és el temps en segons per a que la càmera s'escalfi abans de capturar la imatge.
    
    Retorna
        path: ruta on s'ha guardat el fitxer.
    """
    if path is None:
        path = datetime.now().strftime("%Y%m%d_%H%M%S.jpg")

    picam2 = Picamera2()
    picam2.configure(picam2.create_still_configuration())
    picam2.start_and_capture_file(path, delay=warmup)
    picam2.close()
    return path