// ---------------------------------------- buttons_control.ino
const int buttonPins[3] = { 51, 52, 53 };
int estatActual[3] = { HIGH, HIGH, HIGH };
int estatAnterior[3] = { HIGH, HIGH, HIGH };
unsigned long lastPressTime[3] = { 0, 0, 0 };
int clickCount[3] = { 0, 0, 0 };
unsigned long lastReleaseTime[3] = { 0, 0, 0 };
unsigned long holdStartTime[3] = { 0, 0, 0 };
int clickState[3] = { 0, 0, 0 };

const long debounceDelay = 50;
const long doubleClickInterval = 300;
const long holdTimeThreshold = 3000;

typedef enum
{
  BUTTON_NONE = -1,
  BUTTON0_SINGLE_CLICK = 0,
  BUTTON1_SINGLE_CLICK = 1,
  BUTTON2_SINGLE_CLICK = 2,
  BUTTON0_LONG_PRESS = 10,
  BUTTON1_LONG_PRESS = 11,
  BUTTON2_LONG_PRESS = 12,
  BUTTON1_DOUBLE_CLICK = 21
} ButtonEvent;

// ---------------------------------------- detect_distances.ino
const int trigPins[3] = { 45, 13, 47 };
const int echoPins[3] = { 44, 12, 46 };
const int thresholds[3] = { 30, 30, 30 };

float distancies[3];

// Màquina d’estats per sensor
enum EstatSensor { IDLE, TRIGGER, ESPERANT_HIGH, ESPERANT_LOW, CALCULAT };

const unsigned long timeoutEcho = 30000;
bool objectesDetectats[3];


// ---------------------------------------- motors.ino
#include <RotaryEncoder.h>

#define LPWM1 10
#define LPWM2 11
#define RPWM1 5
#define RPWM2 6

#define LEFT_CHA 19
#define LEFT_CHB 20
#define RIGHT_CHA 2
#define RIGHT_CHB 3

const long TICKS_PER_REV = 45000;//50000;
const float r = 7.25; // cm
const float d = 15.0; // cm
const uint8_t linealPWM = 50;
const uint8_t rotatoryPWM = 80;

RotaryEncoder leftEncoder(LEFT_CHA, LEFT_CHB, RotaryEncoder::LatchMode::TWO03);
RotaryEncoder rightEncoder(RIGHT_CHA, RIGHT_CHB, RotaryEncoder::LatchMode::TWO03);

// ---------------------------------------- detect_clap.ino
const int micPin = A0;
const int soundThreshold = 1500;
const unsigned long minTimeGap = 200;
const unsigned long maxTimeGap = 500;
unsigned long previousTapTime = 0;
unsigned long timeSinceLastTap = 0;

typedef enum {
  ATURAT,
  RECONEIX,
  AVANCA,
  GIRA,
  PETICIO,
  ZEBRA_ESPERA,
  ZEBRA_UBICA,
  ZEBRA_AVANCA,
  APROPAMENT_SEARCH_PERSON,
  APROPAMENT_ORIENT_ROBOT,
  APROPAMENT_FORWARD_TO_OBSTACLE,
  APROPAMENT_CHECK_ARRIVAL,
  APROPAMENT_AVOID_OBSTACLE,
} RobotState;

RobotState estat = ATURAT;
ButtonEvent button = BUTTON_NONE;
RobotState giraSeguent = ATURAT;
float angle = 0;
int instruccio;

// APROPAMENT: variables
int  scanAngle = angleStartFromFOV();           // angle actual de barrida
unsigned long lastScanTs = 0;                   // temporitzador de camera
int  foundAngle = 0;                            // angle relatiu a la persona
float offset = 0;
uint16_t bbWidth = 0;                           // bounding‑box
int  avoidTurnDeg = 0;                          // +90 / –90  (esquerra / dreta)
bool instructionSent = false;                   // indica si s'ha enviat la instrucció a la Raspberry

const char* robotStateToString(RobotState state) {
  switch (state) {
  case ATURAT:        return "ATURAT    ";
  case RECONEIX:      return "RECONEIX  ";
  case AVANCA:        return "AVANCA    ";
  case GIRA:          return "GIRA      ";
  case PETICIO:       return "PETICIO   ";
  case ZEBRA_ESPERA:  return "ZEB_ESPERA";
  case ZEBRA_UBICA:   return "ZEB_UBICA ";
  case ZEBRA_AVANCA:  return "ZEB_AVANCA";
  case APROPAMENT:    return "APROPAMENT";
  default:            return "DESCONEGUT";
  }
}

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
  button = getButtons();
  RevisaObstacles();
  // Serial.println(readInput());
  Serial.print("\rEstat: ");
  Serial.print(robotStateToString(estat));
  Serial.print(" | Button: ");
  Serial.print(button);
  Serial.print(" | Left: ");
  Serial.print(objectesDetectats[0]);
  Serial.print(" | Front: ");
  Serial.print(objectesDetectats[1]);
  Serial.print(" | Right: ");
  Serial.print(objectesDetectats[2]);
  Serial.print(" | Distances: ");
  Serial.print(distancies[0]);
  Serial.print(", ");
  Serial.print(distancies[1]);
  Serial.print(", ");
  Serial.println(distancies[2]);
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
      estat = APROPAMENT_SEARCH_PERSON;
      break;
    }

    // Pulsació al botó central (una o dues vegades)
    switch (button) {
    case BUTTON1_SINGLE_CLICK:
      estat = AVANCA;
      moveForward();
      break;
    case BUTTON1_DOUBLE_CLICK:
      estat = RECONEIX;
      sendInstructionToRaspberry(RECONEIX);
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
    //       AVANCA        //
    //=====================//
  case AVANCA:
    //--------------[ ACCIONS ]--------------
    // moveForward();

    //-----------[ CANVIS D'ESTAT ]----------
    // Detectar parets davant
    // RevisaObstacles(objectesDetectats);
    if (objectesDetectats[1])
    {
      angle = objectesDetectats[2] ? -PI / 2 : PI / 2;  // En cas que la dreta no estigui lliure, gira a l'esquerra (no contemplat cas on hi ha d'haver gir 180º)
      estat = GIRA;
      giraSeguent = AVANCA;
      break;
    }

    // Pulsació al botó central (una o dues vegades)
    switch (button) {
    case BUTTON0_LONG_PRESS:
    case BUTTON2_LONG_PRESS:
      estat = PETICIO;
      break;
    case BUTTON1_SINGLE_CLICK:
      estat = ATURAT;
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
    //      PETICIO       //
    //=====================//
  case PETICIO:
    //-----------[ CANVIS D'ESTAT ]----------
    // RevisaObstacles(objectesDetectats);  // Es pot treure si veiem que a AVANCA ja es fa i no fa falta repetir-ho

    // Si no es detecten obstacles en la direcció de la PETICIO
    if ((!objectesDetectats[0] && button == BUTTON0_LONG_PRESS) || (!objectesDetectats[2] && button == BUTTON2_LONG_PRESS))
    {
      angle = button == BUTTON0_LONG_PRESS ? -PI / 2 : PI / 2;
      estat = GIRA;
      giraSeguent = AVANCA;
      break;
    }
    else
    {
      estat = AVANCA;
      moveForward();
      break;
    }
    break;


    //=====================//
    //        GIRA         //
    //=====================//
  case GIRA:
    rotate(angle);
    estat = giraSeguent;
    if (estat == AVANCA) moveForward();
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
    if (instruccio == ZEBRA_AVANCA)
    {
      estat = ZEBRA_AVANCA;
      moveForward();
      break;
    }
    break;

    //========================//
    //      ZEBRA_AVANCA      //
    //========================//
  case ZEBRA_AVANCA:
    //--------------[ ACCIONS ]--------------
    // moveForward();

    //-----------[ CANVIS D'ESTAT ]----------
    instruccio = readInstructionFromRaspberry();
    if (instruccio == AVANCA)
    {
      estat = AVANCA;
      moveForward();
      break;
    }
    break;

    //========================//
    //      APROPAMENT        //
    //========================//
  case APROPAMENT_SEARCH_PERSON:

    if (!instructionSent) {
      // Enviem la instrucció a la Raspberry per iniciar la cerca de la persona
      sendInstructionToRaspberry(APROPAMENT_SEARCH_PERSON);
      instructionSent = true;
    }
    if (hasElapsed(lastScanTs)) {
      rotateCamera(scanAngle);
      scanAngle = nextScanAngle(scanAngle);
      if (hasCompletedScan(scanAngle)) {
        //No es troba ningú --> torna a ATURAT
        estat = ATURAT;
        instructionSent = false;
        break;
      }
      lastScanTs = millis();
    }
    estat = APROPAMENT_ORIENT_ROBOT;
    break;

  case APROPAMENT_ORIENT_ROBOT:
    instruccio = readInstructionFromRaspberry();
    //tornem a intentar detectar la persona
    if (instruccio == APROPAMENT_SEARCH_PERSON)
    {
      estat = APROPAMENT_SEARCH_PERSON;
      instructionSent = false;
      break;
    }
    offset = readFloatFromRaspberry();
    if (offset == NAN) {
      estat = ATURAT;
      instructionSent = false;
      break;
    }
    foundAngle = offsetToAngle(offset);
    rotateCameraToCenter();
    rotate(foundAngle);
    moveForward();
    estat = APROPAMENT_FORWARD_TO_OBSTACLE;
    instructionSent = false;
    break;

  case APROPAMENT_FORWARD_TO_OBSTACLE:
    RevisaObstacles(objectesDetectats);
    if (objectesDetectats[1]) {
      stopMotors();
      estat = APROPAMENT_CHECK_ARRIVAL;
      instructionSent = false;
    }
    break;

  case APROPAMENT_CHECK_ARRIVAL:
  {
    if (!instructionSent) {
      // Enviem la instrucció a la Raspberry per comprovar si hi ha una persona a prop
      sendInstructionToRaspberry(APROPAMENT_CHECK_ARRIVAL);
      instructionSent = true;
    }

    instruccio = readInstructionFromRaspberry();
    if (instruccio == APROPAMENT_CHECK_ARRIVAL) {
      // Persona trobada, i a prop. Fi de l'apropament
      stopMotors();
      estat = ATURAT;
      instructionSent = false;
      rotateCameraToCenter();
      scanAngle = angleStartFromFOV();
      rotate(180); // Gira 180º per tornar a la posició inicial
    }
    else if (instruccio == APROPAMENT_AVOID_OBSTACLE) {
      // No es una persona
      avoidTurnDeg = (objectesDetectats[0] || !objectesDetectats[2]) ? 90 : -90; // preferencia esquerra
      rotate(avoidTurnDeg);
      moveForward();
      estat = APROPAMENT_AVOID_OBSTACLE;
      instructionSent = false;
    }
    break;
  }

  case APROPAMENT_AVOID_OBSTACLE:
  {
    bool sideClear = (avoidTurnDeg > 0) ? (objectesDetectats[2])
      : (objectesDetectats[0]);
    if (sideClear) {
      stopMotors();
      // Tornem a buscar la persona
      scanAngle = angleStartFromFOV();
      estat = APROPAMENT_SEARCH_PERSON;
    }
    else if (objectesDetectats[1]) {
      // Si hi ha obstacle frontal, intentem girar cap a l'altre costat
      stopMotors();
      avoidTurnDeg = (avoidTurnDeg > 0) ? -90 : 90;
      rotate(avoidTurnDeg);
      moveForward();
      estat = APROPAMENT_FORWARD_TO_OBSTACLE;
    }
    break;
  }
  break;
  }
}
