// ---------------------------------------- buttons_control.ino
const int buttonPins[3] = {2, 51, 4};
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
const int trigPins[3] = {43, 47, 51};
const int echoPins[3] = {41, 45, 49};

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

// ---------------------------------------- detect_distances.ino
#define ENA 6
#define IN1 8
#define IN2 7
#define ENB 3
#define IN3 4
#define IN4 5

const float linealSpeed = 100; // cm/s (v)
const float turningSpeed = 30; // rpm (ω)

const float K_vL = 100.0 / 12.0; // rpm/V per al motor esquerre
const float K_vR = 100.0 / 12.0;  // rpm/V per al motor dret

const float V_bat = 12; // V : Bateria
const float r = 4; // cm : Radi de les rodes
const float d = 5; // cm : Distància entre el centre i una roda

// PWM per moviment lineal
const uint8_t linealPWML = 255 * linealSpeed / (V_bat * K_vL * r);
const uint8_t linealPWMR = 255 * linealSpeed / (V_bat * K_vR * r);

// PWM per gir
const uint8_t turningPWML = 255 * (d * turningSpeed) / (V_bat * K_vL * r);
const uint8_t turningPWMR = 255 * (d * turningSpeed) / (V_bat * K_vR * r);

void RevisaObstacles(bool (&objectesDetectats)[3]);

// ---------------------------------------- detect_clap.ino
const int micPin = A0;
const int soundThreshold = 800;
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
          estat = AVANCA;
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
      moveForward();

      //-----------[ CANVIS D'ESTAT ]----------
      // Detectar parets davant
      RevisaObstacles(objectesDetectats);
      if(objectesDetectats[1])
      {
        angle = objectesDetectats[2]? -90.0 : 90.0;  // En cas que la dreta no estigui lliure, gira a l'esquerra (no contemplat cas on hi ha d'aver gir 180º)
        estat = GIRA;
        giraSeguent = AVANCA;
        break;
      }

      // Pulsació al botó central (una o dues vegades)
      button = getButtons(currentTime);
      switch (button){
        case BUTTON0_LONG_PRESS:
        case BUTTON2_LONG_PRESS:
          estat = PETICIO;
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
      RevisaObstacles(objectesDetectats);  // Es pot treure si veiem que a AVANCA ja es fa i no fa falta repetir-ho

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
          break;
      }
      break;

    //========================//
    //      ZEBRA_AVANCA      //
    //========================//
    case ZEBRA_AVANCA:
      //--------------[ ACCIONS ]--------------
      moveForward();

      //-----------[ CANVIS D'ESTAT ]----------
      instruccio = readInstructionFromRaspberry();
      if (instruccio == AVANCA)
      {
          estat = AVANCA;
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
