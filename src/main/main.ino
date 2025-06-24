// ---------------------------------------- buttons_control.ino
const int buttonPins[3] = {51, 52, 53};
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
const int trigPins[3] = {45, 13, 47};
const int echoPins[3] = {44, 12, 46};
const int thresholds[3] = {30, 30, 30};

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
  APROPAMENT
} RobotState;

RobotState estat = ATURAT;
ButtonEvent button = BUTTON_NONE;
RobotState giraSeguent = ATURAT;
float angle=0;
int instruccio;

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
      switch (button){
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
      if(readInstructionFromRaspberry() == ATURAT)
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
      if(objectesDetectats[1])
      {
        angle = objectesDetectats[2]? -PI/2 : PI/2;  // En cas que la dreta no estigui lliure, gira a l'esquerra (no contemplat cas on hi ha d'haver gir 180º)
        estat = GIRA;
        giraSeguent = AVANCA;
        break;
      }

      // Pulsació al botó central (una o dues vegades)
      switch (button){
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
      if((!objectesDetectats[0] && button == BUTTON0_LONG_PRESS) || (!objectesDetectats[2] && button == BUTTON2_LONG_PRESS))
      {
        angle = button == BUTTON0_LONG_PRESS? -PI/2 : PI/2;
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
    case APROPAMENT:
      //--------------[ ACCIONS ]--------------
      // TODO

      //-----------[ CANVIS D'ESTAT ]----------
      // TODO
      break;
  }
}
