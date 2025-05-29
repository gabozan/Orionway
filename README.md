<h2 align="center">🤖 Orionway: El robot guia intel·ligent 🤖</h2>

<p align="center">
  <img src="assets/orionway.JPG" alt="Logo Orionway" width="1200"/>
</p>

---

### 📚 Taula de continguts

- [📖 Descripció del projecte](#descripció-del-projecte)
- [🎥 Video demostració]()
- [🧠 Característiques principals](#carasterístiques-principals)
- [⚙️ Arquitectura i hardware](arquitectura-i-hardware)
- [🧪 Tests]()
- [🙌 Autors]()
- [📄 Llicència]()

---

### 📖 Descripció del projecte

**Orionway** és un robot guia dissenyat per acompanyar persones amb discapacitat visual en l'entorn del campus de la UAB. El sistema combina sensors i visió per computador per proporcionar assistència segura i intuïtiva.
L'objectiu principal és tenir una alternativa a un gos guia i al bastó blanc per invidents de manera que **Orionway** sigui un punt intermig entre el que pot aportar un gos guia i un bastó.  
---

### 🧠 Característiques principals

Les funcions principals d'**Orionway** són:
 * Guía la direcció de l'usuari, buscant canviar la direcció quan l'usuari ho solicita.
 * Detecta obstacles propers canviant la direcció en cas de no poder avançar.
 * Detecta passos de zebra i extrema la precaució.
 * Detecta semàfors i actua en consecuència segons l'estat d'aquest.
 * Detecta objectes que sosté l'usuari.
 * Apropament automàtic a l'usuari en entorns tancats al escoltar dos cops de mà.
   
### ⚙️ Arquitectura i hardware

L'arquitectura de software del nostre projecte está formada per:
 * Arduino
 * Python (Control Raspberry Pi)
 * YOLO (Detecció d'objectes)
 * ZebrAI (Projecte Visió per Computador que detectar semàfors i passos de zebra) [Enllaç](https://github.com/albert-ce/ZebrAI-Crossing)

Respecte a hardware i components hem utilitzat el següent:
 * Arduino Mega 2560 [Datasheet](https://docs.arduino.cc/resources/datasheets/A000067-datasheet.pdf)
 * Raspberry Pi 4 4GB [Datasheet](https://www.farnell.com/datasheets/4170044.pdf)
 * Motor pas a pas 28BYJ-48 [Datasheet](https://www.mouser.com/datasheet/2/758/stepd-01-data-sheet-1143075.pdf)
 * Driver motor pas a pas ULN2003 [Datasheet](https://www.ti.com/lit/ds/symlink/uln2003a.pdf)
 * Motor 12V 455A105 [Datasheet](https://octopart.com/es/datasheet/455a105-2-globe+motors-19929790)
 * E2 optical encoder[Datasheet](https://www.usdigital.com/datasheets/e2-datasheet.pdf)
 * Sensor d'ultrasons HC-SR04 [Datasheet](https://leantec.es/wp-content/uploads/2019/06/Leantec.ES-HC-SR04.pdf)
 * 
