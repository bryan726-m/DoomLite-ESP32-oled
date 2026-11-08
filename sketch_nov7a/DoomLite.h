#pragma once

#include "sprites.h"

// ------------------------- OPCIONES -------------------------
// Si al empujar el joystick hacia adelante el personaje retrocede, pon 1
#define DM_FWD_INVERT    0
// Si al empujar a la derecha gira a la izquierda, pon 1
#define DM_TURN_INVERT   0

// Suelo: 0 = negro (como en Wokwi), 1 = punteado con degradado (version anterior)
#define DM_FLOOR_STYLE   0
// Tamano del arma en pixeles (el sprite original es 32x32)
#define DM_GUN_SIZE      24
// Mira central: 0 = sin mira (como Wokwi), 1 = con mira
#define DM_CROSSHAIR     0
// Cuadro de bmp_items_bits que corresponde a cada objeto (si salen al reves, intercambialos)
#define DM_ITEM_MED_FRAME 0
#define DM_ITEM_KEY_FRAME 1

namespace doomlite {

// ------------------------- PANTALLA / TIEMPOS -------------------------
constexpr int      SCR_W        = 128;
constexpr int      SCR_H        = 64;
constexpr int      VIEW_H       = 56;      // zona 3D (el HUD usa las 8 filas de abajo)
constexpr uint32_t FRAME_MS     = 35;      // tope ~28 FPS (la OLED limita igual)
constexpr uint32_t AUTOEXT_MS   = 5000UL;  // Game Over -> menu automatico
constexpr uint32_t EXIT_HOLD_MS = 1500UL;  // B1+B2 para salir

// ------------------------- JUGADOR / MOTOR -------------------------
constexpr int   STICK_DEAD  = 320;     // zona muerta del joystick (ADC, centro 2048)
constexpr float MOVE_SPEED  = 2.4f;    // celdas por segundo
constexpr float ROT_SPEED   = 2.6f;    // rad/s con el joystick a tope
constexpr float FOV_PLANE   = 0.66f;   // ~66 grados de campo de vision
constexpr float PLAYER_R    = 0.22f;
constexpr int   MAX_STEPS   = 22;      // profundidad maxima del rayo (celdas)
constexpr float START_ANG   = 0.0f;    // 0 = mirando al Este

// ------------------------- ENEMIGOS -------------------------
constexpr int8_t IMP_HP      = 3;
constexpr float  IMP_SPEED   = 0.85f;
constexpr float  IMP_SIGHT   = 9.0f;
constexpr float  FIRE_SPEED  = 3.2f;
constexpr float  FIRE_COOL   = 0.35f;  // segundos entre disparos del jugador

// ------------------------- MAPA -------------------------
// '#' pared   '.' piso   'P' inicio   'E' demonio   'M' botiquin
// 'K' llave   'X' salida
constexpr int MW = 26;
constexpr int MH = 20;
static const char* const LEVEL[MH] = {
  "##########################",
  "#......##........##.....K#",
  "#......##...E....##......#",
  "#.P........#..#........E.#",
  "#......##........##......#",
  "#......##..#..#..##......#",
  "#......##.M....E.#####.###",
  "###.#####........#####.###",
  "###.#########.########.###",
  "###.#########.######.....#",
  "#........####.######.....#",
  "#....E...##.......##.....#",
  "#........##...E...##..#..#",
  "#....#...##..#....##..E..#",
  "#........................#",
  "#....#...##.......##..#..#",
  "#......M.##....#..##.....#",
  "#........##.....E.##.M.X.#",
  "###########.......##.....#",
  "##########################"
};

// Cuadros del sprite del imp (bmp_imp_bits, 5 cuadros de 32x32 apilados)
constexpr int IMP_F_STAND = 0;   // 0 y 1 = caminar
constexpr int IMP_F_WALK2 = 1;
constexpr int IMP_F_ATTACK = 2;
constexpr int IMP_F_HIT   = 3;
constexpr int IMP_F_DEAD  = 4;

// Matriz de Bayer 4x4 (sombreado por tramado)
static const uint8_t BAYER[4][4] = {
  {  0,  8,  2, 10 },
  { 12,  4, 14,  6 },
  {  3, 11,  1,  9 },
  { 15,  7, 13,  5 }
};

// ------------------------- ESTADO -------------------------
enum Fase   : uint8_t { PH_TITLE, PH_PLAY, PH_DYING, PH_OVER, PH_WIN };
enum EType  : uint8_t { ET_IMP, ET_FIRE, ET_MED, ET_KEY, ET_EXIT };
enum EState : uint8_t { ES_IDLE, ES_ALERT, ES_FIRING, ES_HIT, ES_MELEE, ES_DEAD };

struct Ent {
  uint8_t type;
  uint8_t state;
  bool    on;
  int8_t  hp;
  float   x, y, vx, vy;
  float   timer;
  float   dist;
};
constexpr int MAX_ENT = 40;
Ent ent[MAX_ENT];

Fase  fase = PH_TITLE;
float px = 2.5f, py = 3.5f, ang = 0, dirX = 1, dirY = 0, plX = 0, plY = FOV_PLANE;
int   health = 100, keys = 0, kills = 0, totalImps = 0;
int   score = 0, hiScore = 0;
bool  newRecord = false, won = false;
float fireCd = 0, gunKick = 0, flashT = 0, bobPh = 0, bobAmt = 0, deathT = 0, msgT = 0;
bool  fireArmed = false, invertOn = false;
bool  titleDrawn = false, titleArmed = false, overDrawn = false, overArmed = false;
uint32_t lastFrame = 0, overMs = 0, exitHold = 0;
const char* msg = "";

float    zbuf[SCR_W];
uint8_t* fb = nullptr;

// ------------------------- UTILIDADES -------------------------
static inline void putPix(int x, int y, bool white) {
  if ((unsigned)x >= (unsigned)SCR_W || (unsigned)y >= (unsigned)SCR_H) return;
  uint8_t* p = fb + x + (y >> 3) * SCR_W;
  uint8_t m = 1 << (y & 7);
  if (white) *p |= m; else *p &= ~m;
}

static inline void xorPix(int x, int y) {
  if ((unsigned)x >= (unsigned)SCR_W || (unsigned)y >= (unsigned)SCR_H) return;
  fb[x + (y >> 3) * SCR_W] ^= (1 << (y & 7));
}

static inline bool isWall(int x, int y) {
  if (x < 0 || y < 0 || x >= MW || y >= MH) return true;
  return LEVEL[y][x] == '#';
}

static bool canStand(float x, float y, float r) {
  return !isWall((int)(x - r), (int)(y - r)) && !isWall((int)(x + r), (int)(y - r)) &&
         !isWall((int)(x - r), (int)(y + r)) && !isWall((int)(x + r), (int)(y + r));
}

// Linea de vista sin paredes entre dos puntos
static bool los(float x0, float y0, float x1, float y1) {
  float dx = x1 - x0, dy = y1 - y0;
  float d  = sqrtf(dx * dx + dy * dy);
  int n = (int)(d / 0.15f) + 1;
  float sx = dx / n, sy = dy / n;
  for (int i = 1; i < n; i++)
    if (isWall((int)(x0 + sx * i), (int)(y0 + sy * i))) return false;
  return true;
}

// Eje analogico -> -1..+1 con zona muerta
static float axis(int pin, bool inv) {
  int v = analogRead(pin) - ADC_MID;
  int a = v < 0 ? -v : v;
  float s = 0;
  if (a > STICK_DEAD) {
    s = (float)(a - STICK_DEAD) / (2047.0f - STICK_DEAD);
    if (s > 1.0f) s = 1.0f;
    if (v < 0) s = -s;
  }
  return inv ? -s : s;
}

static void setMsg(const char* m, float t) { msg = m; msgT = t; }

// Texto con la fuente 4x6 de sprites.h (solo mayusculas, numeros y . , - _ ( ) { } [ ] #)
static void drawText(const char* t, int x, int y) {
  for (; *t; t++, x += 5) {
    char ch = *t;
    if (ch >= 'a' && ch <= 'z') ch -= 32;
    int idx = -1;
    for (int i = 0; CHAR_MAP[i]; i++) {
      if (CHAR_MAP[i] == ch) { idx = i; break; }
    }
    if (idx < 0) continue;                       // caracter no disponible: queda un hueco
    for (int r = 0; r < CHAR_HEIGHT; r++) {
      uint8_t b = pgm_read_byte(bmp_font + r * bmp_font_width + (idx >> 1));
      uint8_t n = (idx & 1) ? (b & 0x0F) : (b >> 4);
      for (int c = 0; c < 4; c++) putPix(x + c, y + r, (n & (8 >> c)) != 0);
    }
  }
}

// ------------------------- ENTIDADES -------------------------
static void addEnt(uint8_t type, float x, float y, int8_t hp) {
  for (int k = 0; k < MAX_ENT; k++) {
    if (!ent[k].on) {
      ent[k].type = type; ent[k].state = ES_IDLE; ent[k].on = true; ent[k].hp = hp;
      ent[k].x = x; ent[k].y = y; ent[k].vx = 0; ent[k].vy = 0;
      ent[k].timer = 0; ent[k].dist = 99;
      return;
    }
  }
}

static void loadLevel() {
  for (int k = 0; k < MAX_ENT; k++) ent[k].on = false;
  totalImps = 0;
  for (int y = 0; y < MH; y++) {
    for (int x = 0; x < MW; x++) {
      char c = LEVEL[y][x];
      float cx = x + 0.5f, cy = y + 0.5f;
      switch (c) {
        case 'P': px = cx; py = cy; break;
        case 'E': addEnt(ET_IMP, cx, cy, IMP_HP); totalImps++; break;
        case 'M': addEnt(ET_MED, cx, cy, 0); break;
        case 'K': addEnt(ET_KEY, cx, cy, 0); break;
        case 'X': addEnt(ET_EXIT, cx, cy, 0); break;
      }
    }
  }
}

static void setCamera() {
  dirX = cosf(ang); dirY = sinf(ang);
  plX = -dirY * FOV_PLANE;
  plY =  dirX * FOV_PLANE;
}

static void hurt(int d) {
  if (fase != PH_PLAY) return;
  health -= d;
  flashT = 0.15f;
  SolicitarPulsoJuego(30, 250);
  if (health <= 0) {
    health = 0;
    fase = PH_DYING;
    deathT = 0;
    SolicitarPulsoJuego(35, 500);
  }
}

static void spawnFire(float x, float y) {
  float dx = px - x, dy = py - y;
  float d = sqrtf(dx * dx + dy * dy);
  if (d < 0.01f) return;
  for (int k = 0; k < MAX_ENT; k++) {
    if (!ent[k].on) {
      ent[k].type = ET_FIRE; ent[k].state = ES_IDLE; ent[k].on = true; ent[k].hp = 0;
      ent[k].x = x; ent[k].y = y;
      ent[k].vx = dx / d * FIRE_SPEED;
      ent[k].vy = dy / d * FIRE_SPEED;
      ent[k].timer = 0; ent[k].dist = d;
      return;
    }
  }
}

static void updateImp(Ent& e, float dt) {
  if (e.state == ES_DEAD) return;
  e.timer -= dt;
  float d = e.dist;
  bool see = (d < IMP_SIGHT + 2.0f) && los(e.x, e.y, px, py);

  switch (e.state) {
    case ES_IDLE:
      if (see && d < IMP_SIGHT) { e.state = ES_ALERT; e.timer = 0.8f + random(0, 60) / 100.0f; }
      break;
    case ES_HIT:
      if (e.timer <= 0) { e.state = ES_ALERT; e.timer = 1.0f; }
      break;
    case ES_FIRING:
      if (e.timer <= 0) { e.state = ES_ALERT; e.timer = 1.6f + random(0, 120) / 100.0f; }
      break;
    case ES_MELEE:
      if (d > 1.5f) { e.state = ES_ALERT; e.timer = 0.6f; }
      else if (e.timer <= 0) { hurt(8); e.timer = 0.9f; }
      break;
    case ES_ALERT:
      if (d <= 1.1f) { e.state = ES_MELEE; e.timer = 0.5f; }
      else if (e.timer <= 0 && see && d > 2.0f) {
        spawnFire(e.x, e.y);
        e.state = ES_FIRING; e.timer = 0.45f;
      } else if (d > 0.01f) {
        float sp = IMP_SPEED * dt;
        float ux = (px - e.x) / d, uy = (py - e.y) / d;
        float nx = e.x + ux * sp, ny = e.y + uy * sp;
        if (canStand(nx, e.y, 0.25f)) e.x = nx;
        if (canStand(e.x, ny, 0.25f)) e.y = ny;
      }
      break;
  }
}

static void winGame() {
  won = true;
  score = kills * 100 + 500 + health * 2;
  newRecord = false;
  if (score > hiScore) { hiScore = score; newRecord = true; }
  fase = PH_WIN; overDrawn = false;
  SolicitarPulsoJuego(30, 400);
}

static void updateEntities(float dt) {
  for (int k = 0; k < MAX_ENT; k++) {
    Ent& e = ent[k];
    if (!e.on) continue;
    float dx = px - e.x, dy = py - e.y;
    e.dist = sqrtf(dx * dx + dy * dy);

    switch (e.type) {
      case ET_IMP:
        updateImp(e, dt);
        break;
      case ET_FIRE: {
        float nx = e.x + e.vx * dt, ny = e.y + e.vy * dt;
        if (isWall((int)nx, (int)ny)) { e.on = false; break; }
        e.x = nx; e.y = ny;
        if (e.dist < 0.4f) { hurt(10); e.on = false; }
        break;
      }
      case ET_MED:
        if (e.dist < 0.55f && health < 100) {
          health += 25; if (health > 100) health = 100;
          e.on = false; flashT = 0.1f; setMsg("SALUD", 1.0f);
        }
        break;
      case ET_KEY:
        if (e.dist < 0.55f) {
          keys++; e.on = false; flashT = 0.1f; setMsg("LLAVE", 1.5f);
        }
        break;
      case ET_EXIT:
        if (e.dist < 0.7f) {
          if (keys > 0) { winGame(); return; }
          else if (msgT <= 0) setMsg("NECESITAS LLAVE", 1.2f);
        }
        break;
    }
  }
}

// ------------------------- DISPARO -------------------------
static void shoot() {
  SolicitarPulsoJuego(27, 180);          // mismo pulso al disparar
  gunKick = 1.0f;

  int   best = -1;
  float bestF = 99;
  for (int k = 0; k < MAX_ENT; k++) {
    Ent& e = ent[k];
    if (!e.on || e.type != ET_IMP || e.state == ES_DEAD) continue;
    float rx = e.x - px, ry = e.y - py;
    float fwd = rx * dirX + ry * dirY;
    if (fwd < 0.3f || fwd > 14.0f) continue;
    float lat = fabsf(rx * dirY - ry * dirX);
    if (lat > 0.42f) continue;
    if (!los(px, py, e.x, e.y)) continue;
    if (fwd < bestF) { bestF = fwd; best = k; }
  }
  if (best >= 0) {
    Ent& e = ent[best];
    e.hp -= (bestF < 2.5f) ? 2 : 1;
    if (e.hp <= 0) {
      e.state = ES_DEAD; kills++;
    } else {
      e.state = ES_HIT; e.timer = 0.25f;
    }
  }
}

// ------------------------- DIBUJO 3D -------------------------
static inline void wallColumn(int x, int y0, int y1, int shade) {
  for (int y = y0; y <= y1; y++)
    putPix(x, y, shade > BAYER[y & 3][x & 3]);
}

// Suelo: por defecto negro (la pantalla ya viene borrada). Estilo 1 = punteado anterior.
static void drawFloor(int hz) {
#if DM_FLOOR_STYLE == 1
  int h0 = hz < 0 ? 0 : hz;
  int range = VIEW_H - h0 + 1;
  for (int y = h0; y < VIEW_H; y++) {
    int s = 1 + ((y - h0) * 3) / range;
    for (int x = 0; x < SCR_W; x++)
      if (s > BAYER[y & 3][x & 3]) putPix(x, y, true);
  }
#else
  (void)hz;
#endif
}

static void drawWalls(int hz) {
  for (int x = 0; x < SCR_W; x++) {
    float camX = 2.0f * x / SCR_W - 1.0f;
    float rdx = dirX + plX * camX;
    float rdy = dirY + plY * camX;
    int   mx = (int)px, my = (int)py;
    float ddx = (rdx == 0) ? 1e30f : fabsf(1.0f / rdx);
    float ddy = (rdy == 0) ? 1e30f : fabsf(1.0f / rdy);
    int   stepX, stepY;
    float sdx, sdy;
    if (rdx < 0) { stepX = -1; sdx = (px - mx) * ddx; }
    else         { stepX =  1; sdx = (mx + 1.0f - px) * ddx; }
    if (rdy < 0) { stepY = -1; sdy = (py - my) * ddy; }
    else         { stepY =  1; sdy = (my + 1.0f - py) * ddy; }

    int  side = 0, steps = 0;
    bool hit = false;
    while (!hit && steps < MAX_STEPS) {
      if (sdx < sdy) { sdx += ddx; mx += stepX; side = 0; }
      else           { sdy += ddy; my += stepY; side = 1; }
      if (isWall(mx, my)) hit = true;
      steps++;
    }
    if (!hit) { zbuf[x] = 999.0f; continue; }

    float pd = (side == 0) ? (sdx - ddx) : (sdy - ddy);
    if (pd < 0.05f) pd = 0.05f;
    zbuf[x] = pd;

    float wallX = (side == 0) ? (py + pd * rdy) : (px + pd * rdx);
    wallX -= floorf(wallX);

    int lineH = (int)(VIEW_H / pd);
    int y0 = hz - lineH / 2;
    int y1 = hz + lineH / 2;
    if (y0 < 0) y0 = 0;
    if (y1 > VIEW_H - 1) y1 = VIEW_H - 1;

    // Cerca = blanco solido; lejos = se va tramando hasta casi negro
    int shade = 18 - (int)(pd * 1.5f);
    if (side) shade -= 2;
    if (wallX < 0.05f || wallX > 0.95f) shade -= 6;   // juntas entre bloques
    if (shade < 2) shade = 2;
    wallColumn(x, y0, y1, shade);
  }
}

static inline bool bitAt(const uint8_t* d, int bpr, int row, int col) {
  return (pgm_read_byte(d + row * bpr + (col >> 3)) & (0x80 >> (col & 7))) != 0;
}

// Atenuacion por distancia para los sprites (16 = sin atenuar)
static inline int fadeFor(float d) {
  if (d <= 3.0f) return 16;
  int f = 16 - (int)((d - 3.0f) * 1.6f);
  return f < 6 ? 6 : f;
}

// Dibuja un sprite (bits + mascara opcional, PROGMEM) escalado a w x h,
// recortado contra el z-buffer de paredes y limitado a la zona 3D.
//   frame: cuadro dentro de un sprite apilado verticalmente
//   mask == nullptr -> sprite opaco
//   depth: profundidad para el z-buffer (0 = siempre visible, p. ej. el arma)
//   fade: 16 = sin atenuar; menor = mas tramado en los pixeles blancos
static void drawSpriteBM(const uint8_t* bits, const uint8_t* mask, int tw, int th, int frame,
                         int cx, int bottom, int w, int h, float depth, int fade) {
  if (w < 1 || h < 1) return;
  const int bpr = (tw + 7) >> 3;
  const int rowBase = frame * th;
  int left = cx - w / 2;
  int top  = bottom - h;
  int sx0 = left < 0 ? 0 : left;
  int sx1 = (left + w - 1) > (SCR_W - 1) ? (SCR_W - 1) : (left + w - 1);
  int sy0 = top < 0 ? 0 : top;
  int sy1 = (bottom - 1) > (VIEW_H - 1) ? (VIEW_H - 1) : (bottom - 1);

  for (int sx = sx0; sx <= sx1; sx++) {
    if (depth >= zbuf[sx]) continue;
    int u = ((sx - left) * tw) / w;
    for (int sy = sy0; sy <= sy1; sy++) {
      int row = rowBase + ((sy - top) * th) / h;
      if (mask && !bitAt(mask, bpr, row, u)) continue;
      bool white = bitAt(bits, bpr, row, u);
      if (white && fade < 16 && fade <= BAYER[sy & 3][sx & 3]) white = false;
      putPix(sx, sy, white);
    }
  }
}

static void drawEntities(int hz) {
  int   idx[MAX_ENT];
  float td[MAX_ENT];
  int   n = 0;
  float invDet = 1.0f / (plX * dirY - dirX * plY);

  for (int k = 0; k < MAX_ENT; k++) {
    if (!ent[k].on) continue;
    float rx = ent[k].x - px, ry = ent[k].y - py;
    float ty = invDet * (-plY * rx + plX * ry);        // profundidad
    if (ty < 0.25f || ty > 18.0f) continue;
    td[k] = ty;
    idx[n++] = k;
  }
  // ordenar de lejos a cerca
  for (int a = 1; a < n; a++) {
    int t = idx[a], b = a - 1;
    while (b >= 0 && td[idx[b]] < td[t]) { idx[b + 1] = idx[b]; b--; }
    idx[b + 1] = t;
  }

  for (int a = 0; a < n; a++) {
    Ent& e = ent[idx[a]];
    float rx = e.x - px, ry = e.y - py;
    float tx = invDet * (dirY * rx - dirX * ry);
    float ty = td[idx[a]];
    int   sxc = (int)((SCR_W / 2) * (1.0f + tx / ty));
    if (sxc < -SCR_W || sxc > 2 * SCR_W) continue;

    int   lineH  = (int)(VIEW_H / ty);
    int   floorY = hz + lineH / 2;
    int   fade   = fadeFor(ty);

    switch (e.type) {
      case ET_IMP: {
        int s = (lineH * 9) / 10;
        int frame;
        switch (e.state) {
          case ES_ALERT:  frame = ((millis() / 400UL) & 1) ? IMP_F_WALK2 : IMP_F_STAND; break;
          case ES_FIRING:
          case ES_MELEE:  frame = IMP_F_ATTACK; break;
          case ES_HIT:    frame = IMP_F_HIT;    break;
          case ES_DEAD:   frame = IMP_F_DEAD;   break;
          default:        frame = IMP_F_STAND;  break;
        }
        drawSpriteBM(bmp_imp_bits, bmp_imp_mask, BMP_IMP_WIDTH, BMP_IMP_HEIGHT, frame,
                     sxc, floorY, s, s, ty, fade);
        break;
      }
      case ET_FIRE: {
        int s = lineH / 3; if (s < 3) s = 3;
        drawSpriteBM(bmp_fireball_bits, bmp_fireball_mask, BMP_FIREBALL_WIDTH, BMP_FIREBALL_HEIGHT, 0,
                     sxc, hz + s / 2, s, s, ty, 16);
        break;
      }
      case ET_MED: {
        int s = lineH / 2; if (s < 4) s = 4;
        drawSpriteBM(bmp_items_bits, bmp_items_mask, BMP_ITEMS_WIDTH, BMP_ITEMS_HEIGHT, DM_ITEM_MED_FRAME,
                     sxc, floorY, s, s, ty, fade);
        break;
      }
      case ET_KEY: {
        int s = lineH / 2; if (s < 4) s = 4;
        int lift = (int)(sinf(millis() * 0.006f) * 2);
        drawSpriteBM(bmp_items_bits, bmp_items_mask, BMP_ITEMS_WIDTH, BMP_ITEMS_HEIGHT, DM_ITEM_KEY_FRAME,
                     sxc, floorY - lineH / 8 + lift, s, s, ty, fade);
        break;
      }
      case ET_EXIT: {
        int s = lineH;
        drawSpriteBM(bmp_door_bits, nullptr, BMP_DOOR_WIDTH, BMP_DOOR_HEIGHT, 0,
                     sxc, floorY, s, s, ty, fade);
        break;
      }
    }
  }
}

static void drawGun() {
  const int S = DM_GUN_SIZE;
  int gx = 64 - S / 2 + (int)(sinf(bobPh) * bobAmt * 3.0f);
  int gy = VIEW_H - S + (int)(fabsf(cosf(bobPh)) * bobAmt * 2.0f) + (int)(gunKick * 5.0f);

  // Fogonazo (se dibuja primero; el arma queda por encima)
  if (gunKick > 0.65f) {
    int fx = gx + (18 * S) / 32 - BMP_FIRE_WIDTH / 2 + 1;
    int fy = gy - BMP_FIRE_HEIGHT + 6;
    // contorno negro para que se lea sobre las paredes blancas
    display.drawBitmap(fx - 1, fy,     bmp_fire_bits, BMP_FIRE_WIDTH, BMP_FIRE_HEIGHT, SSD1306_BLACK);
    display.drawBitmap(fx + 1, fy,     bmp_fire_bits, BMP_FIRE_WIDTH, BMP_FIRE_HEIGHT, SSD1306_BLACK);
    display.drawBitmap(fx,     fy - 1, bmp_fire_bits, BMP_FIRE_WIDTH, BMP_FIRE_HEIGHT, SSD1306_BLACK);
    display.drawBitmap(fx,     fy + 1, bmp_fire_bits, BMP_FIRE_WIDTH, BMP_FIRE_HEIGHT, SSD1306_BLACK);
    display.drawBitmap(fx,     fy,     bmp_fire_bits, BMP_FIRE_WIDTH, BMP_FIRE_HEIGHT, SSD1306_WHITE);
  }

  drawSpriteBM(bmp_gun_bits, bmp_gun_mask, BMP_GUN_WIDTH, BMP_GUN_HEIGHT, 0,
               gx + S / 2, gy + S, S, S, 0.0f, 16);
}

#if DM_CROSSHAIR
static void drawCrosshair() {
  const int cx = SCR_W / 2, cy = VIEW_H / 2;
  xorPix(cx - 4, cy); xorPix(cx - 3, cy);
  xorPix(cx + 3, cy); xorPix(cx + 4, cy);
  xorPix(cx, cy - 4); xorPix(cx, cy - 3);
  xorPix(cx, cy + 3);
}
#endif

static void drawHUD() {
  char buf[8];
  const int y = VIEW_H + 1;

  // Limpia la franja del HUD
  display.fillRect(0, VIEW_H, SCR_W, SCR_H - VIEW_H, SSD1306_BLACK);

  // Cruz + salud
  display.fillRect(2, y,     3, 7, SSD1306_WHITE);
  display.fillRect(0, y + 2, 7, 3, SSD1306_WHITE);
  snprintf(buf, sizeof(buf), "%d", health);
  drawText(buf, 10, y);

  // Llave + cantidad
  display.drawRect(38, y + 1, 5, 5, SSD1306_WHITE);          // anillo
  display.drawFastHLine(43, y + 3, 6, SSD1306_WHITE);        // cuerpo
  display.drawFastVLine(47, y + 4, 2, SSD1306_WHITE);        // dientes
  display.drawFastVLine(45, y + 4, 2, SSD1306_WHITE);
  snprintf(buf, sizeof(buf), "%d", keys);
  drawText(buf, 53, y);

  // Demonios restantes
  drawText("IMP", 84, y);
  snprintf(buf, sizeof(buf), "%d", totalImps - kills);
  drawText(buf, 104, y);

  // Mensaje emergente
  if (msgT > 0) {
    int w = (int)strlen(msg) * 5 + 4;
    int x = (SCR_W - w) / 2;
    display.fillRect(x, 8, w, 9, SSD1306_BLACK);
    display.drawRect(x, 8, w, 9, SSD1306_WHITE);
    drawText(msg, x + 3, 9);
  }
}

// Fotograma completo (protegido con el mutex)
static void render() {
  if (xSemaphoreTake(i2cMutex, portMAX_DELAY) != pdTRUE) return;
  display.clearDisplay();
  fb = display.getBuffer();

  int deathDrop = 0;
  if (fase == PH_DYING) deathDrop = (int)(deathT * 22.0f);
  int hz = VIEW_H / 2 + (int)(sinf(bobPh) * bobAmt * 1.5f) + deathDrop;

  drawFloor(hz);
  drawWalls(hz);
  drawEntities(hz);
  if (fase != PH_DYING) {
    drawGun();
#if DM_CROSSHAIR
    drawCrosshair();
#endif
  }
  drawHUD();

  bool inv = (flashT > 0) || (fase == PH_DYING && ((int)(deathT * 8) & 1));
  if (inv != invertOn) { display.invertDisplay(inv); invertOn = inv; }
  display.display();
  xSemaphoreGive(i2cMutex);
}

// ------------------------- PANTALLAS -------------------------
static void dibujarTitulo() {
  if (xSemaphoreTake(i2cMutex, portMAX_DELAY) != pdTRUE) return;
  display.invertDisplay(false); invertOn = false;
  display.clearDisplay();
  fb = display.getBuffer();
  display.drawBitmap((SCR_W - BMP_LOGO_WIDTH) / 2, 2,
                     bmp_logo_bits, BMP_LOGO_WIDTH, BMP_LOGO_HEIGHT, SSD1306_WHITE);
  drawText("PRESS FIRE", (SCR_W - 49) / 2, 54);
  display.display();
  xSemaphoreGive(i2cMutex);
}

static void dibujarFin() {
  if (xSemaphoreTake(i2cMutex, portMAX_DELAY) != pdTRUE) return;
  display.invertDisplay(false); invertOn = false;
  display.clearDisplay();
  display.setTextSize(2); display.setTextColor(SSD1306_WHITE);
  if (won) {
    display.setCursor(14, 4);  display.print(F("YOU"));
    display.setCursor(14, 22); display.print(F("WIN!"));
  } else {
    display.setCursor(14, 4);  display.print(F("GAME"));
    display.setCursor(14, 22); display.print(F("OVER"));
  }
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
static void reiniciarPartida() {
  loadLevel();
  ang = START_ANG; setCamera();
  health = 100; keys = 0; kills = 0; won = false;
  score = 0; newRecord = false;
  fireCd = 0; gunKick = 0; flashT = 0; bobPh = 0; bobAmt = 0; deathT = 0; msgT = 0;
  fireArmed = false;      // exige soltar el boton antes del primer disparo
  overDrawn = false; overArmed = false;
  exitHold = 0;
  lastFrame = millis();
}

// conTitulo = true la primera vez; false al reiniciar tras Game Over
static void iniciar(bool conTitulo) {
  randomSeed(analogRead(InputA0));
  reiniciarPartida();
  if (conTitulo) { fase = PH_TITLE; titleDrawn = false; titleArmed = false; }
  else           { fase = PH_PLAY; }
}

static void salir() {
  // Esperar a soltar los botones para que no activen una opcion del menu
  uint32_t t0 = millis();
  while ((digitalRead(ledPinB1) == LOW || digitalRead(ledPinB2) == LOW) &&
         millis() - t0 < 2000) {
    delay(10);
  }
  if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
    display.invertDisplay(false); invertOn = false;
    xSemaphoreGive(i2cMutex);
  }
  volverAlMenu();
}

static void updatePlay(float dt) {
  bool b1 = (digitalRead(ledPinB1) == LOW);
  bool b2 = (digitalRead(ledPinB2) == LOW);
  bool fireBtn = (b1 != b2);              // B1+B2 a la vez = salir, no dispara

  float fwd = axis(InputA2, DM_FWD_INVERT);
  float trn = axis(InputA3, DM_TURN_INVERT);

  // Giro
  ang += trn * ROT_SPEED * dt;
  if (ang > 6.2831853f) ang -= 6.2831853f;
  if (ang < 0)          ang += 6.2831853f;
  setCamera();

  // Movimiento con deslizamiento contra paredes
  float v  = fwd * MOVE_SPEED * dt;
  float nx = px + dirX * v, ny = py + dirY * v;
  if (canStand(nx, py, PLAYER_R)) px = nx;
  if (canStand(px, ny, PLAYER_R)) py = ny;

  // Balanceo de camara y arma
  float af = fabsf(fwd);
  float k  = dt * 8.0f; if (k > 1.0f) k = 1.0f;
  bobAmt += (af - bobAmt) * k;
  bobPh  += dt * 10.0f * bobAmt;

  // Disparo (mantener = fuego automatico lento)
  if (!fireBtn) fireArmed = true;
  fireCd -= dt; if (fireCd < 0) fireCd = 0;
  if (fireBtn && fireArmed && fireCd <= 0) { shoot(); fireCd = FIRE_COOL; }

  gunKick -= dt * 4.0f; if (gunKick < 0) gunKick = 0;
  if (flashT > 0) flashT -= dt;
  if (msgT   > 0) msgT   -= dt;

  updateEntities(dt);
}

// Se llama en cada vuelta del loop() principal
static void tick() {
  bool b1 = (digitalRead(ledPinB1) == LOW);
  bool b2 = (digitalRead(ledPinB2) == LOW);
  uint32_t now = millis();

  // ---- GAME OVER / VICTORIA ----
  if (fase == PH_OVER || fase == PH_WIN) {
    if (!overDrawn) {
      overDrawn = true; overMs = now; overArmed = false;
      dibujarFin();
    }
    if (!overArmed) { if (!b1 && !b2) overArmed = true; }
    else if (b1) { salir(); return; }
    else if (b2) { iniciar(false); return; }
    if (now - overMs >= AUTOEXT_MS) salir();
    return;
  }

  // ---- Salir manteniendo B1+B2 ----
  if (b1 && b2) {
    if (exitHold == 0) exitHold = now;
    else if (now - exitHold >= EXIT_HOLD_MS) { salir(); return; }
  } else {
    exitHold = 0;
  }

  // ---- TITULO ----
  if (fase == PH_TITLE) {
    if (!titleDrawn) { titleDrawn = true; dibujarTitulo(); }
    if (!titleArmed) { if (!b1 && !b2) titleArmed = true; }
    else if (b1 != b2) { fase = PH_PLAY; lastFrame = now; fireArmed = false; }
    return;
  }

  if (now - lastFrame < FRAME_MS) return;
  float dt = (now - lastFrame) / 1000.0f;
  if (dt > 0.08f) dt = 0.08f;
  lastFrame = now;

  if (fase == PH_PLAY) {
    updatePlay(dt);
    if (fase == PH_PLAY) render();
    else if (fase == PH_DYING) render();
    // si ganaste, winGame() ya cambio la fase a PH_WIN y se dibuja en el siguiente ciclo
  } else if (fase == PH_DYING) {
    deathT += dt;
    if (flashT > 0) flashT -= dt;
    render();
    if (deathT > 1.4f) {
      score = kills * 100;
      newRecord = false;
      if (score > hiScore) { hiScore = score; newRecord = true; }
      fase = PH_OVER; overDrawn = false;
    }
  }
}

} // namespace doomlite

// ------------------------- PUNTOS DE ENTRADA -------------------------
void IniciarDoom()     { doomlite::iniciar(true); }
void ActualizarDoom()  { doomlite::tick(); }