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
float angle = 0;

// APROPAMENT: variables i subestats
enum ApropSubState : uint8_t {
  SEARCH_PERSON,
  ORIENT_ROBOT,
  FORWARD_TO_OBSTACLE,
  CHECK_ARRIVAL,
  AVOID_OBSTACLE
};
ApropSubState subState = SEARCH_PERSON;         // subestat
int  scanAngle = angleStartFromFOV();           // angle actual de barrida
unsigned long lastScanTs = 0;                   // temporitzador de camera
int  foundAngle = 0;                            // angle relatiu a la persona
uint16_t bbWidth = 0;                           // bounding‑box
int  avoidTurnDeg = 0;                          // +90 / –90  (esquerra / dreta)


int instruccio;

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
  switch (estat) {

    //=====================//
    //       ATURAT        //
    //=====================//
  case ATURAT:
    //--------------[ ACCIONS ]--------------
    stopMotors();

    //-----------[ CANVIS D'ESTAT ]----------

    // Dos cops de mans
    if (getTaps())
    {
      estat = APROPAMENT;
      break;
    }

    // Pulsació al botó central (una o dues vegades)
    button = getButtons(currentTime);
    switch (button) {
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

      // La Raspberry retorna el següent codi = Ha acabat correctament
    if (readInstructionFromRaspberry() == ATURAT)
    {
      estat = ATURAT;
      break;
    }
    break;

    //=====================//
    //       AVANÇA        //
    //=====================//
  case AVANÇA:
    //--------------[ ACCIONS ]--------------
    moveForward();

    //-----------[ CANVIS D'ESTAT ]----------
    // Detectar parets davant
    RevisaObstacles(objectesDetectats);
    if (objectesDetectats[1])
    {
      angle = objectesDetectats[2] ? -90.0 : 90.0;  // En cas que la dreta no estigui lliure, gira a l'esquerra (no contemplat cas on hi ha d'aver gir 180º)
      estat = GIRA;
      giraSegüent = AVANÇA;
      break;
    }

    // Pulsació al botó central (una o dues vegades)
    button = getButtons(currentTime);
    switch (button) {
    case BUTTON0_LONG_PRESS:
    case BUTTON2_LONG_PRESS:
      estat = PETICIÓ;
      break;
    case BUTTON1_DOUBLE_CLICK:
      estat = RECONEIX;
      sendInstructionToRaspberry(RECONEIX);
      break;
    }

    // Reconeixement de pas de zebra
    instruccio = readInstructionFromRaspberry();
    if (instruccio == ZEBRA_UBICA)
    {
      estat = ZEBRA_UBICA;
      break;
    }

    break;

    //=====================//
    //      PETICIÓ       //
    //=====================//
  case PETICIÓ:
    //-----------[ CANVIS D'ESTAT ]----------
    RevisaObstacles(objectesDetectats);  // Es pot treure si veiem que a AVANÇA ja es fa i no fa falta repetir-ho

    // Si no es detecten obstacles en la direcció de la petició
    if ((!objectesDetectats[0] && button == BUTTON0_LONG_PRESS) || (!objectesDetectats[2] && button == BUTTON2_LONG_PRESS))
    {
      angle = button == BUTTON0_LONG_PRESS ? -90.0 : 90.0;
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

    //========================//
    //      ZEBRA_UBICA       //
    //========================//
  case ZEBRA_UBICA:

    //-----------[ CANVIS D'ESTAT ]----------
    instruccio = readInstructionFromRaspberry();
    if (instruccio == ZEBRA_ESPERA)
    {
      estat = ZEBRA_ESPERA;
      break;
    }
    break;

    //========================//
    //      ZEBRA_ESPERA      //
    //========================//
  case ZEBRA_ESPERA:

    //-----------[ CANVIS D'ESTAT ]----------
    instruccio = readInstructionFromRaspberry();
    if (instruccio == ZEBRA_AVANÇA)
    {
      estat = ZEBRA_AVANÇA;
      break;
    }
    break;

    //========================//
    //      ZEBRA_AVANÇA      //
    //========================//
  case ZEBRA_AVANÇA:
    //--------------[ ACCIONS ]--------------
    moveForward();

    //-----------[ CANVIS D'ESTAT ]----------
    instruccio = readInstructionFromRaspberry();
    if (instruccio == AVANÇA)
    {
      estat = AVANÇA;
      break;
    }
    break;

    //=====================//
    //     APROPAMENT      //
    //=====================//
  case APROPAMENT:
  {
    switch (subState) {

    case SEARCH_PERSON:
      if (hasElapsed(lastScanTs)) {
        rotateCamera(scanAngle);
        if (detectPerson(foundAngle, bbWidth)) {
          rotateCameraToCenter();
          subState = ORIENT_ROBOT;
        }
        else {
          scanAngle = nextScanAngle(scanAngle);
          if (hasCompletedScan(scanAngle)) {
            //No es troba ningú --> torna a ATURAT
            estat = ATURAT;
          }
        }
        lastScanTs = millis();
      }
      break;

    case ORIENT_ROBOT:
      rotate(foundAngle);
      rotateCameraToCenter();
      moveForward();
      subState = FORWARD_TO_OBSTACLE;
      break;

    case FORWARD_TO_OBSTACLE:
      RevisaObstacles(objectesDetectats);
      if (objectesDetectats[1]) {
        stopMotors();
        subState = CHECK_ARRIVAL;
      }
      break;

    case CHECK_ARRIVAL:
    {
      if (detectPerson(foundAngle, bbWidth)) {
        const uint16_t frameW = getCameraFrameWidth();
        if (isPersonCloseEnough(bbWidth)) {
          // Persona trobada
          estat = ATURAT;
          subState = SEARCH_PERSON;
          scanAngle = angleStartFromFOV();
          rotate(180);
          break;
        }
      }
      // No es una persona
      avoidTurnDeg = (objectesDetectats[0] || !objectesDetectats[2]) ? 90 : -90; // preferencia esquerra
      rotate(avoidTurnDeg);
      moveForward();
      subState = AVOID_OBSTACLE;
      break;
    }

    case AVOID_OBSTACLE:
    {
      bool sideClear = (avoidTurnDeg > 0) ? (objectesDetectats[2])
        : (objectesDetectats[0]);
      if (sideClear) {
        stopMotors();
        // Tornem a buscar la persona
        scanAngle = angleStartFromFOV();
        subState = SEARCH_PERSON;
      }
      else if (objectesDetectats[1]) {
        stopMotors();
        avoidTurnDeg = (avoidTurnDeg > 0) ? -90 : 90;
        rotate(avoidTurnDeg);
        moveForward();
      }
      break;
    }
    }
  }

  break;
  }
}
