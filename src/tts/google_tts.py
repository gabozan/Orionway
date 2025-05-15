########################################################################################################################
#                                                                                                                      #
#                                    OrionWay - Robótica, Llenguatge i Planificació                                    #
#                                                                                                                      #
########################################################################################################################

from google.cloud import texttospeech
from pathlib import Path
from typing import Union
import pygame
import os

# ==================================================================================================================== #

os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = "key.json"

def generate_audio(text: str, file_path: Union[str, Path] = "../audios/detectarObjecte.mp3"):
    """
    Genera un fitxer d'àudio en format MP3 a partir d’un text utilitzant el servei Text-to-Speech de Google Cloud. El
    fitxer generat es desa localment amb el nom indicat.

    Paràmetres:
    - text (str): Text que es vol convertir en àudio.
    - file_path (str o Path): Ruta i nom del fitxer de sortida.
    """
    output_path = Path(file_path)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    client = texttospeech.TextToSpeechClient()
    synthesis_input = texttospeech.SynthesisInput(text=text)
    voice = texttospeech.VoiceSelectionParams(
        language_code="ca-ES",
        ssml_gender=texttospeech.SsmlVoiceGender.NEUTRAL
    )
    audio_config = texttospeech.AudioConfig(
        audio_encoding=texttospeech.AudioEncoding.MP3,
        speaking_rate=0.9
    )
    response = client.synthesize_speech(
        input=synthesis_input,
        voice=voice,
        audio_config=audio_config
    )
    with open(output_path, "wb") as out:
        out.write(response.audio_content)


def play_audio(file_path: Union[str, Path] = "../audios/detectarObjecte.mp3"):
    """
    Reprodueix un fitxer d'àudio en format MP3 utilitzant la llibreria pygame.

    Paràmetres:
    - file_path (str o Path): Ruta al fitxer d'àudio que es vol reproduir.
    """
    output_path = Path(file_path)
    if not output_path.exists():
        print(f"El fitxer {file_path} no existeix.")
        return

    pygame.mixer.init()
    pygame.mixer.music.load(str(output_path))
    pygame.mixer.music.play()
    while pygame.mixer.music.get_busy():
        continue