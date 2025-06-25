const uint8_t CAMERA_FOV_DEG = 120;  // amplitud total (+-60°)
const uint8_t CAMERA_STEP_DEG = 10;   // step degree
const uint16_t CAMERA_SETTLE_MS = 500;  // Temps minim entre captures


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

    float offsetToAngle(int offset, int resolution = 640, float fov = 62.0) {
        return (offset / (resolution / 2.0)) * (fov / 2.0);
    }
