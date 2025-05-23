void initRaspberryIO(){
    Serial.begin(115200);
}

int readFromRaspberry()
{
  if (Serial.available())
  {
    return Serial.read() - '0';
  }
  return -1;
}

void sendToRaspberry(int i)
{
  Serial.println(i);
}
