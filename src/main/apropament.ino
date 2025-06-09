const uint8_t CAMERA_FOV_DEG = 120;  // amplitud total (+-60°)
const uint8_t CAMERA_STEP_DEG = 10;   // step degree
const uint8_t PERSON_CLOSE_PERC = 80;   // bounding‑box >= 80 % ample imatge
const uint16_t CAMERA_SETTLE_MS = 120;  // Temps minim entre captures
const uint16_t FRAME_WIDTH = 640; // Ample de la imatge de la càmera

int cameraCurrentAngle = 0;



// ----------------------
// PROPOSTA DE FUNCIONS
// ----------------------

// Canviar ???
void rotateCamera(int angle) {

    int delta = angle - cameraCurrentAngle;

    if (delta != 0) {
        stepper_rotateDegrees(delta);
        cameraCurrentAngle = angle;
    }
}

void rotateCameraToCenter() {
    rotateCamera(0);
}

bool detectPerson(int& angleOut, uint16_t& bbWidth) {
    return false; //FALTA IMPLEMENTACIO
}

bool isPersonCloseEnough(uint16_t bbWidth) {
    return ((bbWidth * 100UL) / FRAME_WIDTH) >= PERSON_CLOSE_PERC;
}

int angleStartFromFOV() {
    return -(CAMERA_FOV_DEG / 2);
}

bool hasElapsed(unsigned long lastTime) {
    return millis() - lastTime >= CAMERA_SETTLE_MS;
}

int nextScanAngle(int currentAngle) {
    return currentAngle + CAMERA_STEP_DEG;
}

bool hasCompletedScan(int angle) {
    return angle > (CAMERA_FOV_DEG / 2);
}

uint16_t getCameraFrameWidth() {
    return FRAME_WIDTH;
}