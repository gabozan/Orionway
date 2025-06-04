########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

import cv2
import numpy as np
import torch
from torchvision import transforms
from typing import Tuple

# ==================================================================================================================== #

def load_image_from_bytes(file_bytes: bytes) -> np.ndarray:
    """
    Carrega una imatge a partir de bytes en format BGR.

    Paràmetres:
        file_bytes (bytes): Contingut binari de la imatge.

    Retorna:
        np.ndarray: Imatge carregada en format BGR .
    """
    np_arr = np.frombuffer(file_bytes, np.uint8)
    image = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
    return image

def load_image_from_bytes_v5(file_bytes: bytes) -> torch.Tensor:
    """
    Carrega una imatge a partir de bytes i la converteix en un tensor.

    Paràmetres:
        file_bytes (bytes): Contingut binari de la imatge.

    Retorna:
        torch.Tensor: Imatge com tensor [1, 3, H, W] en RGB, normalitzada entre 0-1.
    """
    np_arr = np.frombuffer(file_bytes, np.uint8)
    image = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
    image_resized = letterbox(image, new_size=(640, 640))
    image_rgb = cv2.cvtColor(image_resized, cv2.COLOR_BGR2RGB)
    image_rgb = image_rgb.astype(np.float32) / 255.0
    image_rgb = np.transpose(image_rgb, (2, 0, 1))
    return torch.from_numpy(image_rgb).unsqueeze(0)


def letterbox(img: np.ndarray,
              new_size: Tuple[int, int] = (640, 640),
              color: Tuple[int, int, int] = (114, 114, 114)
              ) -> np.ndarray:
    """
    Redimensiona una imatge mantenint la seva relació d'aspecte i aplica padding per ajustar-la a la mida desitjada.

    Paràmetres:
        img (np.ndarray): Imatge original en format BGR.
        new_size (tuple): Mida final desitjada (width, height).
        color (tuple): Color del padding en format BGR.

    Retorna:
        img_padded: Imatge amb padding ajustada a la nova mida.
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
    return img_padded