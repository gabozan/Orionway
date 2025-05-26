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
bool objectesDetectats[3];
ButtonEvent button = BUTTON_NONE;
RobotState giraSegüent = ATURAT;
float angle=0;

void setup()
{
  initDistancies();
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
      button = getButtons(currentTime);
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
        break;
      }
      break;

    //=====================//
    //      PETICIÓ       //
    //=====================//
    case PETICIÓ:
      RevisaObstacles(objectesDetectats);  // Es pot treure si veiem que a AVANÇA ja es fa i no fa falta repetir-ho

      //-----------[ CANVIS D'ESTAT ]----------

      // Si no es detecten obstacles en la direcció de la petició
      if((!objectesDetectats[0] && button == BUTTON0_LONG_PRESS) || (!objectesDetectats[2] && button == BUTTON2_LONG_PRESS))
      {
        angle = button == BUTTON0_LONG_PRESS? -90.0 : 90.0;
        estat = GIRA;
        giraSegüent = AVANÇA;
        break;
      }
      else
      {
        estat = AVANÇA;
        break;
      }
      break;

    
    //=====================//
    //        GIRA         //
    //=====================//
    case GIRA:
      rotate(angle);
      estat = giraSegüent;
      break;
  }
}
