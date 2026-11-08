#pragma once
/*
  ============================================================
   RoadRush.h  -  Juego "Road Rush" para el control RC (OLED 128x64)
  ============================================================
  Se agrega como una pestana nueva (RoadRush.h) en el mismo sketch y se
  incluye con  #include "RoadRush.h"  (ver instrucciones de integracion).

  Usa de tu codigo principal (sin modificarlos):
    display, i2cMutex, ledPinB1, ledPinB2, InputA3, ADC_MID, ADC_MAX,
    rollReverse, rotationAngle, MapearJoysticks(), SolicitarPulsoJuego(),
    volverAlMenu()  y la bandera  raceActive.

  CONTROLES
    - Joystick DIR (InputA3, el mismo de dispData.roll) y/o inclinacion del
      control (giroscopio, rotationAngle).
    - B1+B2 mantenidos 1.5 s durante la partida = salir al menu.
    - Game Over: B2 reinicia, B1 vuelve al menu (o solo vuelve a los 5 s).
*/

// ------------------------- OPCIONES -------------------------
// 0 = joystick DIR + inclinacion (manda el que este mas desviado)
// 1 = solo joystick DIR
// 2 = solo inclinacion (giroscopio)
#define RR_CONTROL       0

// Si al inclinar a la derecha el auto va a la izquierda, cambia a 1
#define RR_TILT_INVERT   0

// 0 = linea central unica, 1 = linea central doble
#define RR_LINEA_DOBLE   0

namespace RR {

// ------------------------- PANTALLA / TIEMPOS -------------------------
constexpr int      SCR_W        = 128;
constexpr int      SCR_H        = 64;
constexpr uint32_t FRAME_MS     = 30;      // ~33 FPS
constexpr uint32_t AUTOEXT_MS   = 5000UL;  // Game Over -> menu automatico
constexpr uint32_t EXIT_HOLD_MS = 1500UL;  // B1+B2 para salir

// ------------------------- ENTRADAS -------------------------
constexpr float JOY_DEAD  = 18.0f;   // zona muerta del joystick (0..255, centro 127.5)
                                     // (109..140 = igual que los triangulos de DIR)
constexpr float TILT_DEAD = 2.0f;    // grados sin efecto
constexpr float TILT_FULL = 35.0f;   // grados para giro maximo

// ------------------------- GEOMETRIA DE LA PISTA -------------------------
constexpr int   HORIZON   = 26;
constexpr int   ROWS      = SCR_H - HORIZON;   // 38
constexpr float ZK        = 37.0f;
constexpr float HW_BOTTOM = 62.0f;
constexpr float NODE_LEN  = 5.0f;
constexpr float LOOK      = 0.30f;
constexpr float CURVE_K   = 0.08f;
constexpr float CURB_P    = 0.40f;
constexpr float DASH_P    = 0.40f;
constexpr float POST_SP   = 1.2f;

// ------------------------- JUEGO -------------------------
constexpr int   PLAYER_BASE_Y = 60;
constexpr float STEER_SPEED   = 1.7f;
constexpr float DRIFT         = 0.95f;
constexpr float OFFROAD_LIMIT = 1.08f;
constexpr float SPD_MIN       = 1.6f;
constexpr float SPD_MAX       = 3.8f;
constexpr float LEVEL_DIST    = 60.0f;
constexpr float MTN_K         = 4.0f;
constexpr float ENEMY_W       = 0.48f;
constexpr float ZSPAWN        = 18.0f;
constexpr float ZHIT_FAR      = 1.6f;
constexpr float ZHIT_NEAR     = 0.9f;
static const float LANE_X[3]  = { -0.62f, 0.0f, 0.62f };

// ------------------------- SPRITES -------------------------
// '#' = blanco, '0' = negro (se dibuja), '.' = transparente
constexpr int SPR_W = 29;

static const char* const CAR_STRAIGHT[] = {
  "........#############",
  "........#00000000000#",
  "..#######00000000000#######",
  ".###########################",
  ".###########################",
  ".##0000000#0000000#0000000##",
  ".##00###00#########00###00##",
  ".##0000000#########0000000##",
  ".###########################",
  ".###########################",
  ".######000000000000000######",
  ".######000000000000000######",
  ".######000#########000######",
  ".######000000000000000######"
};
constexpr int CAR_STRAIGHT_H = 14;

// Girando a la izquierda (a la derecha se dibuja en espejo)
static const char* const CAR_TURN[] = {
  ".........##############",
  ".......##0000#0000000000#",
  "....####0000#000000000000####",
  "...##########################",
  "..###########################",
  "..########000000#0000#000000#",
  "..########00###0######00###0#",
  "..########000000######000000#",
  ".############################",
  "...######0000000000000######",
  "...##00##0000000000000######",
  "...##00##000########00######",
  "...######0000000000000######"
};
constexpr int CAR_TURN_H = 13;

static const uint8_t MTN_X[12] = {  0,  9, 15, 21, 36, 52, 60, 66, 80, 91, 107, 128 };
static const uint8_t MTN_Y[12] = { 22, 19, 20, 22, 16, 23, 24, 25, 19, 22,  15,  22 };

// ------------------------- ESTADO -------------------------
enum Fase { FASE_PLAY, FASE_CRASH, FASE_OVER };
Fase fase = FASE_PLAY;

struct Enemy { bool on; float z; float ex; float v; };
constexpr int MAX_EN = 8;
Enemy en[MAX_EN];

float dist = 0, speedU = 0, sf = 0.3f;
float px = 0, vx = 0, mx = 0;
float scoreF = 0, rotF = 0;
int   score = 0, hiScore = 0, level = 1, lastLevel = 1;
float crashT = 0, spawnT = 1.0f, levelBannerT = 0;
bool  newRecord = false;
int   steerShow = 0;
uint32_t lastFrame = 0, frameCount = 0, overMs = 0, exitHold = 0;
bool  overDrawn = false, overArmed = false;

float roadCx[ROWS];
float roadHw[ROWS];
uint8_t* fb = nullptr;

// ------------------------- UTILIDADES -------------------------
static inline int iMax(int a, int b) { return a > b ? a : b; }
static inline int iMin(int a, int b) { return a < b ? a : b; }

static inline void putPix(int x, int y, bool white) {
  if ((unsigned)x >= (unsigned)SCR_W || (unsigned)y >= (unsigned)SCR_H) return;
  uint8_t* p = fb + x + (y >> 3) * SCR_W;
  uint8_t m = 1 << (y & 7);
  if (white) *p |= m; else *p &= ~m;
}

static inline void hspan(int y, int x0, int x1, bool white) {
  if (y < 0 || y >= SCR_H) return;
  if (x0 < 0) x0 = 0;
  if (x1 > SCR_W - 1) x1 = SCR_W - 1;
  uint8_t m = 1 << (y & 7);
  uint8_t* p = fb + (y >> 3) * SCR_W;
  if (white) { for (int x = x0; x <= x1; x++) p[x] |= m; }
  else       { for (int x = x0; x <= x1; x++) p[x] &= ~m; }
}

// Giro del jugador: -1.0 (izquierda) .. +1.0 (derecha)
float leerGiro() {
  float sJoy = 0, sTilt = 0;

#if RR_CONTROL != 2
  // Mismo valor 0..255 que se muestra como "DIR" (data.roll)
  int roll = MapearJoysticks(analogRead(InputA3), 0, ADC_MID, ADC_MAX, rollReverse);
  float d  = (float)roll - 127.5f;
  float ad = fabsf(d);
  if (ad > JOY_DEAD) {
    sJoy = (ad - JOY_DEAD) / (127.5f - JOY_DEAD);
    if (d < 0) sJoy = -sJoy;
  }
#endif

#if RR_CONTROL != 1
  // rotationAngle (grados, -90..90) se actualiza en loop() antes de cada juego
  rotF += ((float)rotationAngle - rotF) * 0.35f;      // filtro anti-ruido
  float ang = rotF;
  #if RR_TILT_INVERT
  ang = -ang;
  #endif
  float aa = fabsf(ang);
  if (aa > TILT_DEAD) {
    sTilt = (aa - TILT_DEAD) / (TILT_FULL - TILT_DEAD);
    if (sTilt > 1.0f) sTilt = 1.0f;
    if (ang < 0) sTilt = -sTilt;
  }
#endif

  float s = (fabsf(sJoy) >= fabsf(sTilt)) ? sJoy : sTilt;
  if (s > 1.0f) s = 1.0f;
  if (s < -1.0f) s = -1.0f;
  return s;
}

// ------------------------- GENERADOR DE PISTA -------------------------
float nodes[64];
int   genCount = 0, patLeft = 0, patLen = 0, patPos = 0;
bool  patCurve = false;
float patSign = 1, patMag = 0;

void resetTrack() {
  genCount = 0; patLeft = 0; patPos = 0; patCurve = false;
  for (int k = 0; k < 64; k++) nodes[k] = 0;
}

void genNode() {
  if (patLeft == 0) {
    if (genCount < 4) {
      patCurve = false; patLen = patLeft = 4;
    } else if (random(100) < 40) {
      patCurve = false; patLen = patLeft = random(2, 5);
    } else {
      patCurve = true;  patLen = patLeft = random(3, 6);
      patSign = random(2) ? 1.0f : -1.0f;
      patMag  = 0.55f + random(0, 46) / 100.0f;
    }
    patPos = 0;
  }
  float v = 0;
  if (patCurve) v = patSign * patMag * sinf(PI * (patPos + 1) / (patLen + 1));
  nodes[genCount & 63] = v;
  genCount++; patPos++; patLeft--;
}

float nodeAt(int n) {
  while (genCount <= n) genNode();
  return nodes[n & 63];
}

float curveAt(float s) {
  if (s < 0) s = 0;
  int n = (int)(s / NODE_LEN);
  float t = s / NODE_LEN - n;
  float a = nodeAt(n), b = nodeAt(n + 1);
  t = t * t * (3.0f - 2.0f * t);
  return a + (b - a) * t;
}

void computeRoad() {
  float x = 0, dx = 0;
  for (int i = ROWS - 1; i >= 0; i--) {
    float s = dist + (ROWS - 1 - i) * LOOK;
    dx += curveAt(s) * CURVE_K;
    x  += dx;
    float hw = (i + 1) * HW_BOTTOM / ZK;
    roadHw[i] = hw;
    roadCx[i] = 64.0f - px * hw + x;
  }
}

// ------------------------- DIBUJO -------------------------
void drawSprite(const char* const* rows, int h, int x0, int yBottom, bool mirror) {
  int yTop = yBottom - h + 1;
  for (int r = 0; r < h; r++) {
    const char* s = rows[r];
    for (int c = 0; s[c]; c++) {
      char ch = s[c];
      if (ch == '.') continue;
      int x = mirror ? (x0 + SPR_W - 1 - c) : (x0 + c);
      display.drawPixel(x, yTop + r, ch == '#' ? SSD1306_WHITE : SSD1306_BLACK);
    }
  }
}

int mountainY(int xm) {
  for (int k = 0; k < 11; k++) {
    if (xm >= MTN_X[k] && xm < MTN_X[k + 1]) {
      int dx = MTN_X[k + 1] - MTN_X[k];
      return MTN_Y[k] + ((int)MTN_Y[k + 1] - (int)MTN_Y[k]) * (xm - MTN_X[k]) / dx;
    }
  }
  return MTN_Y[0];
}

void drawMountains() {
  int off = ((int)floorf(mx) % 128 + 128) % 128;
  int prev = mountainY(off);
  for (int x = 0; x < SCR_W; x++) {
    int ya = prev;
    int yb = mountainY((x + 1 + off) % 128);
    prev = yb;
    int lo = iMin(ya, yb), hi = iMax(ya, yb) + 1;
    for (int y = lo; y <= hi; y++) putPix(x, y, true);
  }
}

void drawTerrainAndRoad() {
  for (int y = HORIZON; y < SCR_H; y++) {
    hspan(y, 0, SCR_W - 1, true);
    if (y - HORIZON >= 2 && (y & 1) == 0) {
      int off = ((y >> 1) & 1) * 2;
      for (int x = off; x < SCR_W; x += 4) putPix(x, y, false);
    }
  }
  for (int i = 0; i < ROWS; i++) {
    int y = HORIZON + i;
    float hw = roadHw[i], cx = roadCx[i];
    int xl = (int)floorf(cx - hw + 0.5f);
    int xr = (int)floorf(cx + hw + 0.5f);
    int cw = iMax(1, (int)(hw * 0.13f));
    float z = ZK / (i + 1);
    float w = z + dist;
    bool odd = ((int)floorf(w / CURB_P)) & 1;

    putPix(xl - cw - 1, y, false);
    putPix(xr + cw + 1, y, false);
    hspan(y, xl - cw, xl - 1, true);
    hspan(y, xr + 1, xr + cw, true);
    if (odd) {
      for (int x = xl - cw; x < xl; x++)      if ((x + y) & 1) putPix(x, y, false);
      for (int x = xr + 1; x <= xr + cw; x++) if ((x + y) & 1) putPix(x, y, false);
    }
    hspan(y, xl, xr, false);

    if (i >= 8 && fmodf(w, DASH_P) < DASH_P * 0.5f) {
      int lw = iMax(1, (int)(hw * 0.06f));
#if RR_LINEA_DOBLE
      int o = iMax(lw + 1, (int)(hw * 0.12f));
      int xa = (int)(cx - o) - lw / 2, xb = (int)(cx + o) - lw / 2;
      hspan(y, xa, xa + lw - 1, true);
      hspan(y, xb, xb + lw - 1, true);
#else
      int xa = (int)cx - lw / 2;
      hspan(y, xa, xa + lw - 1, true);
#endif
    }
  }
}

void drawPosts() {
  int k0 = (int)ceilf((dist + 1.0f) / POST_SP);
  for (int k = k0; ; k++) {
    float z = k * POST_SP - dist;
    if (z > 14.0f) break;
    if (z < 1.0f) continue;
    int i = (int)(ZK / z - 1.0f + 0.5f);
    if (i < 1 || i >= ROWS) continue;
    float hw = roadHw[i];
    int pw = iMax(1, (int)(hw * 0.09f));
    int ph = iMax(2, (int)(hw * 0.20f));
    int y  = HORIZON + i;
    for (int side = -1; side <= 1; side += 2) {
      int x = (int)(roadCx[i] + side * hw * 1.45f);
      display.fillRect(x - pw / 2, y - ph + 1, pw, ph, SSD1306_BLACK);
    }
  }
}

void drawEnemyCar(int cx, int yb, int w) {
  if (w < 2) { display.drawPixel(cx, yb, SSD1306_WHITE); return; }
  int h  = iMax(2, (w * 5) / 9);
  int hb = (h + 1) / 2;
  int hc = h - hb;
  int x0 = cx - w / 2;
  if (hc > 0) display.fillRect(x0 + w / 5, yb - h + 1, w - 2 * (w / 5), hc, SSD1306_WHITE);
  display.fillRect(x0, yb - hb + 1, w, hb, SSD1306_WHITE);
  if (w >= 8 && hc >= 2)
    display.fillRect(x0 + w / 5 + 1, yb - h + 2, w - 2 * (w / 5) - 2, hc - 1, SSD1306_BLACK);
  if (w >= 10) {
    int lw = iMax(1, w / 6), lh = iMax(1, hb / 3);
    display.fillRect(x0 + 1, yb - hb + 2, lw, lh, SSD1306_BLACK);
    display.fillRect(x0 + w - 1 - lw, yb - hb + 2, lw, lh, SSD1306_BLACK);
  }
  if (w >= 14) display.fillRect(cx - w / 8, yb - hb + 2, iMax(2, w / 4), 1, SSD1306_BLACK);
}

void enemyScreen(const Enemy& e, int& sx, int& sy, int& w) {
  float fi = ZK / e.z - 1.0f;
  int i = (int)(fi + 0.5f);
  int ic = i < 0 ? 0 : (i > ROWS - 1 ? ROWS - 1 : i);
  float hw = roadHw[ic];
  sx = (int)(roadCx[ic] + e.ex * hw);
  sy = HORIZON + i;
  w  = (int)(ENEMY_W * hw);
}

void drawEnemies() {
  int idx[MAX_EN], n = 0;
  for (int k = 0; k < MAX_EN; k++) if (en[k].on) idx[n++] = k;
  for (int a = 1; a < n; a++) {
    int t = idx[a], b = a - 1;
    while (b >= 0 && en[idx[b]].z < en[t].z) { idx[b + 1] = idx[b]; b--; }
    idx[b + 1] = t;
  }
  for (int a = 0; a < n; a++) {
    int sx, sy, w;
    enemyScreen(en[idx[a]], sx, sy, w);
    drawEnemyCar(sx, sy, w);
  }
}

void drawPlayer() {
  int x0 = 64 - SPR_W / 2;
  if (steerShow == 0) drawSprite(CAR_STRAIGHT, CAR_STRAIGHT_H, x0, PLAYER_BASE_Y, false);
  else                drawSprite(CAR_TURN, CAR_TURN_H, x0, PLAYER_BASE_Y, steerShow > 0);
  if (fase == FASE_PLAY && sf > 0.2f) {
    int o = (frameCount & 1) ? 0 : 2;
    display.drawFastHLine(x0 - 7, PLAYER_BASE_Y - 1 + o, 4, SSD1306_WHITE);
    display.drawFastHLine(x0 + SPR_W + 3, PLAYER_BASE_Y - 1 + o, 4, SSD1306_WHITE);
  }
}

void drawHUD() {
  char b[20];
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  snprintf(b, sizeof(b), "PTS %d", score);
  display.setCursor(0, 0);
  display.print(b);
  int rec = hiScore > score ? hiScore : score;
  snprintf(b, sizeof(b), "REC %d", rec);
  display.setCursor(SCR_W - 6 * (int)strlen(b), 0);
  display.print(b);

  display.drawRect(2, 9, 49, 5, SSD1306_WHITE);
  display.fillRect(2, 9, (int)(49 * sf), 5, SSD1306_WHITE);
  int kmh = (int)(50 + 190 * sf);
  display.setCursor(55, 8);
  display.print(kmh);
  float lp = fmodf(dist, LEVEL_DIST) / LEVEL_DIST;
  display.drawRect(78, 9, 48, 5, SSD1306_WHITE);
  display.fillRect(78, 9, (int)(48 * lp), 5, SSD1306_WHITE);
}

void drawExplosion() {
  for (int k = 0; k < 14; k++) {
    float ang = random(0, 360) * 0.0174533f;
    int len = random(6, 24);
    display.drawLine(64, 52, 64 + (int)(cosf(ang) * len * 1.6f),
                             52 + (int)(sinf(ang) * len), SSD1306_WHITE);
  }
}

// Dibuja un fotograma completo y lo envia a la OLED (protegido con el mutex)
void render(bool showPlayer) {
  if (xSemaphoreTake(i2cMutex, portMAX_DELAY) != pdTRUE) return;
  fb = display.getBuffer();
  display.clearDisplay();
  drawMountains();
  drawTerrainAndRoad();
  drawPosts();
  drawEnemies();
  if (showPlayer) drawPlayer();
  drawHUD();

  if (fase == FASE_PLAY && levelBannerT > 0) {
    display.fillRect(34, 30, 60, 11, SSD1306_BLACK);
    display.drawRect(34, 30, 60, 11, SSD1306_WHITE);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(42, 32);
    display.print(F("NIVEL "));
    display.print(level);
  }
  if (fase == FASE_CRASH) {
    drawExplosion();
    display.invertDisplay(((int)(crashT * 8)) & 1);
  }
  display.display();
  xSemaphoreGive(i2cMutex);
}

// Pantalla GAME OVER con el mismo formato que Dino / Snake
void dibujarGameOver() {
  if (xSemaphoreTake(i2cMutex, portMAX_DELAY) != pdTRUE) return;
  display.invertDisplay(false);
  display.clearDisplay();
  display.setTextSize(2); display.setTextColor(WHITE);
  display.setCursor(14, 4);  display.print(F("GAME"));
  display.setCursor(14, 22); display.print(F("OVER"));
  display.setTextSize(1);
  display.setCursor(72, 8);  display.print(F("Pts:")); display.print(score);
  display.setCursor(72, 20); display.print(F("Rec:")); display.print(hiScore);
  if (newRecord) { display.setCursor(72, 32); display.print(F("NUEVO!")); }
  display.setCursor(0, 46);  display.print(F("B2: reiniciar"));
  display.setCursor(0, 56);  display.print(F("B1: volver menu"));
  display.display();
  xSemaphoreGive(i2cMutex);
}

// ------------------------- LOGICA -------------------------
void resetRun() {
  for (int k = 0; k < MAX_EN; k++) en[k].on = false;
  dist = 0; px = 0; vx = 0; mx = 0; sf = 0.3f; speedU = SPD_MIN;
  scoreF = 0; score = 0; level = 1; lastLevel = 1;
  spawnT = 1.5f; levelBannerT = 1.0f; steerShow = 0; newRecord = false;
  rotF = (float)rotationAngle;
  resetTrack();
}

void iniciar() {
  resetRun();
  computeRoad();
  fase = FASE_PLAY;
  overDrawn = false; overArmed = false;
  exitHold = 0;
  lastFrame = millis();
}

void salir() {
  // Esperar a soltar los botones para que no activen una opcion del menu
  uint32_t t0 = millis();
  while ((digitalRead(ledPinB1) == LOW || digitalRead(ledPinB2) == LOW) &&
         millis() - t0 < 2000) {
    delay(10);
  }
  volverAlMenu();
}

void spawnEnemy(float ex, float v) {
  for (int k = 0; k < MAX_EN; k++) {
    if (!en[k].on) { en[k].on = true; en[k].z = ZSPAWN; en[k].ex = ex; en[k].v = v; return; }
  }
}

void trySpawn() {
  for (int k = 0; k < MAX_EN; k++)
    if (en[k].on && en[k].z > ZSPAWN - 6.0f) { spawnT = 0.3f; return; }
  float v = 1.2f + random(0, 16) / 10.0f;
  if (level >= 2 && random(100) < 15 + level * 6) {
    int freeLane = random(0, 3);
    for (int l = 0; l < 3; l++) if (l != freeLane) spawnEnemy(LANE_X[l], v);
  } else {
    spawnEnemy(random(-70, 71) / 100.0f, v);
  }
  float base = 2.3f - 0.16f * level;
  if (base < 1.0f) base = 1.0f;
  spawnT = base * (0.7f + random(0, 61) / 100.0f);
}

void choque() {
  score = (int)scoreF;
  newRecord = false;
  if (score > hiScore) { hiScore = score; newRecord = true; }
  crashT = 0;
  fase = FASE_CRASH;
  SolicitarPulsoJuego(27, 180);          // vibracion al estrellarse
}

bool updateEnemies(float dt) {
  spawnT -= dt;
  if (spawnT <= 0) trySpawn();
  for (int k = 0; k < MAX_EN; k++) {
    Enemy& e = en[k];
    if (!e.on) continue;
    e.z -= (speedU + e.v) * dt;
    if (e.z < ZHIT_NEAR) { e.on = false; scoreF += 25; continue; }
    if (e.z < ZHIT_FAR) {
      int sx, sy, w;
      enemyScreen(e, sx, sy, w);
      int d = sx - 64; if (d < 0) d = -d;
      if (d < (int)((w + SPR_W) * 0.39f)) return true;
    }
  }
  return false;
}

void updatePlay(float dt) {
  float steer = leerGiro();

  float target = 0.45f + 0.10f * level;
  if (target > 1.0f) target = 1.0f;
  sf += (target - sf) * dt * 0.6f;
  speedU = SPD_MIN + (SPD_MAX - SPD_MIN) * sf;
  dist  += speedU * dt;
  scoreF += speedU * dt * 10.0f;
  score = (int)scoreF;

  level = 1 + (int)(dist / LEVEL_DIST);
  if (level != lastLevel) { lastLevel = level; levelBannerT = 1.3f; }
  if (levelBannerT > 0) levelBannerT -= dt;

  float cNear = curveAt(dist + 0.6f);
  float targetV = steer * STEER_SPEED - cNear * DRIFT * (0.4f + 0.6f * sf);
  float a = dt * 10.0f; if (a > 1.0f) a = 1.0f;
  vx += (targetV - vx) * a;
  px += vx * dt;
  mx += cNear * speedU * MTN_K * dt;

  if (steer > 0.15f)             steerShow = 1;
  else if (steer < -0.15f)       steerShow = -1;
  else if (fabsf(cNear) > 0.45f) steerShow = (cNear > 0) ? 1 : -1;
  else                           steerShow = 0;

  computeRoad();

  if (fabsf(px) > OFFROAD_LIMIT) { choque(); return; }
  if (updateEnemies(dt))         { choque(); return; }
}

// Se llama en cada vuelta del loop() principal
void tick() {
  bool b1 = (digitalRead(ledPinB1) == LOW);
  bool b2 = (digitalRead(ledPinB2) == LOW);
  uint32_t now = millis();

  // ---- GAME OVER ----
  if (fase == FASE_OVER) {
    if (!overDrawn) {
      overDrawn = true; overMs = now; overArmed = false;
      dibujarGameOver();
    }
    if (!overArmed) { if (!b1 && !b2) overArmed = true; }
    else if (b1) { salir();   return; }
    else if (b2) { iniciar(); return; }
    if (now - overMs >= AUTOEXT_MS) salir();
    return;
  }

  // ---- Salir manteniendo B1+B2 ----
  if (fase == FASE_PLAY) {
    if (b1 && b2) {
      if (exitHold == 0) exitHold = now;
      else if (now - exitHold >= EXIT_HOLD_MS) { salir(); return; }
    } else {
      exitHold = 0;
    }
  }

  if (now - lastFrame < FRAME_MS) return;
  float dt = (now - lastFrame) / 1000.0f;
  if (dt > 0.06f) dt = 0.06f;
  lastFrame = now;
  frameCount++;

  if (fase == FASE_PLAY) {
    updatePlay(dt);
    render(true);
  } else {                                   // FASE_CRASH
    crashT += dt;
    render(((int)(crashT * 10) & 1) == 0);
    if (crashT > 1.3f) { fase = FASE_OVER; overDrawn = false; }
  }
}

} // namespace RR

// ------------------------- PUNTOS DE ENTRADA -------------------------
void IniciarRoadRush()     { RR::iniciar(); }
void ActualizarRoadRush()  { RR::tick(); }
