// ---------------------------------------- buttons_control.ino
const int buttonPins[3] = {22, 24, 26};
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
const int trigPins[3] = {39, 43, 47};
const int echoPins[3] = {37, 41, 45};

const int thresholds[3] = {40, 40, 40};

float distancies[3];

// Màquina d’estats per sensor
enum EstatSensor { IDLE, TRIGGER, ESPERANT_HIGH, ESPERANT_LOW, CALCULAT };
EstatSensor estat_distances[3] = {IDLE, IDLE, IDLE};

unsigned long tempsInici[3];
unsigned long tempsHighStart[3];
unsigned long duracions[3];

const unsigned long timeoutEcho = 30000; 
const unsigned long intervalLectura = 60000;
unsigned long tempsAnteriorLectura = 0;

// ---------------------------------------- motors.ino
#include <RotaryEncoder.h>

#define LPWM1 7
#define LPWM2 8
#define RPWM1 10
#define RPWM2 9

#define LEFT_CHA 20
#define LEFT_CHB 19
#define RIGHT_CHA 2
#define RIGHT_CHB 3

const long TICKS_PER_REV = 50000;
const float r = 7.25; // cm
const float d = 15.0; // cm
const uint8_t PWM = 100;

RotaryEncoder leftEncoder(LEFT_CHA, LEFT_CHB, RotaryEncoder::LatchMode::TWO03);
RotaryEncoder rightEncoder(RIGHT_CHA, RIGHT_CHB, RotaryEncoder::LatchMode::TWO03);

void RevisaObstacles(bool (&objectesDetectats)[3]);

// ---------------------------------------- detect_clap.ino
const int micPin = A0;
const int soundThreshold = 50;
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
bool objectesDetectats[3];
ButtonEvent button = BUTTON_NONE;
RobotState giraSeguent = ATURAT;
float angle=0;
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
  button = getButtons();
  RevisaObstacles(objectesDetectats);
  Serial.println(
    String("\rEstat: ") + estat +
    " | Button: " + button +
    " | Front: " + objectesDetectats[0] +
    " | Left: " + objectesDetectats[1] +
    " | Right: " + objectesDetectats[2]
  );
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
      //moveForward();

      //-----------[ CANVIS D'ESTAT ]----------
      // Detectar parets davant
      // RevisaObstacles(objectesDetectats);
      if(objectesDetectats[1])
      {
        angle = objectesDetectats[2]? -90.0 : 90.0;  // En cas que la dreta no estigui lliure, gira a l'esquerra (no contemplat cas on hi ha d'aver gir 180º)
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
        angle = button == BUTTON0_LONG_PRESS? -90.0 : 90.0;
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
