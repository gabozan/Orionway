void initRaspberryIO()
{
  Serial.begin(115200);
}

void sendInstructionToRaspberry(uint8_t code)
{
  Serial.write((uint8_t)0x01); // HEADER: Instrucció
  Serial.write(code);          // 1 byte
}

void sendDataToRaspberry(float val)
{
  Serial.write((uint8_t)0x02);                // HEADER: Dada float
  Serial.write((uint8_t *)&val, sizeof(val)); // 4 bytes
}

int readInstructionFromRaspberry()
{
  if (Serial.available() >= 2)
  {
    uint8_t header = Serial.read();
    if (header == 0x01 && Serial.available() >= 1)
    {
      return Serial.read();
    }
  }
  return -1;
}

float readFloatFromRaspberry()
{
  if (Serial.available() >= 5)
  {
    uint8_t header = Serial.read();
    if (header == 0x02 && Serial.available() >= 4)
    {
      float val;
      Serial.readBytes((byte *)&val, sizeof(val));
      return val;
    }
  }
  return NAN;
}