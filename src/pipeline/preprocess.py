########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

import cv2
import numpy as np
from typing import Tuple

# ==================================================================================================================== #

def letterbox(img: np.ndarray,
              new_size: Tuple[int, int] = (512, 512),
              color: Tuple[int, int, int] = (114, 114, 114)
              ) -> Tuple[np.ndarray, float, Tuple[int, int]]:
    """
    Redimensiona una imatge mantenint la seva relació d'aspecte i aplica padding per ajustar-la a la mida desitjada.

    Paràmetres:
        img (np.ndarray): Imatge original en format BGR.
        new_size (tuple): Mida final desitjada (width, height).
        color (tuple): Color del padding en format BGR.

    Retorna:
        Tuple[np.ndarray, float, Tuple[int, int]]:
            - img_padded: Imatge amb padding ajustada a la nova mida.
            - scale: Escala aplicada respecte a la mida original.
            - padding: Padding aplicat (left, top).
    """
    img = img.astype(np.uint8) if img.dtype != np.uint8 else img
    orig_h, orig_w = img.shape[:2]
    new_w, new_h = new_size
    scale = min(new_w / orig_w, new_h / orig_h)
    resize_w, resize_h = int(orig_w * scale), int(orig_h * scale)

    img_resized = cv2.resize(img, (resize_w, resize_h), interpolation=cv2.INTER_LINEAR)

    pad_w = new_w - resize_w
    pad_h = new_h - resize_h
    top = pad_h // 2
    bottom = pad_h - top
    left = pad_w // 2
    right = pad_w - left

    img_padded = cv2.copyMakeBorder(img_resized, top, bottom, left, right,
                                    borderType=cv2.BORDER_CONSTANT, value=color)
    return img_padded, scale, (left, top)