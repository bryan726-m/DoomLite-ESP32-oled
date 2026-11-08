//CODIGO PRINCIPAL
#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <Freertos/FreeRTOS.h>
#include <Freertos/task.h>
#include <Freertos/semphr.h>

// ── Constantes ────────────────────────────────────────────────
#define MPU_ADDR     0x68
#define PWR_MGMT_1   0x6B
#define ACCEL_XOUT_H 0x3B

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1

#define LOGO_WIDTH  84
#define LOGO_HEIGHT 52

const unsigned char PROGMEM logo1[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0xfc, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x0f, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x07, 0xfe, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0f, 0xff, 0x1f, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xbf, 0xff, 0x9f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x3f, 0xff, 0xff, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x06, 0xfe, 
  0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x01, 0xfe, 0xf0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0xff, 0xc0, 0x07, 0xfd, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x80, 0x3f, 
  0xfb, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x01, 0xff, 0xf7, 0xe0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x3e, 0x03, 0xff, 0xc7, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x0f, 
  0xfe, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x1f, 0xf8, 0x71, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x78, 0x3f, 0xc0, 0x79, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 
  0x7f, 0x00, 0x38, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0xfc, 0xff, 0xfc, 0xf8, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x0f, 0xf1, 0xf9, 0xff, 0xfc, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 
  0xf1, 0xf0, 0x00, 0x3c, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf1, 0xef, 0xff, 0xbc, 0x7f, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xe1, 0xff, 0xff, 0xbc, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x0f, 0xe1, 0xef, 0xff, 0x7c, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xe1, 0xe0, 0x00, 0xfc, 
  0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf1, 0xff, 0xfd, 0xf8, 0x7f, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x0f, 0xf1, 0xff, 0xff, 0xf8, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0xf0, 0x07, 
  0xf0, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x78, 0x1f, 0xe0, 0xf8, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xf8, 0x3c, 0xff, 0xc0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x1b, 
  0xff, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x07, 0xfc, 0x01, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x3c, 0x1f, 0xf7, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 
  0x3f, 0xc7, 0xc7, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x7f, 0x01, 0xef, 0xf0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfd, 0xfe, 0xef, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xff, 0xfb, 0xff, 0xf7, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xf0, 0x00, 0x7f, 0xe0, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x1f, 0xef, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0xef, 0xfe, 0xfb, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xe0, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 
  0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x71, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x17, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 
  0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// ═══════════════════════════════════════════════════════════════
//  SPRITE PIXEL ART DEL DINO
//  Dimensiones del cuerpo y cabeza: 20x16 píxeles.
//  Las patas se dibujan por separado para animarlas.
// ═══════════════════════════════════════════════════════════════

const uint8_t dinoCuerpo[] PROGMEM = {
  0x00, 0x1F, 0xE0,
  0x00, 0x3F, 0xF0,
  0x00, 0x3F, 0xF0,
  0x00, 0x3F, 0xF0,
  0x00, 0x3F, 0xF0,
  0x00, 0x7F, 0xE0,
  0x00, 0xFF, 0x00,
  0x01, 0xFE, 0x00,
  0x07, 0xFE, 0x00,
  0x1F, 0xFE, 0x00,
  0x7F, 0xFE, 0x00,
  0xFF, 0xFF, 0x00,
  0x7F, 0xFE, 0x00,
  0x1F, 0xFC, 0x00,
  0x0F, 0xF8, 0x00,
  0x07, 0xF8, 0x00
};

const uint8_t pteroAlasArriba[] PROGMEM = {
  0x04, 0x00,
  0x0C, 0x00,
  0x1E, 0x00,
  0x3F, 0x00,
  0xFF, 0xF0,
  0x7F, 0xF0,
  0x31, 0xC0
};

const uint8_t pteroAlasAbajo[] PROGMEM = {
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x01, 0x80,
  0xFF, 0xF0,
  0x7F, 0xF0,
  0x3F, 0x00
};

// ═══════════════════════════════════════════════════════════════
//  DIBUJAR PTERODÁCTILO ANIMADO
// ═══════════════════════════════════════════════════════════════


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ── Variables compartidas entre tareas (protegidas por mutex) ──
struct DisplayData {
  uint8_t throttlePercent;
  int8_t  rotation;
  uint8_t roll;
  uint8_t aux3;
  uint8_t aux2;
  uint8_t aux1;
  float   batteryVoltage;
  float   batteryPercent;
  bool    showMenuTriangle;
  bool    throttleUpArrows;
};
DisplayData dispData;

volatile bool menuActive       = false;
volatile bool dinoActive       = false;
volatile bool snakeActive      = false;
volatile bool invadersActive   = false;
volatile bool raceActive       = false;
volatile bool doomActive       = false;   
volatile bool HabilitarOled    = false;
unsigned long BanderaTiempoF   = 300;

SemaphoreHandle_t i2cMutex;

// Control de vibración
const int PIN_MOTOR = 13;
bool vibracionActiva = true;

// Potencia máxima del motor (%)
const int VIBRACION_MAXIMA = 35;
const int FRECUENCIA_PWM = 30000;
const unsigned long VIBRACION_ARRANQUE_MS = 700UL; // 5 segundos

bool pulsoJuegoActivo = false;

uint8_t pulsoJuegoPotencia = 0;
unsigned long pulsoJuegoInicio = 0;
unsigned long pulsoJuegoDuracion = 0;

bool dinoCrouchAnterior = false;


// ── Pines y variables generales ──────────────────────────────
int ledPinB1 = 16;
bool botonSt1 = false;
unsigned long botonT1 = 0;

int ledPinB2 = 17;
bool botonSt2 = false;
unsigned long botonT2 = 0;

double accelX, accelY, accelZ;
double rotationAngle;

static unsigned long BanderaTiempoI = 0;
static unsigned long BanderaTBoton  = 200;

int swicth1 = 25;
int swicth2 = 26;

//===================PB3======================================
const int botonPinPB3 = 15;
int estadoPB3 = 0; //Aux3

unsigned long tiempoInicioPB3 = 0;
const unsigned long tiempoPresionadoPB3= 1000;

bool presionadoPB3 = false;
bool yaCambioPB3 = false;
//=============================================================

unsigned long tiempoUltimaImpresion = 0;
const unsigned long tiempoEspera    = 4000;
bool mensajeImpreso = false;

int InputA0 = 36;   // Throttle  (joystick derecho, eje X: 0=atras  4095=adelante)
int InputA1 = 39;   // Pitch     (joystick derecho, eje Y: 0=izq    4095=der)
int InputA2 = 34;   // Yaw       (joystick izquierdo, eje Y — navega menú vertical)
int InputA3 = 35;   // Roll      (joystick izquierdo, eje X — navega menú HORIZONTAL / Snake)
int InputA6 = 32;   // Aux1
const int InputBattery = 33;

const float BAT_MAX_V = 8.25f;
const float BAT_MIN_V = 6.00f;
const unsigned long BAT_REFRESH_MS = 8000UL;

#define ADC_MAX         4095
#define ADC_MID         2048
#define ADC_THR_LOW     2048
#define ADC_THR_MID     3000
#define NAV_H           2800
#define NAV_L           1300
#define NAV_NEUTRAL_L   1500
#define NAV_NEUTRAL_H   2600

// Umbrales para la navegación horizontal (InputA3) en el menú de juegos
#define GAME_SEL_H  2800   // hacia la derecha → Snake
#define GAME_SEL_L  1300   // hacia la izquierda → Dino
#define GAME_NEUT_L 1500
#define GAME_NEUT_H 2600

uint8_t throttleBuffer, pitchBuffer, rollBuffer, yawBuffer,
        aux1Buffer, aux2Buffer, throttleBufferPercentage;
int8_t rotationBuffer;

const uint64_t PIPE_OUT = 0x70BCD91E8E81LL;
RF24 radio(4, 5);

struct ControlSignal {
  uint8_t throttle;
  uint8_t pitch;
  uint8_t roll;
  uint8_t yaw;
  uint8_t aux1;
  uint8_t aux2;
  //uint8_t aux3;
  int8_t  rotation;
};
ControlSignal data;

// EEPROM
const int EEPROM_ADDR     = 0;
const int EEPROM_REFRESH  = 6;
const int EEPROM_CHANNEL  = 8;
const int EEPROM_PA       = 9;
const int EEPROM_DATARATE = 10;
const int EEPROM_DEADZONE = 11;

bool throttleReverse = false;
bool pitchReverse    = false;
bool yawReverse      = false;
bool rollReverse     = false;
bool aux1Reverse     = false;
bool mapthrottle     = false;

uint8_t rfChannel   = 100;
uint8_t rfPALevel   = 2;
uint8_t rfDataRate  = 0;
uint8_t joyDeadzone = 0;

const char* paLabels[]       = { "MIN", "LOW", "HIGH", "MAX" };
const char* dataRateLabels[] = { "250K", "1M", "2M" };

struct MenuItem {
  const char* label;
  bool*       value;
};
MenuItem menuItems[] = {
  { "Thrott ", &throttleReverse },
  { "Pitch  ", &pitchReverse    },
  { "Yaw    ", &yawReverse      },
  { "Roll   ", &rollReverse     },
  { "Aux1   ", &aux1Reverse     },
  { "Thrott2", &mapthrottle     }
};

const int MenuContador   = sizeof(menuItems) / sizeof(MenuItem);
int  BanderaContador     = 0;
bool MenuBanderaOn       = false;

const int IDX_VIBRADOR  = MenuContador;
const int IDX_AJUSTES   = MenuContador + 1;
const int IDX_GAMES     = MenuContador + 2;
const int IDX_MENU_OFF  = MenuContador + 3;

const int TOTAL_OPCIONES = MenuContador + 4;

bool subMenuAjustesActivo = false;
int  subMenuIdx           = 0;
#define SUB_COUNT 4

// ── Submenú de juegos ─────────────────────────────────────────
bool juegoSubMenuActivo  = false;
int  juegoSubMenuSel     = 0;
static bool juegoNavListo      = true;
static unsigned long juegoNavMs = 0;


// ── DINO ─────────────────────────────────────────────────────
#define DINO_X           10
#define DINO_W           20
#define DINO_H           20
#define DINO_GROUND_Y    54
#define DINO_STAND_Y     (DINO_GROUND_Y - DINO_H)
#define DINO_JUMP_VY    -7.0f
#define DINO_GRAVITY     0.85f
#define DINO_FRAME_MS    25
#define DINO_SPEED_INIT  1.6f
#define DINO_SPEED_MAX   6.2f
#define DINO_OBS_COUNT   2
#define DINO_AUTOEXT_MS  5000UL

float    dinoY;
float    dinoVY;
bool     dinoOnGround;
uint16_t dinoScore;
float    dinoSpeed;
bool     dinoGameOver, dinoGameOverDraw;
unsigned long dinoLastFrame, dinoGameOverMs;
bool     dinoLegToggle;
unsigned long dinoLastLegAnim;
bool     dinoPrevB1, dinoPrevB2;
bool     dinoJumpHeld;
bool     dinoCrouching = false;

struct DinoObs {
  float x;
  uint8_t h;       // Altura del cactus
  int8_t y;        // Posición vertical del pterodáctilo
  bool active;
  bool esPtero;    // false = cactus, true = pterodáctilo
};

DinoObs dinoObs[DINO_OBS_COUNT];

unsigned long dinoLastSpawn;
unsigned int  dinoNextSpawnMs;

// ── SNAKE ─────────────────────────────────────────────────────
// El área de juego ocupa 10..127 en X y 10..63 en Y (borde de 1 px)
// Tamaño de cada celda: 4 px → área útil ≈ 29×13 celdas
#define SNAKE_CELL      4           // píxeles por celda
#define SNAKE_COLS      29          // (128 - 10) / 4  ≈ 29  (10 px reservado para borde izq)
#define SNAKE_ROWS      13          // (64  - 10) / 4  ≈ 13  (10 px reservado para cabecera)
#define SNAKE_ORIGIN_X  10          // píxel X donde empieza la cuadrícula
#define SNAKE_ORIGIN_Y  10          // píxel Y donde empieza la cuadrícula
#define SNAKE_MAX_LEN   (SNAKE_COLS * SNAKE_ROWS)
#define SNAKE_FRAME_MS  150         // ms entre movimientos
#define SNAKE_AUTOEXT_MS 5000UL     // ms hasta volver al menú tras game over

// Direcciones de la serpiente
#define SDIR_UP    0
#define SDIR_RIGHT 1
#define SDIR_DOWN  2
#define SDIR_LEFT  3

struct SnakeCell { int8_t col; int8_t row; };

SnakeCell snakeBody[SNAKE_MAX_LEN];
int       snakeLen;
uint8_t   snakeDir;
uint8_t   snakeNextDir;   // dirección pedida (se aplica al inicio del siguiente frame)
int8_t    snakeFoodCol;
int8_t    snakeFoodRow;
uint16_t  snakeScore;
bool      snakeGameOver;
bool      snakeGameOverDraw;
unsigned long snakeLastFrame;
unsigned long snakeGameOverMs;
bool      snakePrevB1, snakePrevB2;

// Control de joystick para Snake (umbral de zona muerta)
#define SNAKE_JOY_THRESH  1200      // desviación mínima del centro (2048)

//====================SI===========================================
// ── SPACE INVADERS ────────────────────────────────────────────
#define INV_COLS          7
#define INV_ROWS          3
#define INV_ALIEN_W       11
#define INV_ALIEN_H       8
#define INV_PITCH_X       15
#define INV_PITCH_Y       10
#define INV_SHIP_W        8
#define INV_SHIP_H        8
#define INV_SHIP_Y        55
#define INV_SHIP_STEP     4       // px que avanza la nave por cada empujón del joystick
#define INV_MAX_BULLETS   6
#define INV_BULLET_W      2
#define INV_BULLET_H      4
#define INV_BULLET_STEP   4
#define INV_FRAME_MS      33
#define INV_FIRE_MS       400
#define INV_BANNER_MS     1000
#define INV_AUTOEXT_MS    5000UL
#define INV_EXIT_HOLD_MS  1500

// Joystick de movimiento (0 = izquierda, 4095 = derecha)
#define INV_JOY_PIN       InputA1   // joystick izquierdo, eje X
#define INV_JOY_PIN2      InputA3   // ← NUEVO: joystick derecho, eje horizontal
#define INV_JOY_RIGHT     2800
#define INV_JOY_LEFT      1300
#define INV_JOY_NEUT_L    1500
#define INV_JOY_NEUT_H    2600

#define INV_FIRE_MIN_MS   90      // mínimo entre balas por pulsación (anti-rebote)
#define INV_JOY_HOLD_MS   250     // tiempo sostenido antes de empezar el movimiento continuo
#define INV_JOY_REPEAT_MS 35      // ms entre pasos en movimiento continuo
#define INV_STARS         23      // cantidad de estrellas de fondo

const unsigned char PROGMEM inv_ship[] = {
  0b00011000,
  0b00011000,
  0b10011001,
  0b11011011,
  0b11011011,
  0b01111110,
  0b00111100,
  0b00011000
};

const unsigned char PROGMEM inv_alien_a[] = {
  0x20,0x80, 0x11,0x00, 0x3F,0x80, 0x6E,0xC0,
  0xFF,0xE0, 0xBF,0xA0, 0xA0,0xA0, 0x1B,0x00
};
const unsigned char PROGMEM inv_alien_b[] = {
  0x20,0x80, 0x91,0x20, 0xBF,0xA0, 0xEE,0xE0,
  0xFF,0xE0, 0x7F,0xC0, 0x20,0x80, 0x40,0x40
};

struct InvBullet { int16_t x; int16_t y; bool active; };
InvBullet invBullets[INV_MAX_BULLETS];

bool     invAlive[INV_ROWS][INV_COLS];
int      invAliveCount;
int      invFleetX, invFleetY;
int8_t   invDir;
bool     invAnimFrame;
int      invShipX;
uint16_t invScore;
uint8_t  invLevel;
bool     invGameOver, invGameOverDraw, invGoArmed, invBanner;
//bool invJoyListo = false;
int8_t        invJoyDir = 2;      // 0=centro, 1=derecha, -1=izquierda, 2=bloqueado hasta pasar por el centro
unsigned long invJoyPushMs = 0;   // cuándo empezó el empujón actual
unsigned long invJoyStepMs = 0;   // cuándo se dio el último paso
bool          invFirePrev = true; // estado del disparo en el ciclo anterior

struct InvStar { int16_t x; int16_t y10; uint8_t size; };   // y10 = posición Y x10
InvStar invStars[INV_STARS];

unsigned long invBannerMs, invLastFrame, invLastFleet, invLastFire;
unsigned long invGameOverMs, invExitHoldMs;
//=======================SI======================================

// ═══════════════════════════════════════════════════════════════
//  PROTOTIPOS
// ═══════════════════════════════════════════════════════════════
void volverAlMenu();
void eepromUpdate(int addr, uint8_t val);
void IniciarMPU();
void ProtocoloMPUData();
double calcularAngulo(double x, double y, double z);
int MapearJoysticks(int val, int lower, int middle, int upper, bool reverse);
void AplicarConfigRF();
void CargarDatos();
void GuardarDatos();
void GuardarAjustes();
int NavJoystick(bool &listo, unsigned long &lastMs);
void VentanaSubMenuAjustes();
void VentanaMenu();
void EnviarDatos();
void BotonesI();
void dibujarIndicadorInclinacion(int angulo);
// Dino
void IniciarDino();
void ActualizarDino();
void DibujarDinoSprite(int x, int y, bool legToggle, bool onGround);
void DibujarCactus(int cx, uint8_t ch);
unsigned int GenerarIntervaloSpawn();
void DibujarEscenaDino();
void DibujarGameOverDino();
// Snake
void IniciarSnake();
void ActualizarSnake();
void snakeColocarComida();
void DibujarEscenaSnake();
void DibujarGameOverSnake();
// Space Invaders
void IniciarInvaders();
void ActualizarInvaders();
void InvCargarNivel();
void InvDibujarBanner();
void InvDibujarEscena();
void InvDibujarGameOver();
void InvSalir();
void InvMoverNave(int8_t dir);
void InvIniciarEstrellas();
void InvMoverEstrellas();
void InvDibujarEstrellas();
// Motor Vibrador
void ActualizarVibrador();
void SolicitarPulsoJuego(uint8_t potencia, unsigned long duracion);

#include "RoadRush.h"
#include "DoomLite.h"


// ═══════════════════════════════════════════════════════════════
//  TAREA OLED (Core 0)
// ═══════════════════════════════════════════════════════════════
void oledTask(void *pvParameters) {
  unsigned long previousMillis = 0;
  while (1) {
    unsigned long now = millis();
    if (now - previousMillis >= BanderaTiempoF) {
      previousMillis = now;

      if (HabilitarOled && !menuActive && !dinoActive && !snakeActive && !invadersActive && !raceActive && !doomActive) {
        if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
          display.clearDisplay();
          display.setTextSize(2); display.setTextColor(WHITE);
          display.setCursor(0, 0);
          display.print(F("A:")); display.print(dispData.throttlePercent); display.println(F("%"));
          display.println(' ');
          display.setTextSize(1); display.setTextColor(WHITE);
          display.setCursor(0, 22); display.print(F("ROT:")); display.print(dispData.rotation); display.write(248); display.println();
          display.println(' ');
          display.setCursor(0, 39); display.print(F("DIR:")); display.println(dispData.roll);
          display.setCursor(0, 56); display.print(F("POT:")); display.println(dispData.aux1);
          if (dispData.roll <= 109) { display.fillTriangle(56, 44, 56, 50, 48, 47, WHITE); }
          if (dispData.roll >= 140) { display.fillTriangle(72, 44, 72, 50, 80, 47, WHITE); }
          if (dispData.throttleUpArrows) {
            if (dispData.throttlePercent >= 10) { display.fillTriangle(64, 50, 61, 54, 67, 54, WHITE); }
            if (dispData.throttlePercent >= 50) { display.fillTriangle(64, 44, 61, 48, 67, 48, WHITE); }
            if (dispData.throttlePercent >= 90) { display.fillTriangle(64, 38, 61, 42, 67, 42, WHITE); }
          }
          display.setCursor(80, 2);   display.print(F("BAT:")); display.print(dispData.batteryVoltage, 2);
          display.drawRect(100, 20, 24, 11, WHITE);
          display.drawRect(124, 23, 2, 5, WHITE);
          display.fillRect(101, 21, 22, 9, BLACK);
          display.setCursor(105, 22); display.print(dispData.batteryPercent, 0); display.println(F("%"));
          display.setCursor(98, 39);  display.print(F("DIG:")); display.println(dispData.aux2);
          display.setCursor(98, 56);  display.print(F("BOT:")); display.println(dispData.aux3);
          display.setCursor(52, 56);  display.print(F("Menu"));
          if (dispData.showMenuTriangle) display.fillTriangle(60, 47, 68, 47, 64, 53, WHITE);
          int lado = 8;
          //display.fillRect(SCREEN_WIDTH - lado, SCREEN_HEIGHT - lado, lado, lado, SSD1306_WHITE);
          //display.fillRect(0, SCREEN_HEIGHT - lado, lado, lado, SSD1306_WHITE);
          dibujarIndicadorInclinacion(dispData.rotation);
          display.display();
          xSemaphoreGive(i2cMutex);
        }
      } else if (HabilitarOled && !menuActive && !dinoActive && !snakeActive && !invadersActive && !raceActive && !doomActive) {
        if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
          display.clearDisplay(); display.display();
          xSemaphoreGive(i2cMutex);
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// ═══════════════════════════════════════════════════════════════
//  FUNCIONES AUXILIARES
// ═══════════════════════════════════════════════════════════════
void eepromUpdate(int addr, uint8_t val) {
  if (EEPROM.read(addr) != val) EEPROM.write(addr, val);
}

void IniciarMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(PWR_MGMT_1);
  Wire.write(0);
  Wire.endTransmission(true);
}

void ProtocoloMPUData() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(ACCEL_XOUT_H);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  accelX = (int16_t)(Wire.read() << 8 | Wire.read());
  accelY = (int16_t)(Wire.read() << 8 | Wire.read());
  accelZ = (int16_t)(Wire.read() << 8 | Wire.read());
}

double calcularAngulo(double x, double y, double z) {
  return atan2(x, sqrt(y*y + z*z)) * 180.0 / PI;
}

int MapearJoysticks(int val, int lower, int middle, int upper, bool reverse) {
  val = constrain(val, lower, upper);
  if (joyDeadzone > 0) {
    int dz = map(joyDeadzone, 0, 30, 0, 200);
    if (abs(val - middle) < dz) val = middle;
  }
  if (val < middle) val = map(val, lower,  middle, 0,   127);
  else              val = map(val, middle, upper,  128, 255);
  return (reverse ? 255 - val : val);
}

void AplicarConfigRF() {
  radio.setChannel(rfChannel);
  switch (rfPALevel) {
    case 0: radio.setPALevel(RF24_PA_MIN);  break;
    case 1: radio.setPALevel(RF24_PA_LOW);  break;
    case 2: radio.setPALevel(RF24_PA_HIGH); break;
    case 3: radio.setPALevel(RF24_PA_MAX);  break;
  }
  switch (rfDataRate) {
    case 0: radio.setDataRate(RF24_250KBPS); break;
    case 1: radio.setDataRate(RF24_1MBPS);   break;
    case 2: radio.setDataRate(RF24_2MBPS);   break;
  }
}

void CargarDatos() {
  throttleReverse = EEPROM.read(EEPROM_ADDR)     == 1;
  pitchReverse    = EEPROM.read(EEPROM_ADDR + 1) == 1;
  yawReverse      = EEPROM.read(EEPROM_ADDR + 2) == 1;
  rollReverse     = EEPROM.read(EEPROM_ADDR + 3) == 1;
  aux1Reverse     = EEPROM.read(EEPROM_ADDR + 4) == 1;
  mapthrottle     = EEPROM.read(EEPROM_ADDR + 5) == 1;

  uint16_t rf = ((uint16_t)EEPROM.read(EEPROM_REFRESH) << 8) |
                 (uint16_t)EEPROM.read(EEPROM_REFRESH + 1);
  if (rf >= 50 && rf <= 1000) BanderaTiempoF = rf;

  uint8_t ch = EEPROM.read(EEPROM_CHANNEL);
  if (ch >= 1 && ch <= 125) rfChannel = ch;
  uint8_t pa = EEPROM.read(EEPROM_PA);
  if (pa <= 3) rfPALevel = pa;
  uint8_t dr = EEPROM.read(EEPROM_DATARATE);
  if (dr <= 2) rfDataRate = dr;
  uint8_t dz = EEPROM.read(EEPROM_DEADZONE);
  if (dz <= 30 && (dz % 2 == 0)) joyDeadzone = dz;
}

void GuardarDatos() {
  eepromUpdate(EEPROM_ADDR,     throttleReverse);
  eepromUpdate(EEPROM_ADDR + 1, pitchReverse);
  eepromUpdate(EEPROM_ADDR + 2, yawReverse);
  eepromUpdate(EEPROM_ADDR + 3, rollReverse);
  eepromUpdate(EEPROM_ADDR + 4, aux1Reverse);
  eepromUpdate(EEPROM_ADDR + 5, mapthrottle);
  EEPROM.commit();
}

void GuardarAjustes() {
  eepromUpdate(EEPROM_REFRESH,     (uint8_t)((BanderaTiempoF >> 8) & 0xFF));
  eepromUpdate(EEPROM_REFRESH + 1, (uint8_t)(BanderaTiempoF & 0xFF));
  eepromUpdate(EEPROM_CHANNEL,     rfChannel);
  eepromUpdate(EEPROM_PA,          rfPALevel);
  eepromUpdate(EEPROM_DATARATE,    rfDataRate);
  eepromUpdate(EEPROM_DEADZONE,    joyDeadzone);
  EEPROM.commit();
}

// NavJoystick — usa InputA2 (eje vertical del joystick izquierdo)
int NavJoystick(bool &listo, unsigned long &lastMs) {
  int joyVal = analogRead(InputA2);
  if (!listo) {
    if (joyVal >= NAV_NEUTRAL_L && joyVal <= NAV_NEUTRAL_H) listo = true;
    return 0;
  }
  if (millis() - lastMs < 200) return 0;
  if (joyVal > NAV_H) { lastMs = millis(); listo = false; return +1; }
  if (joyVal < NAV_L) { lastMs = millis(); listo = false; return -1; }
  return 0;
}

// NavJoystickH — usa InputA3 (eje horizontal del joystick izquierdo)
// Devuelve +1 (derecha → Snake) o -1 (izquierda → Dino) o 0
int NavJoystickH(bool &listo, unsigned long &lastMs) {
  int joyVal = analogRead(InputA3);
  if (!listo) {
    if (joyVal >= GAME_NEUT_L && joyVal <= GAME_NEUT_H) listo = true;
    return 0;
  }
  if (millis() - lastMs < 200) return 0;
  if (joyVal > GAME_SEL_H) { lastMs = millis(); listo = false; return +1; }
  if (joyVal < GAME_SEL_L) { lastMs = millis(); listo = false; return -1; }
  return 0;
}

// ═══════════════════════════════════════════════════════════════
//  SUB-MENÚ AJUSTES (sin cambios)
// ═══════════════════════════════════════════════════════════════
// ═══════════════════════════════════════════════════════════════
//  INDICADOR DE SELECCIÓN PARA OLED 128x64
//  Triángulo sólido blanco apuntando hacia la derecha.
//  Tamaño aproximado: 6x6 píxeles.
// ═══════════════════════════════════════════════════════════════

void DibujarIndicadorSeleccion(int x, int y) {
  display.fillTriangle(
    x,     y + 1,  // Vértice superior izquierdo
    x,     y + 6,  // Vértice inferior izquierdo
    x + 5, y + 3,  // Punta hacia la derecha
    WHITE
  );
}


// ═══════════════════════════════════════════════════════════════
//  SUBMENÚ DE AJUSTES RF
// ═══════════════════════════════════════════════════════════════

void VentanaSubMenuAjustes() {
  static bool navListo = true;
  static unsigned long navMs = 0;

  int nav = NavJoystick(navListo, navMs);

  if (nav > 0) subMenuIdx = (subMenuIdx + 1) % SUB_COUNT;
  if (nav < 0) subMenuIdx = (subMenuIdx - 1 + SUB_COUNT) % SUB_COUNT;

  // ── Botón B2: cambiar el valor seleccionado ────────────────
  static bool b2Ant = HIGH;
  static unsigned long b2Ms = 0;

  bool b2 = digitalRead(ledPinB2);
  bool b2Pulsado = false;

  if (b2Ant == HIGH && b2 == LOW &&
      (millis() - b2Ms >= BanderaTBoton)) {
    b2Ms = millis();
    b2Pulsado = true;
  }

  b2Ant = b2;

  if (b2Pulsado) {
    switch (subMenuIdx) {
      case 0:
        BanderaTiempoF += 50;
        if (BanderaTiempoF > 1000) BanderaTiempoF = 50;
        break;

      case 1:
        rfChannel++;
        if (rfChannel > 125) rfChannel = 1;
        break;

      case 2:
        rfPALevel = (rfPALevel + 1) % 4;
        AplicarConfigRF();
        break;

      case 3:
        rfDataRate = (rfDataRate + 1) % 3;
        AplicarConfigRF();
        break;
    }
  }

  // ── Botón B1: guardar y salir ──────────────────────────────
  static bool b1Ant = HIGH;
  static unsigned long b1Ms = 0;

  bool b1 = digitalRead(ledPinB1);

  if (b1Ant == HIGH && b1 == LOW &&
      (millis() - b1Ms >= BanderaTBoton)) {
    b1Ms = millis();

    GuardarAjustes();
    AplicarConfigRF();

    subMenuAjustesActivo = false;
    subMenuIdx = 0;
    navListo = true;
    botonSt1 = false;
    botonSt2 = false;

    if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
      display.clearDisplay();
      display.display();
      xSemaphoreGive(i2cMutex);
    }

    return;
  }

  b1Ant = b1;

  // ── Dibujar menú ──────────────────────────────────────────
  if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(0, 0);
    display.print(F("    -- Ajustes --"));

    display.drawFastHLine(0, 8, SCREEN_WIDTH, WHITE);
    display.println(' ');

    const char* labels[SUB_COUNT] = {
      " Refresco ms",
      " Canal RF   ",
      " Potencia TX",
      " Vel.Datos "
    };

    for (int i = 0; i < SUB_COUNT; i++) {
      int y = 10 + i * 10;

      // Triángulo en lugar del carácter '>'
      if (i == subMenuIdx) {
        DibujarIndicadorSeleccion(0, y);
      }

      // Se conserva la posición original del texto
      display.setCursor(6, y);
      display.print(labels[i]);
      display.print(':');

      switch (i) {
        case 0:
          display.print(BanderaTiempoF);
          break;

        case 1:
          display.print(rfChannel);
          break;

        case 2:
          display.print(paLabels[rfPALevel]);
          break;

        case 3:
          display.print(dataRateLabels[rfDataRate]);
          break;
      }
    }

    display.setCursor(0, SCREEN_HEIGHT - 8);
    display.print(F("B2:cambiar B1:guardar"));

    display.display();
    xSemaphoreGive(i2cMutex);
  }
}

void DibujarPterodactilo(int x, int y, bool alasArriba) {
  const uint8_t* sprite = alasArriba
                          ? pteroAlasArriba
                          : pteroAlasAbajo;

  display.drawBitmap(
    x, y,
    sprite,
    12, 7,
    SSD1306_WHITE
  );
}

//=============Motor vibrador=====================================
void ActualizarVibrador() {

  static int nivelActual = 0;
  static int ultimoPWM = -1;

  // Umbrales de aceleración
  const int umbrales[] = {10, 30, 50, 70, 90};

  // Potencia fija para cada nivel (% PWM)
  const int potencias[] = {
    0, 18, 20, 22, 24, 26
  };

  // ── Vibración desactivada o menú ────────────────────────────
  if (!vibracionActiva || menuActive) {

    pulsoJuegoActivo = false;
    nivelActual = 0;

    if (ultimoPWM != 0) {
      analogWrite(PIN_MOTOR, 0);
      ultimoPWM = 0;
    }

    return;
  }

  // ═══════════════════════════════════════════════════════════
  //  VIBRACIÓN DE LOS JUEGOS
  // ═══════════════════════════════════════════════════════════

  if (dinoActive || snakeActive || invadersActive || raceActive || doomActive) {

    nivelActual = 0;

    int pwmPulso = 0;

    if (pulsoJuegoActivo) {

      if (millis() - pulsoJuegoInicio < pulsoJuegoDuracion) {

        int potencia = constrain(
          (int)pulsoJuegoPotencia, 0, 100
        );

        pwmPulso = map(potencia, 0, 100, 0, 255);

      } else {

        // Finalizar pulso
        pulsoJuegoActivo = false;
        pulsoJuegoPotencia = 0;
      }
    }

    // Actualizar únicamente cuando cambie el PWM
    if (pwmPulso != ultimoPWM) {
      analogWrite(PIN_MOTOR, pwmPulso);
      ultimoPWM = pwmPulso;
    }

    return;
  }

  // Fuera de los juegos no debe quedar ningún pulso pendiente
  pulsoJuegoActivo = false;
  pulsoJuegoPotencia = 0;

  // ═══════════════════════════════════════════════════════════
  //  OBTENER ACELERACIÓN ACTUAL
  // ═══════════════════════════════════════════════════════════

  uint8_t aceleracion = 0;

  if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(10)) == pdTRUE) {

    aceleracion = dispData.throttlePercent;

    xSemaphoreGive(i2cMutex);

  } else {
    return;
  }

  aceleracion = constrain((int)aceleracion, 0, 100);

  // ═══════════════════════════════════════════════════════════
  //  SELECCIÓN DE NIVEL CON HISTÉRESIS
  // ═══════════════════════════════════════════════════════════

  const int HIST = 2;

  // Aumentar nivel
  while (nivelActual < 5 &&
         aceleracion >= umbrales[nivelActual] + HIST) {

    nivelActual++;
  }

  // Disminuir nivel
  while (nivelActual > 0 &&
         aceleracion < umbrales[nivelActual - 1] - HIST) {

    nivelActual--;
  }

  // ═══════════════════════════════════════════════════════════
  //  APLICAR POTENCIA FIJA
  // ═══════════════════════════════════════════════════════════

  int potencia = constrain(potencias[nivelActual], 0, 100);

  int pwmMotor = map(potencia, 0, 100, 0, 255);

  if (pwmMotor != ultimoPWM) {

    analogWrite(PIN_MOTOR, pwmMotor);
    ultimoPWM = pwmMotor;
  }
}
//================================================================

// ═══════════════════════════════════════════════════════════════
//  SUBMENÚ DE JUEGOS
//  0 = Snake, 1 = Dino, 2 = Back
// ═══════════════════════════════════════════════════════════════

void VentanaJuegosSubMenu() {
  const int JUEGO_COUNT = 6;

  int nav = NavJoystick(juegoNavListo, juegoNavMs);
  if (nav > 0) juegoSubMenuSel = (juegoSubMenuSel + 1) % JUEGO_COUNT;
  if (nav < 0) juegoSubMenuSel = (juegoSubMenuSel - 1 + JUEGO_COUNT) % JUEGO_COUNT;

  if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    if (juegoSubMenuSel == 0) DibujarIndicadorSeleccion(0, 0);
    display.setCursor(12, 0);  display.print(F("Snake"));

    if (juegoSubMenuSel == 1) DibujarIndicadorSeleccion(0, 10);
    display.setCursor(12, 10); display.print(F("Dino"));

    if (juegoSubMenuSel == 2) DibujarIndicadorSeleccion(0, 20);
    display.setCursor(12, 20); display.print(F("SpaceInvaders"));

    if (juegoSubMenuSel == 3) DibujarIndicadorSeleccion(0, 30);   
    display.setCursor(12, 30); display.print(F("RoadRush"));       

    if (juegoSubMenuSel == 4) DibujarIndicadorSeleccion(0, 40);   
    display.setCursor(12, 40); display.print(F("Doom"));          

    int yBack = SCREEN_HEIGHT - 8;
    if (juegoSubMenuSel == 5) DibujarIndicadorSeleccion(98, yBack);
    display.setCursor(104, yBack);
    display.print(F("Back"));

    display.display();
    xSemaphoreGive(i2cMutex);
  }

  bool confirmar = false;
  if (botonSt1) { botonSt1 = false; confirmar = true; }
  if (botonSt2) { botonSt2 = false; confirmar = true; }

  if (confirmar) {
    juegoNavListo = true;

    if (juegoSubMenuSel == 5) {                 // Back
      juegoSubMenuActivo = false;
      if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
        display.clearDisplay(); display.display();
        xSemaphoreGive(i2cMutex);
      }
    } else if (juegoSubMenuSel == 0) {          // Snake
      juegoSubMenuActivo = false; menuActive = false; snakeActive = true;
      IniciarSnake();
    } else if (juegoSubMenuSel == 2) {          // Space Invaders
      juegoSubMenuActivo = false; invadersActive = true; menuActive = false;
      IniciarInvaders();
    } else if (juegoSubMenuSel == 3) {          // Road Rush
      juegoSubMenuActivo = false; raceActive = true; menuActive = false;
      IniciarRoadRush();
    } else if (juegoSubMenuSel == 4) {          // Doom 
      juegoSubMenuActivo = false; doomActive = true; menuActive = false;
      IniciarDoom();
    } else {                                    // Dino
      juegoSubMenuActivo = false; menuActive = false; dinoActive = true;
      IniciarDino();
    }
  }
}

// ═══════════════════════════════════════════════════════════════
//  MENÚ PRINCIPAL
// ═══════════════════════════════════════════════════════════════


void VentanaMenu() {

  // Entrar en los submenús correspondientes
  if (subMenuAjustesActivo) {
    VentanaSubMenuAjustes();
    return;
  }

  if (juegoSubMenuActivo) {
    VentanaJuegosSubMenu();
    return;
  }

  static bool navListo = true;
  static unsigned long navMs = 0;

  // ── Navegación vertical ──────────────────────────────────
  int nav = NavJoystick(navListo, navMs);

  if (nav > 0) {
    BanderaContador =
      (BanderaContador + 1) % TOTAL_OPCIONES;
  }

  if (nav < 0) {
    BanderaContador =
      (BanderaContador - 1 + TOTAL_OPCIONES) % TOTAL_OPCIONES;
  }

  // ── Dibujar menú ─────────────────────────────────────────
  if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // Opciones booleanas
    for (int i = 0; i < MenuContador; i++) {

      int y = i * 8;

      if (i == BanderaContador) {
        DibujarIndicadorSeleccion(0, y);
      }

      display.setCursor(12, y);
      display.print(menuItems[i].label);
      display.print(F(": "));
      display.print(*menuItems[i].value ? 'T' : 'F');
    }

    // ── VM:ON / VM:OFF (esquina superior derecha) ───────────
    if (BanderaContador == IDX_VIBRADOR) {
      DibujarIndicadorSeleccion(78, 0);
    }

    display.setCursor(86, 0);
    display.print(F("VM:"));

    display.print(
      vibracionActiva ? F("ON") : F("OFF")
    );

    // ── Fila inferior: ConfRF | Games | Back ───────────────

    int yInferior = SCREEN_HEIGHT - 8;

    // ConfRF
    if (BanderaContador == IDX_AJUSTES) {
      DibujarIndicadorSeleccion(0, yInferior);
    }

    display.setCursor(6, yInferior);
    display.print(F("ConfRF"));

    // Games
    if (BanderaContador == IDX_GAMES) {
      DibujarIndicadorSeleccion(50, yInferior);
    }

    display.setCursor(56, yInferior);
    display.print(F("Games"));

    // Back
    if (BanderaContador == IDX_MENU_OFF) {
      DibujarIndicadorSeleccion(98, yInferior);
    }

    display.setCursor(104, yInferior);
    display.print(F("Back"));

    display.display();

    xSemaphoreGive(i2cMutex);
  }

  // ── Confirmar selección ──────────────────────────────────
  bool confirmar = false;

  if (botonSt2) {
    botonSt2 = false;
    confirmar = true;
  }

  if (botonSt1) {
    botonSt1 = false;
    confirmar = true;
  }

  if (confirmar) {

    navListo = true;

    if (BanderaContador == IDX_VIBRADOR) {

      // Activar/desactivar el vibrador
      vibracionActiva = !vibracionActiva;

      // Apagar inmediatamente cuando se desactiva.
      if (!vibracionActiva) {
        analogWrite(PIN_MOTOR, 0);
      }

      Serial.println(
        vibracionActiva ? "VM:ON" : "VM:OFF"
      );

    } else if (BanderaContador == IDX_MENU_OFF) {

      // Salir del menú
      menuActive = false;

    } else if (BanderaContador == IDX_AJUSTES) {

      // Abrir Ajustes RF
      subMenuAjustesActivo = true;
      subMenuIdx = 0;

    } else if (BanderaContador == IDX_GAMES) {

      // Abrir selector de juegos
      juegoSubMenuActivo = true;
      juegoSubMenuSel = 0;
      juegoNavListo = true;

    } else {

      // Activar/desactivar opción booleana
      *menuItems[BanderaContador].value =
        !(*menuItems[BanderaContador].value);

      GuardarDatos();
    }
  }
}


// ═══════════════════════════════════════════════════════════════
//  DINO (sin cambios respecto al original)
// ═══════════════════════════════════════════════════════════════
void DibujarDinoSprite(int x, int y, bool legToggle, bool onGround) {

  // ── POSTURA AGACHADA ───────────────────────────────────────
  if (dinoCrouching) {

    // Mantener los pies sobre el suelo.
    int cy = DINO_GROUND_Y - 12;

    // Cola
    display.fillTriangle(
      x, cy + 8,
      x + 6, cy + 5,
      x + 6, cy + 10,
      WHITE
    );

    // Cuerpo horizontal
    display.fillRect(x + 4, cy + 6, 11, 5, WHITE);

    // Cabeza baja
    display.fillRect(x + 12, cy + 3, 6, 5, WHITE);

    // Hocico
    display.fillRect(x + 16, cy + 5, 4, 2, WHITE);

    // Ojo negro
    display.drawPixel(x + 16, cy + 4, BLACK);

    // Patas cortas
    if (legToggle) {
      display.fillRect(x + 6,  cy + 10, 3, 2, WHITE);
      display.fillRect(x + 12, cy + 10, 3, 2, WHITE);
    } else {
      display.fillRect(x + 7,  cy + 10, 3, 2, WHITE);
      display.fillRect(x + 13, cy + 10, 3, 2, WHITE);
    }

    return;
  }

  // ── POSTURA NORMAL: CONSERVAR EL SPRITE EXISTENTE ───────────
  display.drawBitmap(
    x, y,
    dinoCuerpo,
    20, 16,
    WHITE
  );

  display.drawPixel(x + 12, y + 3, BLACK);

  // Patas durante el salto
  if (!onGround) {
  // Salto: solo dos patas recogidas
  display.fillRect(x + 4,  y + 16, 3, 2, WHITE);
  display.fillRect(x + 12, y + 16, 3, 2, WHITE);
  } 
  else if (legToggle) {
    display.fillRect(x + 5,  y + 15, 3, 5, WHITE);
    display.fillRect(x + 12, y + 15, 3, 5, WHITE);
  }
  else {
    display.fillRect(x + 6,  y + 15, 3, 4, WHITE);
    display.fillRect(x + 11, y + 15, 3, 5, WHITE);
  }
}

void DibujarCactus(int cx, uint8_t ch) {
  display.fillRect(cx+1, DINO_GROUND_Y-ch, 3, ch, WHITE);
  if (ch >= 9)  display.fillRect(cx-1, DINO_GROUND_Y-ch+2, 3, 3, WHITE);
  if (ch >= 11) display.fillRect(cx+4, DINO_GROUND_Y-ch+4, 3, 3, WHITE);
}

unsigned int GenerarIntervaloSpawn() {
  int r = (int)(1400.0f / dinoSpeed) + random(-200, 201);
  return (unsigned int)constrain(r, 500, 2500);
}

void IniciarDino() {
  dinoCrouchAnterior = false;
  dinoCrouching = false;
  dinoY = DINO_STAND_Y; dinoVY = 0; dinoOnGround = true;
  dinoScore = 0; dinoSpeed = DINO_SPEED_INIT;
  dinoGameOver = false; dinoGameOverDraw = false;
  dinoLastFrame = millis(); dinoLegToggle = false;
  dinoLastLegAnim = millis(); dinoJumpHeld = false;
  for (int i = 0; i < DINO_OBS_COUNT; i++) dinoObs[i].active = false;
  randomSeed(analogRead(InputA1));
  dinoLastSpawn = millis(); dinoNextSpawnMs = 1600;
  dinoPrevB1 = botonSt1; dinoPrevB2 = botonSt2;
}

void DibujarEscenaDino() {
  display.clearDisplay();
  display.setTextSize(1); display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(F("Pts:")); display.print(dinoScore / 4);
  display.setCursor(74, 0); display.print(F("B1:saltar"));
  display.drawFastHLine(0, 7, SCREEN_WIDTH, WHITE);
  display.drawFastHLine(0, DINO_GROUND_Y, SCREEN_WIDTH, WHITE);

  static float sueloOffset = 0;
  sueloOffset += dinoSpeed;
  if (sueloOffset > 12) sueloOffset -= 12;
  for (int x = -(int)sueloOffset; x < SCREEN_WIDTH; x += 12)
    if (x >= 0) display.drawFastHLine(x, DINO_GROUND_Y+2, 5, WHITE);

  static float nubeX = 88;
  nubeX -= dinoSpeed * 0.3f;
  if (nubeX < -18) nubeX = SCREEN_WIDTH + 5;
  int nx = (int)nubeX;
  display.fillRect(nx+3, 12, 9, 4, WHITE);
  display.fillRect(nx,   14, 15, 3, WHITE);

  DibujarDinoSprite(DINO_X, (int)dinoY, dinoLegToggle, dinoOnGround);
  // ── Dibujar cactus y pterodáctilos ───────────────────────────
  for (int i = 0; i < DINO_OBS_COUNT; i++) {

    if (!dinoObs[i].active) continue;

    if (dinoObs[i].esPtero) {

      // Cambiar las alas cada 140 ms.
      // El índice permite variar ligeramente la fase entre obstáculos.
      bool alasArriba = ((millis() / 140UL + i) % 2) == 0;

      DibujarPterodactilo(
        (int)dinoObs[i].x,
        dinoObs[i].y,
        alasArriba
      );

    } else {

      DibujarCactus(
        (int)dinoObs[i].x,
        dinoObs[i].h
      );
    }
  }
  display.display();
}

void DibujarGameOverDino() {
  display.clearDisplay();
  display.setTextSize(2); display.setTextColor(WHITE);
  display.setCursor(14, 4);  display.print(F("GAME"));
  display.setCursor(14, 22); display.print(F("OVER"));
  display.setTextSize(1);
  display.setCursor(72, 8);  display.print(F("Pts:")); display.print(dinoScore / 4);
  display.setCursor(0, 46);  display.print(F("B2: reiniciar"));
  display.setCursor(0, 56);  display.print(F("B1: volver menu"));
  display.display();
}

void ActualizarDino() {

  // ── GAME OVER ──────────────────────────────────────────────
  if (dinoGameOver) {

    if (!dinoGameOverDraw) {
      if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
        DibujarGameOverDino();
        xSemaphoreGive(i2cMutex);
      }

      dinoGameOverDraw = true;
      dinoGameOverMs = millis();
    }

    if (botonSt1 != dinoPrevB1) {
      dinoPrevB1 = botonSt1;
      volverAlMenu();
      return;
    }

    if (botonSt2 != dinoPrevB2) {
      dinoPrevB2 = botonSt2;
      IniciarDino();
      return;
    }

    if (millis() - dinoGameOverMs >= DINO_AUTOEXT_MS) {
      volverAlMenu();
    }

    return;
  }

  unsigned long ahora = millis();

  // ── Animación de las patas ─────────────────────────────────
  unsigned long legInterval = (unsigned long)(130.0f / dinoSpeed);

  if (dinoOnGround &&
      (ahora - dinoLastLegAnim >= legInterval)) {

    dinoLegToggle = !dinoLegToggle;
    dinoLastLegAnim = ahora;
  }

  // ── Controles: B1 salta, B2 agacha ─────────────────────────
  bool jumpBtn =
      (digitalRead(ledPinB1) == LOW) ||
      (analogRead(InputA1) < 1600);

  bool crouchBtn = (digitalRead(ledPinB2) == LOW);

  // Saltar solo si está en el suelo y no se está agachando.
  if (jumpBtn &&
      !dinoJumpHeld &&
      dinoOnGround &&
      !crouchBtn) {

    dinoVY = DINO_JUMP_VY;
    dinoOnGround = false;
    dinoJumpHeld = true;
    SolicitarPulsoJuego(22, 180);
  }

  if (!jumpBtn) {
    dinoJumpHeld = false;
  }

  // Solo agacharse mientras está en el suelo.
  // Solo agacharse mientras está en el suelo.
  bool agachadoAhora = crouchBtn && dinoOnGround;

  // Pulso al comenzar a agacharse
  if (agachadoAhora && !dinoCrouchAnterior) {
      SolicitarPulsoJuego(18, 180);
  }

  dinoCrouchAnterior = agachadoAhora;
  dinoCrouching = agachadoAhora;

  // ── Control de los fotogramas ──────────────────────────────
  if (ahora - dinoLastFrame < DINO_FRAME_MS) {

    if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
      DibujarEscenaDino();
      xSemaphoreGive(i2cMutex);
    }

    return;
  }

  dinoLastFrame = ahora;

  // ── Física del salto: sin cambios ──────────────────────────
  if (!dinoOnGround) {
    dinoVY += DINO_GRAVITY;
    dinoY += dinoVY;

    if (dinoY >= (float)DINO_STAND_Y) {
      dinoY = DINO_STAND_Y;
      dinoVY = 0;
      dinoOnGround = true;
    }
  }

  // Actualizar la postura si acaba de aterrizar.
  dinoCrouching = crouchBtn && dinoOnGround;

  // ── Velocidad y puntuación ─────────────────────────────────
  if (dinoSpeed < DINO_SPEED_MAX) {
    dinoSpeed += 0.0009f;
  }

  dinoScore++;

  // ═══════════════════════════════════════════════════════════
  //  GENERAR CACTUS Y PTERODÁCTIL
  // ═══════════════════════════════════════════════════════════
  if (ahora - dinoLastSpawn >= dinoNextSpawnMs) {

    for (int i = 0; i < DINO_OBS_COUNT; i++) {

      if (!dinoObs[i].active) {

        dinoObs[i].x = SCREEN_WIDTH + 2;

        dinoObs[i].esPtero =
          (dinoScore / 4 >= 35) &&
          (random(0, 100) < 30);

        if (dinoObs[i].esPtero) {

          // Vuelo alto o medio.
          dinoObs[i].y =
            (random(0, 2) == 0) ? 20 : 34;

          dinoObs[i].h = 0;

        } else {

          dinoObs[i].h = (uint8_t)random(8, 15);
          dinoObs[i].y = 0;
        }

        dinoObs[i].active = true;
        break;
      }
    }

    dinoLastSpawn = ahora;
    dinoNextSpawnMs = GenerarIntervaloSpawn();
  }

  // ═══════════════════════════════════════════════════════════
  //  CAJA DE COLISIÓN DEL DINO
  // ═══════════════════════════════════════════════════════════

  int dLeft  = DINO_X + (dinoCrouching ? 2 : 5);
  int dRight = DINO_X + DINO_W - (dinoCrouching ? 2 : 4);

  int dTop;
  int dBot;

  if (dinoCrouching) {

    // Caja baja al agacharse.
    int crouchY = DINO_GROUND_Y - 12;
    dTop = crouchY + 2;
    dBot = DINO_GROUND_Y - 1;

  } else {

    // Caja de colisión normal.
    dTop = (int)dinoY + 3;
    dBot = (int)dinoY + DINO_H - 3;
  }

  // ═══════════════════════════════════════════════════════════
  //  COLISIONES: CACTUS O PTERODÁCTILO
  // ═══════════════════════════════════════════════════════════

  for (int i = 0; i < DINO_OBS_COUNT; i++) {

    if (!dinoObs[i].active) {
      continue;
    }

    // Mover cada obstáculo una sola vez por fotograma.
    dinoObs[i].x -= dinoSpeed;

    if (dinoObs[i].x < -20) {
      dinoObs[i].active = false;
      continue;
    }

    bool colision = false;

    if (dinoObs[i].esPtero) {

      // Sprite del pterodáctilo: 12 x 7 píxeles.
      int pLeft   = (int)dinoObs[i].x + 2;
      int pRight  = (int)dinoObs[i].x + 10;
      int pTop    = dinoObs[i].y + 1;
      int pBottom = dinoObs[i].y + 6;

      colision =
        (dRight > pLeft) &&
        (dLeft < pRight) &&
        (dBot > pTop) &&
        (dTop < pBottom);

    } else {

      // Cactus: conservar su detección original.
      int cLeft  = (int)dinoObs[i].x + 1;
      int cRight = (int)dinoObs[i].x + 4;
      int cTop   = DINO_GROUND_Y - dinoObs[i].h;

      colision =
        (dRight > cLeft) &&
        (dLeft < cRight) &&
        (dBot > cTop) &&
        (dTop < DINO_GROUND_Y);
    }

    if (colision) {
      SolicitarPulsoJuego(27, 180);
      dinoGameOver = true;
      dinoGameOverDraw = false;
      return;
    }
  }

  // ── Dibujar escena ─────────────────────────────────────────
  if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
    DibujarEscenaDino();
    xSemaphoreGive(i2cMutex);
  }
}
// ═══════════════════════════════════════════════════════════════
//  SNAKE
// ═══════════════════════════════════════════════════════════════

// Coloca la comida en una celda aleatoria que no esté ocupada por la serpiente
void snakeColocarComida() {
  bool libre = false;
  while (!libre) {
    snakeFoodCol = (int8_t)random(0, SNAKE_COLS);
    snakeFoodRow = (int8_t)random(0, SNAKE_ROWS);
    libre = true;
    for (int i = 0; i < snakeLen; i++) {
      if (snakeBody[i].col == snakeFoodCol && snakeBody[i].row == snakeFoodRow) {
        libre = false; break;
      }
    }
  }
}

void IniciarSnake() {
  snakeLen  = 3;
  snakeDir  = SDIR_RIGHT;
  snakeNextDir = SDIR_RIGHT;
  snakeScore   = 0;
  snakeGameOver      = false;
  snakeGameOverDraw  = false;
  snakeLastFrame     = millis();
  snakePrevB1 = botonSt1;
  snakePrevB2 = botonSt2;

  // Cuerpo inicial en el centro del tablero
  int8_t startCol = SNAKE_COLS / 2;
  int8_t startRow = SNAKE_ROWS / 2;
  snakeBody[0] = { startCol,     startRow };
  snakeBody[1] = { (int8_t)(startCol - 1), startRow };
  snakeBody[2] = { (int8_t)(startCol - 2), startRow };

  randomSeed(analogRead(InputA0));
  snakeColocarComida();
}

void DibujarEscenaSnake() {
  display.clearDisplay();
  display.setTextSize(1); display.setTextColor(WHITE);

  // Cabecera
  display.setCursor(0, 0);
  display.print(F("Snake Pts:")); display.print(snakeScore);

  // Borde del área de juego
  display.drawRect(SNAKE_ORIGIN_X - 1,
                   SNAKE_ORIGIN_Y - 1,
                   SNAKE_COLS * SNAKE_CELL + 2,
                   SNAKE_ROWS * SNAKE_CELL + 2,
                   WHITE);

  // Cuerpo de la serpiente
  for (int i = 0; i < snakeLen; i++) {
    int px = SNAKE_ORIGIN_X + snakeBody[i].col * SNAKE_CELL;
    int py = SNAKE_ORIGIN_Y + snakeBody[i].row * SNAKE_CELL;
    if (i == 0) {
      // Cabeza: cuadrado relleno con un pixel hueco para dar detalle
      display.fillRect(px, py, SNAKE_CELL, SNAKE_CELL, WHITE);
      // Pequeño ojo según dirección
      int eyeX = px + 1, eyeY = py + 1;
      switch (snakeDir) {
        case SDIR_RIGHT: eyeX = px + 2; eyeY = py + 1; break;
        case SDIR_LEFT:  eyeX = px + 1; eyeY = py + 1; break;
        case SDIR_UP:    eyeX = px + 2; eyeY = py + 1; break;
        case SDIR_DOWN:  eyeX = px + 2; eyeY = py + 2; break;
      }
      display.drawPixel(eyeX, eyeY, BLACK);
    } else {
      // Segmentos del cuerpo: rect dejando 1 px de margen para distinguirlos
      display.fillRect(px + 1, py + 1, SNAKE_CELL - 2, SNAKE_CELL - 2, WHITE);
    }
  }

  // Comida: pequeño parpadeo — se dibuja como un pequeño cruz/punto
  int fx = SNAKE_ORIGIN_X + snakeFoodCol * SNAKE_CELL;
  int fy = SNAKE_ORIGIN_Y + snakeFoodRow * SNAKE_CELL;
  display.drawRect(fx, fy, SNAKE_CELL, SNAKE_CELL, WHITE);
  display.drawPixel(fx + 1, fy + 1, WHITE);
  display.drawPixel(fx + 2, fy + 1, WHITE);
  display.drawPixel(fx + 1, fy + 2, WHITE);
  display.drawPixel(fx + 2, fy + 2, WHITE);

  display.display();
}

void DibujarGameOverSnake() {
  display.clearDisplay();
  display.setTextSize(2); display.setTextColor(WHITE);
  display.setCursor(10, 4);  display.print(F("GAME"));
  display.setCursor(10, 22); display.print(F("OVER"));
  display.setTextSize(1);
  display.setCursor(72, 8);  display.print(F("Pts:")); display.print(snakeScore);
  display.setCursor(0, 46);  display.print(F("B2: reiniciar"));
  display.setCursor(0, 56);  display.print(F("B1: volver menu"));
  display.display();
}

// Lee los dos joysticks y determina la dirección deseada.
// Joystick izquierdo: InputA2 (eje Y/vertical) e InputA3 (eje X/horizontal)
// Joystick derecho:   InputA0 (eje X/vertical) e InputA1 (eje Y/horizontal)
//
// Convención según el usuario (origen en 2048 ≈ neutro):
//   Eje X (InputA0 / InputA3): 2048→4095 = adelante (ARRIBA en pantalla)
//                               0→2048    = atrás    (ABAJO en pantalla)
//   Eje Y (InputA1 / InputA2): 2048→4095 = derecha   (RIGHT)
//                               0→2048    = izquierda (LEFT)
void snakeLeerJoystick() {
  // Joystick derecho (InputA0 = eje X vertical, InputA1 = eje Y horizontal)
  int rxVal = analogRead(InputA0);  // 0=abajo, 4095=arriba
  int ryVal = analogRead(InputA1);  // 0=izq,   4095=der

  // Joystick izquierdo (InputA2 = eje Y menú/vertical, InputA3 = eje X horizontal)
  int lxVal = analogRead(InputA3);  // 0=izq,   4095=der
  int lyVal = analogRead(InputA2);  // 0=abajo, 4095=arriba

  // Eje vertical del joystick derecho
  if (rxVal > ADC_MID + SNAKE_JOY_THRESH) {        // empujado hacia adelante → ARRIBA
    if (snakeDir != SDIR_DOWN)  snakeNextDir = SDIR_UP;
  } else if (rxVal < ADC_MID - SNAKE_JOY_THRESH) { // tirado hacia atrás → ABAJO
    if (snakeDir != SDIR_UP)    snakeNextDir = SDIR_DOWN;
  }
  // Eje horizontal del joystick derecho
  if (ryVal > ADC_MID + SNAKE_JOY_THRESH) {        // hacia la derecha
    if (snakeDir != SDIR_LEFT)  snakeNextDir = SDIR_RIGHT;
  } else if (ryVal < ADC_MID - SNAKE_JOY_THRESH) { // hacia la izquierda
    if (snakeDir != SDIR_RIGHT) snakeNextDir = SDIR_LEFT;
  }

  // Eje vertical del joystick izquierdo
  if (lyVal > ADC_MID + SNAKE_JOY_THRESH) {        // adelante → ARRIBA
    if (snakeDir != SDIR_DOWN)  snakeNextDir = SDIR_UP;
  } else if (lyVal < ADC_MID - SNAKE_JOY_THRESH) { // atrás → ABAJO
    if (snakeDir != SDIR_UP)    snakeNextDir = SDIR_DOWN;
  }
  // Eje horizontal del joystick izquierdo
  if (lxVal > ADC_MID + SNAKE_JOY_THRESH) {        // derecha
    if (snakeDir != SDIR_LEFT)  snakeNextDir = SDIR_RIGHT;
  } else if (lxVal < ADC_MID - SNAKE_JOY_THRESH) { // izquierda
    if (snakeDir != SDIR_RIGHT) snakeNextDir = SDIR_LEFT;
  }
}

void ActualizarSnake() {
  if (snakeGameOver) {
    if (!snakeGameOverDraw) {
      if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
        DibujarGameOverSnake(); xSemaphoreGive(i2cMutex);
      }
      snakeGameOverDraw = true;
      snakeGameOverMs   = millis();
    }
    if (botonSt1 != snakePrevB1) { snakePrevB1 = botonSt1; volverAlMenu();  return; }
    if (botonSt2 != snakePrevB2) { snakePrevB2 = botonSt2; IniciarSnake(); return; }
    if (millis() - snakeGameOverMs >= SNAKE_AUTOEXT_MS) volverAlMenu();
    return;
  }

  // Leer entrada de joystick en cada ciclo
  snakeLeerJoystick();

  unsigned long ahora = millis();
  if (ahora - snakeLastFrame < SNAKE_FRAME_MS) {
    // Entre frames solo redibujamos (sin mover)
    if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(5)) == pdTRUE) {
      DibujarEscenaSnake(); xSemaphoreGive(i2cMutex);
    }
    return;
  }
  snakeLastFrame = ahora;

  // Aplicar dirección pedida (sin permitir inversión de 180°)
  snakeDir = snakeNextDir;

  // Calcular nueva posición de la cabeza
  int8_t newCol = snakeBody[0].col;
  int8_t newRow = snakeBody[0].row;
  switch (snakeDir) {
    case SDIR_UP:    newRow--; break;
    case SDIR_DOWN:  newRow++; break;
    case SDIR_LEFT:  newCol--; break;
    case SDIR_RIGHT: newCol++; break;
  }

  // Colisión con paredes
  if (newCol < 0 || newCol >= SNAKE_COLS || newRow < 0 || newRow >= SNAKE_ROWS) {
    SolicitarPulsoJuego(27, 180);
    snakeGameOver = true; snakeGameOverDraw = false; return;
  }

  // Colisión con el propio cuerpo (excluir la cola que se moverá)
  for (int i = 0; i < snakeLen - 1; i++) {
    if (snakeBody[i].col == newCol && snakeBody[i].row == newRow) {
      SolicitarPulsoJuego(27, 180);
      snakeGameOver = true; snakeGameOverDraw = false; return;
    }
  }

  bool comio = (newCol == snakeFoodCol && newRow == snakeFoodRow);

  if (comio) {
      // Crecer: desplazar todo el cuerpo
      if (snakeLen < SNAKE_MAX_LEN) {
          for (int i = snakeLen; i > 0; i--) {
              snakeBody[i] = snakeBody[i - 1];
          }

          snakeBody[0] = { newCol, newRow };
          snakeLen++;

          // Pulso de vibración al crecer
          if (vibracionActiva) {
              SolicitarPulsoJuego(27, 180);
          }
      }

      snakeScore++;
      snakeColocarComida();

  } else {
      // Avanzar normalmente
      for (int i = snakeLen - 1; i > 0; i--) {
          snakeBody[i] = snakeBody[i - 1];
      }

      snakeBody[0] = { newCol, newRow };
  }
}

//========================SI=======================================
// ═══════════════════════════════════════════════════════════════
//  SPACE INVADERS
// ═══════════════════════════════════════════════════════════════

void InvDibujarBanner() {
  if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
    display.clearDisplay();
    display.setTextSize(2); display.setTextColor(WHITE);
    display.setCursor(16, 14); display.print(F("LEVEL ")); display.print(invLevel);
    display.setTextSize(1);
    display.setCursor(28, 40); display.print(F("Pts: ")); display.print(invScore);
    display.display();
    xSemaphoreGive(i2cMutex);
  }
}

void InvCargarNivel() {
  for (int r = 0; r < INV_ROWS; r++)
    for (int c = 0; c < INV_COLS; c++) invAlive[r][c] = true;
  invAliveCount = INV_ROWS * INV_COLS;

  invFleetX = 4;
  invFleetY = 10 + constrain((int)invLevel - 1, 0, 4) * 2;  // empieza más abajo en niveles altos
  invDir = 1;
  invAnimFrame = false;
  invShipX = (SCREEN_WIDTH - INV_SHIP_W) / 2;

  for (int i = 0; i < INV_MAX_BULLETS; i++) invBullets[i].active = false;

  invBanner = true;
  invBannerMs = millis();
  invLastFrame = invLastFleet = millis();
  InvDibujarBanner();
}

void InvMoverNave(int8_t dir) {
  invShipX += dir * INV_SHIP_STEP;
  if (invShipX < 0) invShipX = 0;
  if (invShipX > SCREEN_WIDTH - INV_SHIP_W) invShipX = SCREEN_WIDTH - INV_SHIP_W;
}

// Tamaño 1 = punto, 2 = cuadrado 2x2, 3 = cruz. Más grande = más rápida.
void InvIniciarEstrellas() {
  randomSeed(analogRead(InputA0));
  for (int i = 0; i < INV_STARS; i++) {
    invStars[i].x   = random(2, SCREEN_WIDTH - 2);
    invStars[i].y10 = random(100, 620);
    invStars[i].size = (i < 14) ? 1 : (i < 20) ? 2 : 3;
  }
}

void InvMoverEstrellas() {
  for (int i = 0; i < INV_STARS; i++) {
    // velocidad en décimas de píxel por frame: 2, 4 o 7
    int vel = (invStars[i].size == 1) ? 2 : (invStars[i].size == 2) ? 4 : 7;
    invStars[i].y10 += vel;
    if (invStars[i].y10 > 620) {            // sale por abajo: reaparece arriba
      invStars[i].y10 = 100;
      invStars[i].x = random(2, SCREEN_WIDTH - 2);
    }
  }
}

void InvDibujarEstrellas() {
  for (int i = 0; i < INV_STARS; i++) {
    int x = invStars[i].x;
    int y = invStars[i].y10 / 10;
    switch (invStars[i].size) {
      case 1:
        display.drawPixel(x, y, WHITE);
        break;
      case 2:
        display.fillRect(x, y, 2, 2, WHITE);
        break;
      default:
        display.drawPixel(x,     y,     WHITE);
        display.drawPixel(x - 1, y,     WHITE);
        display.drawPixel(x + 1, y,     WHITE);
        display.drawPixel(x,     y - 1, WHITE);
        display.drawPixel(x,     y + 1, WHITE);
        break;
    }
  }
}

void IniciarInvaders() {
  invScore = 0;
  invLevel = 1;
  invGameOver = false;
  invGameOverDraw = false;
  invGoArmed = false;
  invJoyDir = 2;            // exige pasar por el centro antes del primer movimiento
  invFirePrev = true;       // exige soltar el botón antes del primer disparo
  invLastFire = 0;
  invExitHoldMs = 0;
  InvIniciarEstrellas();
  InvCargarNivel();
}

// Sale del juego: restaura Aux3, espera a que se suelten los botones
// (evita que una pulsación sostenida active una opción del menú) y vuelve.
void InvSalir() {
  unsigned long t0 = millis();
  while ((digitalRead(ledPinB1) == LOW || digitalRead(ledPinB2) == LOW) &&
         millis() - t0 < 2000) {
    delay(10);
  }
  volverAlMenu();
}

unsigned long InvIntervaloFlota() {
  long t = 70 + (long)invAliveCount * 14 - (long)(invLevel - 1) * 25;
  return (unsigned long)constrain(t, 40L, 500L);
}

void InvMoverFlota() {
  int minC = INV_COLS, maxC = -1, maxR = -1;
  for (int r = 0; r < INV_ROWS; r++)
    for (int c = 0; c < INV_COLS; c++)
      if (invAlive[r][c]) {
        if (c < minC) minC = c;
        if (c > maxC) maxC = c;
        if (r > maxR) maxR = r;
      }
  if (maxC < 0) return;

  const int step = 2;
  int left  = invFleetX + minC * INV_PITCH_X;
  int right = invFleetX + maxC * INV_PITCH_X + INV_ALIEN_W - 1;

  invAnimFrame = !invAnimFrame;

  if ((invDir > 0 && right + step > SCREEN_WIDTH - 1) ||
      (invDir < 0 && left - step < 0)) {
    invDir = -invDir;
    invFleetY += 3;                       // bajan al tocar el borde
  } else {
    invFleetX += invDir * step;
  }

  int bottom = invFleetY + maxR * INV_PITCH_Y + INV_ALIEN_H;
  if (bottom >= INV_SHIP_Y) {             // llegaron a la nave
    invGameOver = true;
    invGameOverDraw = false;
  }
}

void InvDisparar() {
  for (int i = 0; i < INV_MAX_BULLETS; i++) {
    if (!invBullets[i].active) {
      invBullets[i].x = invShipX + INV_SHIP_W / 2 - 1;
      invBullets[i].y = INV_SHIP_Y - INV_BULLET_H;
      invBullets[i].active = true;
      SolicitarPulsoJuego(20, 90);
      return;
    }
  }
}

void InvMoverBalas() {
  for (int i = 0; i < INV_MAX_BULLETS; i++) {
    InvBullet &b = invBullets[i];
    if (!b.active) continue;
    b.y -= INV_BULLET_STEP;

    for (int r = 0; r < INV_ROWS && b.active; r++) {
      for (int c = 0; c < INV_COLS && b.active; c++) {
        if (!invAlive[r][c]) continue;
        int ax = invFleetX + c * INV_PITCH_X;
        int ay = invFleetY + r * INV_PITCH_Y;
        if (b.x < ax + INV_ALIEN_W && b.x + INV_BULLET_W > ax &&
            b.y < ay + INV_ALIEN_H && b.y + INV_BULLET_H > ay) {
          invAlive[r][c] = false;
          invAliveCount--;
          invScore++;
          SolicitarPulsoJuego(27, 180);
          b.active = false;
        }
      }
    }
    if (b.active && b.y < 9) b.active = false;   // salió por arriba
  }
}

void InvDibujarEscena() {
  display.clearDisplay();
  display.setTextSize(1); display.setTextColor(WHITE);
  display.setCursor(0, 0);  display.print(invScore);
  display.setCursor(92, 0); display.print(F("LVL ")); display.print(invLevel);
  display.drawFastHLine(0, 8, SCREEN_WIDTH, WHITE);

  InvDibujarEstrellas();                       // ← NUEVO: fondo

  const unsigned char* spr = invAnimFrame ? inv_alien_b : inv_alien_a;
  for (int r = 0; r < INV_ROWS; r++)
    for (int c = 0; c < INV_COLS; c++)
      if (invAlive[r][c])
        display.drawBitmap(invFleetX + c * INV_PITCH_X,
                           invFleetY + r * INV_PITCH_Y,
                           spr, INV_ALIEN_W, INV_ALIEN_H, WHITE);

  display.drawBitmap(invShipX, INV_SHIP_Y, inv_ship, INV_SHIP_W, INV_SHIP_H, WHITE);

  for (int i = 0; i < INV_MAX_BULLETS; i++)
    if (invBullets[i].active)
      display.fillRect(invBullets[i].x, invBullets[i].y, INV_BULLET_W, INV_BULLET_H, WHITE);

  display.display();
}

void InvDibujarGameOver() {
  display.clearDisplay();
  display.setTextSize(2); display.setTextColor(WHITE);
  display.setCursor(10, 4); display.print(F("GAME OVER"));
  display.setTextSize(1);
  display.setCursor(0, 28); display.print(F("Pts:")); display.print(invScore);
  display.print(F("  Nivel:")); display.print(invLevel);
  display.setCursor(0, 46); display.print(F("B2: reiniciar"));
  display.setCursor(0, 56); display.print(F("B1: volver menu"));
  display.display();
}

void ActualizarInvaders() {
  bool b1 = (digitalRead(ledPinB1) == LOW);
  bool b2 = (digitalRead(ledPinB2) == LOW);
  unsigned long ahora = millis();

  // ── Game Over ──────────────────────────────────────────────
  if (invGameOver) {
    if (!invGameOverDraw) {
      if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
        InvDibujarGameOver(); xSemaphoreGive(i2cMutex);
      }
      invGameOverDraw = true;
      invGameOverMs = ahora;
      invGoArmed = false;
    }
    if (!invGoArmed) {
      if (!b1 && !b2) invGoArmed = true;
    } else if (b1) { InvSalir();        return; }
    else if (b2)   { IniciarInvaders(); return; }

    if (ahora - invGameOverMs >= INV_AUTOEXT_MS) InvSalir();
    return;
  }

  // ── Salir manteniendo B1+B2 ────────────────────────────────
  if (b1 && b2) {
    if (invExitHoldMs == 0) invExitHoldMs = ahora;
    else if (ahora - invExitHoldMs >= INV_EXIT_HOLD_MS) { InvSalir(); return; }
  } else {
    invExitHoldMs = 0;
  }

  // ── Pantalla "LEVEL n" ─────────────────────────────────────
  if (invBanner) {
    if (ahora - invBannerMs >= INV_BANNER_MS) {
      invBanner = false;
      invLastFrame = invLastFleet = ahora;
    }
    return;
  }

  // ── Movimiento: toque = 1 paso, sostenido = continuo ───────
  // Se usan los dos joysticks: manda el que esté más lejos del centro
  int joyA = analogRead(INV_JOY_PIN);
  int joyB = analogRead(INV_JOY_PIN2);
  int joy  = (abs(joyA - ADC_MID) >= abs(joyB - ADC_MID)) ? joyA : joyB;

  if (invJoyDir == 2) {                         // bloqueado: espera el centro
    if (joy >= INV_JOY_NEUT_L && joy <= INV_JOY_NEUT_H) invJoyDir = 0;
  } else {
    int8_t dir = 0;
    if (invJoyDir == 0) {                       // desde el centro: umbral de empujón
      if      (joy > INV_JOY_RIGHT) dir = 1;
      else if (joy < INV_JOY_LEFT)  dir = -1;
    } else if (invJoyDir == 1) {                // ya a la derecha: se mantiene hasta volver al centro
      dir = (joy > INV_JOY_NEUT_H) ? 1 : 0;
    } else {                                    // ya a la izquierda
      dir = (joy < INV_JOY_NEUT_L) ? -1 : 0;
    }

    if (dir == 0) {
      invJoyDir = 0;
    } else if (dir != invJoyDir) {              // empujón nuevo → un paso inmediato
      invJoyDir = dir;
      invJoyPushMs = invJoyStepMs = ahora;
      InvMoverNave(dir);
    } else if (ahora - invJoyPushMs >= INV_JOY_HOLD_MS &&
               ahora - invJoyStepMs >= INV_JOY_REPEAT_MS) {
      invJoyStepMs = ahora;                     // sostenido → movimiento continuo
      InvMoverNave(dir);
    }
  }

  // ── Disparo: una bala por pulsación + automático si se mantiene ──
  // Si están B1 y B2 a la vez es la salida y no dispara.
  bool fuego = (b1 != b2);
  if (fuego) {
    if (!invFirePrev) {                         // pulsación nueva
      if (ahora - invLastFire >= INV_FIRE_MIN_MS) {
        invLastFire = ahora;
        InvDisparar();
      }
    } else if (ahora - invLastFire >= INV_FIRE_MS) {   // botón mantenido
      invLastFire = ahora;
      InvDisparar();
    }
  }
  invFirePrev = fuego;

  // ── Frame del juego ────────────────────────────────────────
  if (ahora - invLastFrame < INV_FRAME_MS) return;
  invLastFrame = ahora;

  InvMoverEstrellas();
  InvMoverBalas();

  if (ahora - invLastFleet >= InvIntervaloFlota()) {
    invLastFleet = ahora;
    InvMoverFlota();
    if (invGameOver) return;
  }

  if (invAliveCount == 0) {          // nivel superado
    invLevel++;
    InvCargarNivel();
    return;
  }

  if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
    InvDibujarEscena(); xSemaphoreGive(i2cMutex);
  }
}
//====================SI==========================================

// ═══════════════════════════════════════════════════════════════
//  volverAlMenu
// ═══════════════════════════════════════════════════════════════
void volverAlMenu() {
  dinoActive  = false;
  snakeActive = false;
  invadersActive = false;
  raceActive     = false;
  doomActive = false;
  menuActive  = true;
  botonSt1    = false;
  botonSt2    = false;
  if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
    display.clearDisplay(); display.display();
    xSemaphoreGive(i2cMutex);
  }
}

// ═══════════════════════════════════════════════════════════════
//  LOOP PRINCIPAL (Core 1)
// ═══════════════════════════════════════════════════════════════
void loop() {

  if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
    ProtocoloMPUData();
    xSemaphoreGive(i2cMutex);
  }

  rotationAngle = calcularAngulo(accelY, accelX, accelZ);

  BotonesI();
  leerBotonPB3();

  // ── Juegos activos ────────────────────────────────────────
  if (dinoActive) {
    ActualizarDino();
    ActualizarVibrador();
    return;
  }

  if (snakeActive) {
    ActualizarSnake();
    ActualizarVibrador();
    return;
  }

  if (invadersActive) {
    ActualizarInvaders();
    ActualizarVibrador();
    return;
  }

  if (raceActive) {
    ActualizarRoadRush();
    ActualizarVibrador();
    return;
  }

  if (doomActive) {
    ActualizarDoom();
    ActualizarVibrador();
    return;
  }

  // ── Funcionamiento normal y menú ──────────────────────────
  EnviarDatos();

  ActualizarVibrador();
}
//====================PB3=========================================
void leerBotonPB3() {
  bool botonPB3 = (digitalRead(botonPinPB3) == LOW);

  if (botonPB3 && !presionadoPB3) {
    presionadoPB3 = true;
    tiempoInicioPB3 = millis();
    yaCambioPB3 = false;
  }

  if (presionadoPB3 && botonPB3 && !yaCambioPB3 &&
      millis() - tiempoInicioPB3 >= tiempoPresionadoPB3) {
    estadoPB3 = !estadoPB3;
    Serial.println(estadoPB3);
    yaCambioPB3 = true;
  }

  if (!botonPB3) {
    presionadoPB3 = false;
  }
}
//======================PB3=======================================
// ═══════════════════════════════════════════════════════════════
//  EnviarDatos
// ═══════════════════════════════════════════════════════════════
void EnviarDatos() {
  if (mapthrottle == false) {
    // Modo normal: joystick centrado, solo se usa mitad del recorrido
    // Centro físico (2048) = 0 throttle, tope (4095) = full throttle
    int rawThr = constrain(analogRead(InputA0), ADC_THR_LOW, ADC_MAX);
    data.throttle = (uint8_t)map(rawThr, ADC_THR_LOW, ADC_MAX, 0, 255);
    if (throttleReverse) data.throttle = 255 - data.throttle;
  } else {
    // Modo mapthrottle: usa todo el recorrido 0→4095
    int rawThr = constrain(analogRead(InputA0), 0, ADC_MAX);
    data.throttle = (uint8_t)map(rawThr, 0, ADC_MAX, 0, 255);
    if (throttleReverse) data.throttle = 255 - data.throttle;
  }

  data.pitch = MapearJoysticks(analogRead(InputA1), 0, ADC_MID, ADC_MAX, pitchReverse);
  data.yaw   = MapearJoysticks(analogRead(InputA2), 0, ADC_MID, ADC_MAX, yawReverse);
  data.roll  = MapearJoysticks(analogRead(InputA3), 0, ADC_MID, ADC_MAX, rollReverse);
  data.aux1  = MapearJoysticks(analogRead(InputA6), 0, ADC_MID, ADC_MAX, aux1Reverse);
  data.aux2  = digitalRead(swicth1);
  //data.aux3  = digitalRead(estadoPB3);
  //data.aux3  = estadoPB3;

  if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
    if (!throttleReverse)
      dispData.throttlePercent = map(data.throttle, 0, 255, 0, 100);
    else
      dispData.throttlePercent = map(data.throttle, 0, 255, 100, 0);

    dispData.rotation         = data.rotation;
    dispData.roll             = data.roll;
    dispData.aux3             = estadoPB3;
    dispData.aux2             = data.aux2;
    dispData.aux1             = data.aux1;
    dispData.throttleUpArrows = (data.throttle >= 25);
    dispData.showMenuTriangle = mensajeImpreso;

    static unsigned long batUltimaLectura = 0;
    unsigned long ahora = millis();
    if (dispData.batteryVoltage == 0.0f || (ahora - batUltimaLectura >= BAT_REFRESH_MS)) {
      batUltimaLectura = ahora;
      long suma = 0;
      for (uint8_t s = 0; s < 8; s++) suma += analogRead(InputBattery);
      float promedio = suma / 8.0f;
      float Vpin = promedio * (3.36f / 4095.0f);   // 3.1V = fondo de escala real ADC_11db
      dispData.batteryVoltage = Vpin * 3.1004f;    // factor calibrado con tus resistencias reales
      dispData.batteryPercent = constrain(((dispData.batteryVoltage - BAT_MIN_V) /
                                            (BAT_MAX_V   - BAT_MIN_V)) * 100.0f, 0.0f, 100.0f);
  }
    xSemaphoreGive(i2cMutex);
  }

  if (menuActive) {
    VentanaMenu();
  } else {
    radio.write(&data, sizeof(ControlSignal));
  }

  HabilitarOled = (digitalRead(swicth2) == LOW);
}

// ═══════════════════════════════════════════════════════════════
//  BotonesI — Control de botones, indicador y entrada al menú
// ═══════════════════════════════════════════════════════════════

void BotonesI() {

  if (invadersActive || raceActive || doomActive) return;   //

  // ── Durante los juegos, gestionar botones en Game Over ─────
  if (dinoActive || snakeActive) {

    if (dinoActive && !dinoGameOver) return;
    if (snakeActive && !snakeGameOver) return;

    unsigned long t = millis();

    bool p1 = digitalRead(ledPinB1) == LOW;
    if (p1 && (t - botonT1 >= BanderaTBoton)) {
      botonT1 = t;
      botonSt1 = !botonSt1;
    }

    bool p2 = digitalRead(ledPinB2) == LOW;
    if (p2 && (t - botonT2 >= BanderaTBoton)) {
      botonT2 = t;
      botonSt2 = !botonSt2;
    }

    return;
  }

  // ── Leer botones B1 y B2 ───────────────────────────────────
  bool botonP1 = digitalRead(ledPinB1) == LOW;
  unsigned long T1 = millis();

  if (botonP1 && (T1 - botonT1 >= BanderaTBoton)) {
    botonT1 = T1;
    botonSt1 = !botonSt1;
  }

  bool botonP2 = digitalRead(ledPinB2) == LOW;
  unsigned long T2 = millis();

  if (botonP2 && (T2 - botonT2 >= BanderaTBoton)) {
    botonT2 = T2;
    botonSt2 = !botonSt2;
  }

  // ── Actualizar rotación ────────────────────────────────────
  if (botonSt2) {
    data.rotation = map(rotationAngle, -90, 90, 0, 190);
  } else {
    data.rotation = map(rotationAngle, -90, 90, 190, 0);
  }

  // ── CORRECCIÓN: ocultar el indicador al vencer el tiempo ───
  // Se comprueba en cada llamada, aunque no se pulse un botón.
  if (!menuActive &&
      MenuBanderaOn &&
      (millis() - tiempoUltimaImpresion >= tiempoEspera)) {

    MenuBanderaOn = false;
    mensajeImpreso = false;
  }

  // ── Detectar pulsación para mostrar o entrar al menú ───────
  bool cualquierBoton = (botonSt1 || botonSt2);

  if (cualquierBoton && !menuActive) {

    if (!MenuBanderaOn) {

      // Primera pulsación: mostrar ▽ y comenzar los 5 segundos.
      mensajeImpreso = true;
      MenuBanderaOn = true;
      tiempoUltimaImpresion = millis();

    } else {

      // Segunda pulsación dentro del plazo: entrar al menú.
      menuActive = true;

      MenuBanderaOn = false;
      mensajeImpreso = false;
    }

    // Consumir la pulsación para evitar reutilizarla.
    botonSt1 = false;
    botonSt2 = false;
  }
}

// ═══════════════════════════════════════════════════════════════
//  SETUP
// ═══════════════════════════════════════════════════════════════
void setup() {
  radio.begin();
  radio.openWritingPipe(PIPE_OUT);
  radio.setAutoAck(false);
  radio.stopListening();

  EEPROM.begin(16);
  analogSetAttenuation(ADC_11db);
  analogSetWidth(12);

  Wire.begin(21, 22);
  IniciarMPU();
  CargarDatos();
  AplicarConfigRF();

  pinMode(swicth1,  INPUT);
  pinMode(swicth2,  INPUT);
  pinMode(ledPinB1, INPUT_PULLUP);
  pinMode(ledPinB2, INPUT_PULLUP);
  pinMode(botonPinPB3, INPUT_PULLUP);

  // ── Vibración de encendido ─────────────────────────────────
  pinMode(PIN_MOTOR, OUTPUT);
  analogWriteFrequency(PIN_MOTOR, FRECUENCIA_PWM);
  analogWrite(PIN_MOTOR, 0);

  if (vibracionActiva) {

    int pwmArranque = map(
      VIBRACION_MAXIMA,
      0, 100,
      0, 255
    );

    analogWrite(PIN_MOTOR, pwmArranque);
    delay(VIBRACION_ARRANQUE_MS);

    analogWrite(PIN_MOTOR, 0);
 }

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1); display.setTextColor(WHITE);
  display.drawBitmap((display.width() - LOGO_WIDTH) / 2, 0,
                     logo1, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();

  for (int j = 0; j < 8; j++) {
    display.setTextSize(1); display.setTextColor(WHITE);
    display.setCursor(0, 0);  display.print(F("by:BMC"));
    display.setCursor(30, 57); display.print(F("INICIANDO"));
    for (int i = 0; i < 3; i++) {
      display.print('.'); display.display(); delay(200);
    }
    display.clearDisplay();
    display.drawBitmap((display.width() - LOGO_WIDTH) / 2, 0,
                       logo1, LOGO_WIDTH, LOGO_HEIGHT, 1);
  }
  //delay(4000);
  display.clearDisplay();
  display.display();

  i2cMutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(
    oledTask, "OLED_Task", 4096, NULL, 1, NULL, 0
  );
}

void SolicitarPulsoJuego(uint8_t potencia, unsigned long duracion) {

  if (!vibracionActiva) return;

  potencia = constrain(potencia, 0, 100);

  // Un pulso más intenso puede reemplazar uno más débil.
  if (!pulsoJuegoActivo || potencia >= pulsoJuegoPotencia) {

    pulsoJuegoActivo = true;
    pulsoJuegoPotencia = potencia;
    pulsoJuegoDuracion = duracion;
    pulsoJuegoInicio = millis();
  }
}
// ═══════════════════════════════════════════════════════════════
//  dibujarIndicadorInclinacion (sin cambios)
// ═══════════════════════════════════════════════════════════════
void dibujarIndicadorInclinacion(int angulo) {

  const int cx = SCREEN_WIDTH / 2;
  const int cy = SCREEN_HEIGHT / 2.3;
  const int radio = 8;

  // ── Cálculo original de la inclinación: NO MODIFICADO ──────
  int a = map(angulo, -128, 127, 0, 359);
  float rad = (a * DEG_TO_RAD) - 5.4f;

  // Vector de dirección: conserva el sentido original.
  float dirX = -sin(rad);
  float dirY = -cos(rad);

  // Vector perpendicular para dar grosor a la aguja.
  float perpX = -dirY;
  float perpY =  dirX;

  // ── 1. Círculo exterior ────────────────────────────────────
  display.drawCircle(cx, cy, radio, SSD1306_WHITE);

  // ── 2. Cruz perpendicular punteada ─────────────────────────
  for (int d = -radio + 2; d <= radio - 2; d++) {

    if ((abs(d) % 3) != 2) {
      display.drawPixel(cx + d, cy, SSD1306_WHITE);
      display.drawPixel(cx, cy + d, SSD1306_WHITE);
    }
  }

  // ── 3. Calcular base y punta de la flecha ──────────────────
  // La base queda dentro del círculo.
  // La punta sobresale 3 píxeles respecto al radio exterior.
  const float radioBase = radio - 1.0f;
  const float radioPunta = radio + 3.0f;
  const float medioAncho = 2.5f;

  int bx = round(cx + dirX * radioBase);
  int by = round(cy + dirY * radioBase);

  int tx = round(cx + dirX * radioPunta);
  int ty = round(cy + dirY * radioPunta);

  // Bordes laterales de la aguja para conseguir un grosor
  // aproximado de 3 píxeles.
  int ox = round(perpX);
  int oy = round(perpY);

  // Línea de la flecha más delgada: 1 píxel
  display.drawLine(cx, cy, bx, by, SSD1306_WHITE);

  // ── 4. Punta triangular de la flecha ───────────────────────
  int hx1 = round(cx + dirX * radioBase
                     + perpX * medioAncho);
  int hy1 = round(cy + dirY * radioBase
                     + perpY * medioAncho);

  int hx2 = round(cx + dirX * radioBase
                     - perpX * medioAncho);
  int hy2 = round(cy + dirY * radioBase
                     - perpY * medioAncho);

  display.fillTriangle(
    tx, ty,       // Punta que sobresale del círculo
    hx1, hy1,     // Base lateral 1
    hx2, hy2,     // Base lateral 2
    SSD1306_WHITE
  );

  // ── 5. Punto central pequeño ───────────────────────────────
  display.fillCircle(cx, cy, 1, SSD1306_WHITE);
}
