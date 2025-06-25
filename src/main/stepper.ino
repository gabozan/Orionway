const int IN1 = 8;   // módulo IN1
const int IN2 = 9;   // módulo IN2
const int IN3 = 10;  // módulo IN3
const int IN4 = 11;  // módulo IN4

const int STEPS_PER_REV = 2048;  // pasos totales
const int STEP_DELAY = 3000;     // velocidad

const int stepSequence[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

int absolutePosition = 0;

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

void rotateCamera(float targetAngle) {
  int targetSteps = round(targetAngle * STEPS_PER_REV / 360.0);

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