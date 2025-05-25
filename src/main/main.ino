typedef enum RobotState {
  ATURAT,
  RECONEIX,
  AVANÇA,
  GIRA,
  PETICIÓ,
  ZEBRA_ESPERA,
  ZEBRA_UBICA,
  ZEBRA_AVANÇA,
  APROPAMENT
};

RobotState estat = ATURAT;

void setup()
{
  initButtons();
  initRaspberryIO();
  initMotors();
}

void loop()
{
  unsigned long currentTime = millis();
  switch (estat){

    //=====================//
    //       ATURAT        //
    //=====================//
    case ATURAT:
      //--------------[ ACCIONS ]--------------
      stopMotors();

      //-----------[ CANVIS D'ESTAT ]----------

      // Dos cops de mans
      if(getTaps())
      {
        estat = APROPAMENT;
        break;
      }

      // Pulsació al botó central (una o dues vegades)
      ButtonEvent button = getButtons(currentTime);
      switch (button){
        case BUTTON1_SINGLE_CLICK:
          estat = AVANÇA;
          break;
        case BUTTON1_DOUBLE_CLICK:
          estat = RECONEIX;
          sendToRaspberry(RECONEIX);
          break;
      }

      break;

    //=====================//
    //      RECONEIX       //
    //=====================//
    case RECONEIX:

      //-----------[ CANVIS D'ESTAT ]----------

      // La Raspberry retorna el mateix codi = Ha acabat correctament
      if(readFromRaspberry() == RECONEIX)
      {
        estat = ATURAT;
      }
      break;
  }
}
