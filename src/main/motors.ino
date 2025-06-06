
void leftTick()
{
  leftEncoder.tick();
}

void rightTick()
{
  rightEncoder.tick();
}

void initMotors()
{
  Serial.begin(115200);
  delay(200);
  pinMode(LPWM1, OUTPUT);
  pinMode(LPWM2, OUTPUT);
  pinMode(RPWM1, OUTPUT);
  pinMode(RPWM2, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(LEFT_CHA), leftTick, CHANGE);
  attachInterrupt(digitalPinToInterrupt(LEFT_CHB), leftTick, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT_CHA), rightTick, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT_CHB), rightTick, CHANGE);
  stopMotors();
}

void setLeft(bool dir)
{
  analogWrite(LPWM1, dir ? PWM : 0);
  analogWrite(LPWM2, dir ? 0 : PWM);
}

void setRight(bool dir)
{
  analogWrite(RPWM1, dir ? PWM : 0);
  analogWrite(RPWM2, dir ? 0 : PWM);
}

void moveForward()
{
  setLeft(true);
  setRight(true);
}

void stopMotors()
{
  analogWrite(LPWM1, 0);
  analogWrite(LPWM2, 0);
  analogWrite(RPWM1, 0);
  analogWrite(RPWM2, 0);
}

void resetEncoders()
{
  leftEncoder.setPosition(0);
  rightEncoder.setPosition(0);
}

long getLeftTicks()
{
  return abs(leftEncoder.getPosition());
}

long getRightTicks()
{
  return abs(rightEncoder.getPosition());
}

void rotate(float angleRad)
{
  resetEncoders();

  double arcLength = fabs(angleRad) * d * 2;
  double wheelCircumference = 2.0 * PI * r;
  double wheelRotations = arcLength / wheelCircumference;
  long targetTicks = (long)(wheelRotations * TICKS_PER_REV);

  bool rotateLeft = angleRad > 0;
  setLeft(rotateLeft);
  setRight(!rotateLeft);

  while (getLeftTicks() < targetTicks && getRightTicks() < targetTicks)
  {
    long left = getLeftTicks();
    long right = getRightTicks();

    // Serial.print("Target: ");
    // Serial.print(targetTicks);
    // Serial.print(" | L: ");
    // Serial.print(left);
    // Serial.print(" | R: ");
    // Serial.println(right);

    delay(50);
  }

  stopMotors();

  // Serial.print("Final L: ");
  // Serial.print(getLeftTicks());
  // Serial.print(" | Final R: ");
  // Serial.println(getRightTicks());
}