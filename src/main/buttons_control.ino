void initButtons()
{
  for (int i = 0; i < 3; i++)
  {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

ButtonEvent getButtons()
{
  for (int i = 0; i < 3; i++)
  {
    estatActual[i] = digitalRead(buttonPins[i]);

    if (estatActual[i] == LOW && estatAnterior[i] == HIGH && (millis() - lastPressTime[i]) > debounceDelay)
    {
      lastPressTime[i] = millis();
      clickCount[i]++;
      clickState[i] = 1;
      holdStartTime[i] = millis();
    }

    if (estatActual[i] == LOW && estatAnterior[i] == LOW)
    {
      if (clickState[i] == 1 && (millis() - holdStartTime[i]) >= holdTimeThreshold)
      {
        clickState[i] = 3;
        return (ButtonEvent)(i + 10);
      }
    }

    if (estatActual[i] == HIGH && estatAnterior[i] == LOW)
    {
      lastReleaseTime[i] = millis();
      if (clickState[i] != 3)
      {
        clickState[i] = 1;
      }
      else
      {
        clickState[i] = 0;
      }
    }

    if (clickState[i] != 3 && (millis() - lastReleaseTime[i]) > doubleClickInterval && lastReleaseTime[i] > 0)
    {
      if (clickCount[i] == 1)
      {
        clickCount[i] = 0;
        clickState[i] = 0;
        lastReleaseTime[i] = 0;
        return (ButtonEvent)i;
      }
      else if (clickCount[i] == 2)
      {
        clickCount[i] = 0;
        clickState[i] = 0;
        lastReleaseTime[i] = 0;
        if (i == 1)
          return BUTTON1_DOUBLE_CLICK;
      }
      else
      {
        clickCount[i] = 0;
        clickState[i] = 0;
        lastReleaseTime[i] = 0;
      }
    }

    estatAnterior[i] = estatActual[i];
  }

  return BUTTON_NONE;
}
