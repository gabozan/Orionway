const int buttonPins[3] = {2, 3, 4};
int estatActual[3] = {HIGH, HIGH, HIGH};
int estatAnterior[3] = {HIGH, HIGH, HIGH};
unsigned long lastPressTime[3] = {0, 0, 0};
int clickCount[3] = {0, 0, 0};
unsigned long lastReleaseTime[3] = {0, 0, 0};
unsigned long holdStartTime[3] = {0, 0, 0};
int clickState[3] = {0, 0, 0};

const long debounceDelay = 50;
const long doubleClickInterval = 300;
const long holdTimeThreshold = 3000;

typedef enum {
  BUTTON_NONE = -1,
  BUTTON0_SINGLE_CLICK = 0,
  BUTTON1_SINGLE_CLICK = 1,
  BUTTON2_SINGLE_CLICK = 2,
  BUTTON0_LONG_PRESS = 10,
  BUTTON1_LONG_PRESS = 11,
  BUTTON2_LONG_PRESS = 12,
  BUTTON1_DOUBLE_CLICK = 21
} ButtonEvent;

void initButtons() {
  for (int i = 0; i < 3; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

ButtonEvent getButtons(unsigned long currentTime) {
  for (int i = 0; i < 3; i++) {
    estatActual[i] = digitalRead(buttonPins[i]);

    if (estatActual[i] == LOW && estatAnterior[i] == HIGH && (currentTime - lastPressTime[i]) > debounceDelay) {
      lastPressTime[i] = currentTime;
      clickCount[i]++;
      clickState[i] = 1;
      holdStartTime[i] = currentTime;
    }

    if (estatActual[i] == LOW && estatAnterior[i] == LOW) {
      if (clickState[i] == 1 && (currentTime - holdStartTime[i]) >= holdTimeThreshold) {
        clickState[i] = 3;
        return (ButtonEvent)(i + 10);
      }
    }

    if (estatActual[i] == HIGH && estatAnterior[i] == LOW) {
      lastReleaseTime[i] = currentTime;
      if (clickState[i] != 3) {
        clickState[i] = 1;
      } else {
        clickState[i] = 0;
      }
    }

    if (clickState[i] != 3 && (currentTime - lastReleaseTime[i]) > doubleClickInterval && lastReleaseTime[i] > 0) {
      if (clickCount[i] == 1) {
        clickCount[i] = 0;
        clickState[i] = 0;
        lastReleaseTime[i] = 0;
        return (ButtonEvent)i;
      } else if (clickCount[i] == 2) {
        clickCount[i] = 0;
        clickState[i] = 0;
        lastReleaseTime[i] = 0;
        if (i == 1) return BUTTON1_DOUBLE_CLICK;
      } else {
        clickCount[i] = 0;
        clickState[i] = 0;
        lastReleaseTime[i] = 0;
      }
    }

    estatAnterior[i] = estatActual[i];
  }

  return BUTTON_NONE;
}
