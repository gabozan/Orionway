void initStepperCam() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void moveSingleStep(int direction) {
  static int currentStep = 0;

  currentStep = (currentStep + direction + 8) % 8;

  digitalWrite(IN1, stepSequence[currentStep][0]);
  digitalWrite(IN2, stepSequence[currentStep][1]);
  digitalWrite(IN3, stepSequence[currentStep][2]);
  digitalWrite(IN4, stepSequence[currentStep][3]);

  delayMicroseconds(STEP_DELAY);
  absolutePosition += direction;
}

void rotateCamera(double targetAngle) {
  int targetSteps = round(targetAngle * STEPS_PER_REV / TWO_PI);

  int stepsToMove = targetSteps - absolutePosition;
  int direction = (stepsToMove > 0) ? 1 : -1;
  stepsToMove = abs(stepsToMove);

  for (int i = 0; i < stepsToMove; i++) {
    moveSingleStep(direction);
  }

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void rotateCameraToCenter() {
  rotateCamera(0);
}