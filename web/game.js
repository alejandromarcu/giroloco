// Giroloco - Browser Port
// Original: Borland Turbo C++ for DOS (1996-1997) by Alejandro Marcu
// Ported to HTML5 Canvas

const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');

// ---- Constants (from DEFIN.HPP) ----
const DIST_NODOX = 40;
const DIST_NODOY = 30;
const MARGEN_X = 60;
const MARGEN_Y = 45;
const NODOS_X = 14;
const NODOS_Y = 8;
const RADIO_NODO_X = 7;
const RADIO_NODO_Y = 5;

// ROT constants
const ROT_ANCHO_X = 14;
const ROT_ANCHO_Y = 10;
const ROT_ALTO_X = 30;
const ROT_ALTO_Y = 23;
const ROT_VEL_ANGOST = 2;

// EGA 16-color palette
const EGA = [
  '#000000', '#0000AA', '#00AA00', '#00AAAA',
  '#AA0000', '#AA00AA', '#AA5500', '#AAAAAA',
  '#555555', '#5555FF', '#55FF55', '#55FFFF',
  '#FF5555', '#FF55FF', '#FFFF55', '#FFFFFF'
];
const BLACK=0, BLUE=1, GREEN=2, CYAN=3, RED=4, MAGENTA=5, BROWN=6, LIGHTGRAY=7;
const DARKGRAY=8, LIGHTBLUE=9, LIGHTGREEN=10, LIGHTCYAN=11, LIGHTRED=12, LIGHTMAGENTA=13, YELLOW=14, WHITE=15;

// Node image indices (from NODOS.HPP)
const INODO = 1, INODO_AS = 2, INODO_EN = 3, INODO_EN_SIN_EN = 16;
const INODO_COMIENZO = 24, INODO_FINAL = 25, INODO_1PASO = 32;
const INODO_VIDA = 36, INODO_PUNTOS = 37, INODO_PILDTC = 38, INODO_PILDMB = 39;

// Sound indices
const S_PASO_NODO = 0, S_PERDIO_VIDA = 1, S_EXPLOSION = 2;
const S_FIN_NODO_EN = 3, S_APARECER = 4, S_AVISO_FIN_PILDTC = 5, S_FIN_PILDTC = 6;

// Enemy constants
const B_SAncho = 10, B_SAlto = 10;

// Sin/Cos lookup (360 entries, scaled by 256)
const SIN = [], COS = [];
for (let i = 0; i < 360; i++) {
  SIN[i] = Math.round(Math.sin(i * Math.PI / 180) * 256);
  COS[i] = Math.round(Math.cos(i * Math.PI / 180) * 256);
}

function VerdX(x) { return x * DIST_NODOX + MARGEN_X; }
function VerdY(y) { return y * DIST_NODOY + MARGEN_Y; }
function AngPar(a1, a2) { return Math.abs(a1 - a2) < 30; }

// ---- Game state enums ----
const NADA = 0, PERDIO = 1, PASO_NIVEL = 2, PULSO_ESC = 3, GANO = 4;
const EXISTE = 2, NO_ENERGIA = 1, NO_EXISTE = 0;

// Rot_acciones
const ACT_NORMAL = 0;
const ACT_PASAR_INICIAR = 1, ACT_PASAR_ANGOSTAR = 2, ACT_PASAR = 3, ACT_PASAR_ENSANCHAR = 4;
const ACT_APARECER_INICIAR = 5, ACT_APARECER_CRECER = 6, ACT_APARECER_ESPERA = 7;
const ACT_PERDER_INICIAR = 8, ACT_PERDER_ACHICAR = 9, ACT_PERDER_FIN = 10;
const ACT_DESAPARECER_INICIAR = 11, ACT_DESAPARECER_ACHICAR = 12, ACT_DESAPARECER_FIN = 13;

// ---- Asset loading ----
let levels = [];
let nodeSprites = null;
let enemySprites = null;
let images = {};
let sounds = {};
let soundEnabled = true;
let assetsLoaded = false;

async function loadAssets() {
  const [levelsResp, nodesResp, enemiesResp] = await Promise.all([
    fetch('assets/levels.json').then(r => r.json()),
    fetch('assets/nodes.json').then(r => r.json()),
    fetch('assets/enemies.json').then(r => r.json()),
  ]);
  levels = levelsResp;
  nodeSprites = nodesResp;
  enemySprites = enemiesResp;

  // Pre-render node sprite ImageData for faster drawing
  preRenderNodeSprites();
  preRenderEnemySprites();

  const imageNames = ['i0', 'i1', 'i2', 'i3', 'i4', 'i5', 'i6', 'f0', 'f1', 'f2', 'f3', 'f4', 'f5'];
  await Promise.all(imageNames.map(name =>
    new Promise(resolve => {
      const img = new Image();
      img.onload = () => { images[name] = img; resolve(); };
      img.onerror = () => resolve();
      img.src = `assets/images/${name}.png`;
    })
  ));

  for (let i = 0; i < 7; i++) {
    try {
      sounds[i] = new Audio(`assets/sounds/spc${i}.wav`);
      sounds[i].volume = 0.3;
    } catch(e) {}
  }

  assetsLoaded = true;
}

function playSound(id) {
  if (!soundEnabled || !sounds[id]) return;
  try {
    const s = sounds[id].cloneNode();
    s.volume = 0.3;
    s.play().catch(() => {});
  } catch(e) {}
}

// ---- Pre-rendered sprite caches ----
let nodeImageCache = {};
let enemyImageCache = {};

// EGA palette as RGB arrays for pixel manipulation
const EGA_RGB = [
  [0x00,0x00,0x00], [0x00,0x00,0xAA], [0x00,0xAA,0x00], [0x00,0xAA,0xAA],
  [0xAA,0x00,0x00], [0xAA,0x00,0xAA], [0xAA,0x55,0x00], [0xAA,0xAA,0xAA],
  [0x55,0x55,0x55], [0x55,0x55,0xFF], [0x55,0xFF,0x55], [0x55,0xFF,0xFF],
  [0xFF,0x55,0x55], [0xFF,0x55,0xFF], [0xFF,0xFF,0x55], [0xFF,0xFF,0xFF],
];

// Map an RGB pixel to the nearest EGA color index
function rgbToEGA(r, g, b) {
  let best = 0, bestDist = Infinity;
  for (let i = 0; i < 16; i++) {
    const dr = r - EGA_RGB[i][0], dg = g - EGA_RGB[i][1], db = b - EGA_RGB[i][2];
    const d = dr*dr + dg*dg + db*db;
    if (d < bestDist) { bestDist = d; best = i; }
  }
  return best;
}

function preRenderNodeSprites() {
  // Node sprites are rendered at draw time using AND/OR masking
  // Just store the raw sprite/mask data for later use
}

// Draw a node sprite using proper BGI AND/OR masking against the canvas
function drawNodeSprite(spriteIdx, x, y) {
  if (!nodeSprites) return;
  const sprite = nodeSprites.sprites[spriteIdx];
  if (!sprite || !sprite.w || !sprite.h) return;

  const px = x - RADIO_NODO_X;
  const py = y - RADIO_NODO_Y;
  const w = sprite.w, h = sprite.h;

  // Read background pixels from canvas
  const bgData = ctx.getImageData(px, py, w, h);

  for (let p = 0; p < w * h; p++) {
    const spriteColor = sprite.pixels[p] & 0x0F;
    const maskColor = sprite.mask[p] & 0x0F;

    // Map background pixel to nearest EGA color index
    const bgR = bgData.data[p * 4];
    const bgG = bgData.data[p * 4 + 1];
    const bgB = bgData.data[p * 4 + 2];
    const bgEGA = rgbToEGA(bgR, bgG, bgB);

    // Apply BGI AND/OR: final = (background & mask) | sprite
    const finalColor = (bgEGA & maskColor) | spriteColor;

    const c = EGA_RGB[finalColor];
    bgData.data[p * 4] = c[0];
    bgData.data[p * 4 + 1] = c[1];
    bgData.data[p * 4 + 2] = c[2];
    bgData.data[p * 4 + 3] = 255;
  }

  ctx.putImageData(bgData, px, py);
}

function preRenderEnemySprites() {
  // Enemy sprites are rendered at draw time using AND/OR masking
}

function drawEnemySprite(tipo, frame, orientation, x, y) {
  if (!enemySprites) return;
  // Index: frame + norm*3 + orientation*6 + (tipo-1)*24
  const maskSpriteIdx = frame + 1*3 + orientation*6 + (tipo-1)*24;
  const colorSpriteIdx = frame + 0*3 + orientation*6 + (tipo-1)*24;
  const maskSprite = enemySprites.sprites[maskSpriteIdx];
  const colorSprite = enemySprites.sprites[colorSpriteIdx];
  if (!maskSprite || !colorSprite) return;

  const w = colorSprite.w, h = colorSprite.h;
  const px = x - B_SAncho + 2;
  const py = y - B_SAlto + 2;

  if (px < 0 || py < 0 || px + w > 640 || py + h > 350) return;

  const bgData = ctx.getImageData(px, py, w, h);

  for (let p = 0; p < w * h; p++) {
    const maskVal = maskSprite.pixels[p] & 0x0F;
    const spriteColor = colorSprite.pixels[p] & 0x0F;

    const bgR = bgData.data[p * 4];
    const bgG = bgData.data[p * 4 + 1];
    const bgB = bgData.data[p * 4 + 2];
    const bgEGA = rgbToEGA(bgR, bgG, bgB);

    const finalColor = (bgEGA & maskVal) | spriteColor;

    const c = EGA_RGB[finalColor];
    bgData.data[p * 4] = c[0];
    bgData.data[p * 4 + 1] = c[1];
    bgData.data[p * 4 + 2] = c[2];
    bgData.data[p * 4 + 3] = 255;
  }

  ctx.putImageData(bgData, px, py);
}

// ---- Keyboard handling ----
const keys = {};
let lastKey = null;
document.addEventListener('keydown', (e) => {
  keys[e.code] = true;
  lastKey = e.code;
  if (['ArrowUp','ArrowDown','ArrowLeft','ArrowRight','Space'].includes(e.code)) {
    e.preventDefault();
  }
});
document.addEventListener('keyup', (e) => {
  keys[e.code] = false;
});

function consumeKey() {
  const k = lastKey;
  lastKey = null;
  return k;
}

// ---- Node Classes ----
class Nodo {
  constructor(x, y, tipo, param) {
    this.x = x; this.y = y;
    this.estado = EXISTE;
    this.numIm = INODO;
    this.tipo = tipo; this.param = param;
  }
  draw() {
    if (!this.numIm || this.estado === NO_EXISTE) return;
    drawNodeSprite(this.numIm, VerdX(this.x), VerdY(this.y));
  }
  actuar() {}
  usar(angulo) { return this.estado === EXISTE ? NADA : PERDIO; }
  acercarse(angulo) { return NADA; }
  autorizarSalida(angulo) { return true; }
  autorizarLlegada(angulo) { return true; }
  salio() {}
  id() { return this.estado === EXISTE ? 'A' : ' '; }
}

class NodoInexistente extends Nodo {
  constructor(x, y) { super(x, y, ' ', 0); this.estado = NO_EXISTE; this.numIm = 0; }
  id() { return ' '; }
}

class NodoCom extends Nodo {
  constructor(x, y) { super(x, y, 'C', 0); this.numIm = INODO_COMIENZO; }
  id() { return 'C'; }
}

class NodoFin extends Nodo {
  constructor(x, y) {
    super(x, y, 'F', 0);
    this.numIm = INODO_FINAL;
    this.cuadro = 0; this.dir = 1;
  }
  usar(angulo) { return PASO_NIVEL; }
  actuar() {
    if (this.cuadro === 5) {
      if (this.numIm === INODO_FINAL) this.dir = 1;
      if (this.numIm === INODO_FINAL + 6) this.dir = -1;
      this.numIm += this.dir;
    }
    if (this.cuadro === 6) this.cuadro = 0;
    this.cuadro++;
  }
  id() { return 'F'; }
}

class NodoAs extends Nodo {
  constructor(x, y) { super(x, y, 'B', 0); this.numIm = INODO_AS; }
  autorizarLlegada(angulo) { return false; }
  acercarse(angulo) {
    if (angulo <= Math.abs(rot.vel / 2)) return PERDIO;
    return NADA;
  }
  usar(angulo) { return PERDIO; }
  id() { return 'B'; }
}

class NodoEn extends Nodo {
  constructor(x, y, energia) {
    super(x, y, 'K', energia);
    this.energia = energia * 180;
    this.numIm = INODO_EN;
    this.cuadro = 0;
  }
  draw() {
    if (this.estado === EXISTE) {
      this.numIm = INODO_EN + Math.floor(this.energia / 180);
    }
    if (this.estado === NO_EXISTE) return;
    drawNodeSprite(this.numIm, VerdX(this.x), VerdY(this.y));
  }
  usar(ang) {
    this.energia -= Math.abs(ang);
    if (this.energia <= 0 && this.estado === EXISTE) {
      this.estado = NO_ENERGIA;
      this.cuadro = 0;
      playSound(S_EXPLOSION);
    }
    if (this.energia <= 0) rot.puntos += 1;
    if (this.estado === NO_EXISTE) return PERDIO;
    return NADA;
  }
  actuar() {
    if (this.estado !== NO_ENERGIA) return;
    if (!(this.cuadro % 8)) {
      this.numIm = Math.floor(this.cuadro / 8) + INODO_EN_SIN_EN;
      if (this.numIm === INODO_EN_SIN_EN + 8) this.estado = NO_EXISTE;
      if (this.numIm >= INODO_EN_SIN_EN + 7) this.numIm = 0;
      if (this.numIm === INODO_EN_SIN_EN + 6) playSound(S_FIN_NODO_EN);
    }
    this.cuadro++;
  }
  id() { return 'K'; }
}

class Nodo1Paso extends Nodo {
  constructor(x, y, dir) {
    super(x, y, 'L', dir);
    this.dirVal = dir;
    this.numIm = INODO_1PASO + dir - 1;
  }
  autorizarSalida(angulo) {
    return Math.abs(angulo - (this.dirVal - 1) * 90) < 15;
  }
  id() { return 'L'; }
}

class NodoVida extends Nodo {
  constructor(x, y) {
    super(x, y, 'D', 0);
    this.numIm = INODO_VIDA;
    this.usada = false;
  }
  usar(angulo) {
    if (this.usada) return NADA;
    rot.vidas++;
    this.numIm = INODO;
    this.usada = true;
    return NADA;
  }
  id() { return 'D'; }
}

class NodoPuntos extends Nodo {
  constructor(x, y, p) {
    super(x, y, 'M', p);
    this.puntosVal = p * 100;
    this.numIm = INODO_PUNTOS;
  }
  usar(angulo) {
    if (!this.puntosVal) return NADA;
    rot.puntos += this.puntosVal;
    this.numIm = INODO;
    this.puntosVal = 0;
    return NADA;
  }
  id() { return 'M'; }
}

class NodoPildTC extends Nodo {
  constructor(x, y, dur) {
    super(x, y, 'O', dur);
    this.durac = dur;
    this.numIm = INODO_PILDTC;
    this.usado = 0;
    this.tFinal = 0;
    this.savedNodes = [];
  }
  usar(angulo) {
    if (this.usado) return NADA;
    this.usado = 1;
    this.tFinal = this.durac + gameTime;
    for (let cy = 0; cy < NODOS_Y; cy++) {
      for (let cx = 0; cx < NODOS_X; cx++) {
        if (cx === this.x && cy === this.y) continue;
        if (nodos[cx][cy].estado === NO_EXISTE) continue;
        const nid = nodos[cx][cy].id();
        if (nid === 'F' || nid === 'C') continue;
        this.savedNodes.push({ x: cx, y: cy, node: nodos[cx][cy] });
        nodos[cx][cy] = new Nodo(cx, cy, 'A', 0);
      }
    }
    return NADA;
  }
  actuar() {
    if (this.usado === 1) {
      pillTimerPercent = ((this.durac - this.tFinal + gameTime) * 100 / this.durac);
    }
    if (this.usado !== 1 || gameTime < this.tFinal - 3) return;
    if (gameTime < this.tFinal) {
      playSound(S_AVISO_FIN_PILDTC);
      return;
    }
    this.usado = 2;
    playSound(S_FIN_PILDTC);
    pillTimerPercent = -1;
    for (const saved of this.savedNodes) {
      nodos[saved.x][saved.y] = saved.node;
    }
    this.numIm = INODO;
  }
  id() { return 'O'; }
}

class NodoPildMB extends Nodo {
  constructor(x, y, dur) {
    super(x, y, 'N', dur);
    this.durac = dur;
    this.numIm = INODO_PILDMB;
    this.usado = 0;
    this.tFinal = 0;
  }
  usar(angulo) {
    if (this.usado) return NADA;
    rot.pildMBActiva = true;
    this.usado = 1;
    this.tFinal = this.durac + gameTime;
    return NADA;
  }
  actuar() {
    if (this.usado === 1) {
      pillTimerPercent = ((this.durac - this.tFinal + gameTime) * 100 / this.durac);
    }
    if (this.usado !== 1 || gameTime < this.tFinal - 3) return;
    if (gameTime < this.tFinal) {
      playSound(S_AVISO_FIN_PILDTC);
      return;
    }
    this.numIm = INODO;
    this.usado = 2;
    rot.pildMBActiva = false;
    playSound(S_FIN_PILDTC);
    pillTimerPercent = -1;
  }
  id() { return 'N'; }
}

function createNode(x, y, tipo, param) {
  switch (tipo) {
    case ' ': return new NodoInexistente(x, y);
    case 'A': return new Nodo(x, y, 'A', 0);
    case 'B': return new NodoAs(x, y);
    case 'C': return new NodoCom(x, y);
    case 'D': return new NodoVida(x, y);
    case 'F': return new NodoFin(x, y);
    case 'K': return new NodoEn(x, y, param);
    case 'L': return new Nodo1Paso(x, y, param);
    case 'M': return new NodoPuntos(x, y, param);
    case 'O': return new NodoPildTC(x, y, param);
    case 'N': return new NodoPildMB(x, y, param);
    default: return new NodoInexistente(x, y);
  }
}

// ---- Player (Rot) ----
let rot = {
  x: 0, y: 0,
  angulo: 0, vel: 12,
  vidas: 3, puntos: 0,
  accion: ACT_NORMAL,
  quierePasar: false,
  pildMBActiva: false,
  auxiliar: 0, auxiliar2: 0,
  targetX: 0, targetY: 0,
};

function initRot() {
  rot.vel = 12;
  rot.quierePasar = false;
  rot.puntos = 0;
  rot.vidas = 3;
  rot.accion = ACT_NORMAL;
  rot.pildMBActiva = false;
}

function initRotForLevel(cx, cy) {
  rot.angulo = 0;
  rot.accion = ACT_APARECER_INICIAR;
  rot.x = cx;
  rot.y = cy;
}

function drawRot() {
  const seno = SIN[rot.angulo] / 256.0;
  const coseno = COS[rot.angulo] / 256.0;
  const cos2 = -seno, sen2 = coseno;
  const vx = VerdX(rot.x);
  const vy = VerdY(rot.y);
  let ancho_x = ROT_ANCHO_X, ancho_y = ROT_ANCHO_Y;
  let alto_x = ROT_ALTO_X, alto_y = ROT_ALTO_Y;

  switch (rot.accion) {
    case ACT_PASAR_INICIAR:
      rot.auxiliar = rot.angulo % 180 ? ROT_ANCHO_Y : ROT_ANCHO_X;
      rot.accion = ACT_PASAR_ANGOSTAR;
      // fall through
    case ACT_PASAR_ANGOSTAR:
      ancho_x = ancho_y = rot.auxiliar;
      rot.auxiliar -= ROT_VEL_ANGOST;
      if (rot.auxiliar <= 1) rot.accion = ACT_PASAR;
      break;
    case ACT_PASAR:
      ancho_x = ancho_y = rot.auxiliar;
      rot.auxiliar2 = rot.angulo % 180 ? ROT_ANCHO_Y : ROT_ANCHO_X;
      rot.accion = ACT_PASAR_ENSANCHAR;
      break;
    case ACT_PASAR_ENSANCHAR:
      ancho_x = ancho_y = rot.auxiliar;
      rot.auxiliar += ROT_VEL_ANGOST;
      if (rot.auxiliar >= rot.auxiliar2) rot.accion = ACT_NORMAL;
      break;
    case ACT_APARECER_INICIAR:
      rot.auxiliar = 1;
      playSound(S_APARECER);
      rot.auxiliar2 = rot.angulo % 180 ? alto_y : alto_x;
      rot.accion = ACT_APARECER_CRECER;
      // fall through
    case ACT_APARECER_CRECER:
      alto_x = alto_y = ++rot.auxiliar;
      if (rot.auxiliar >= rot.auxiliar2) { rot.accion = ACT_APARECER_ESPERA; rot.auxiliar = 10; }
      break;
    case ACT_APARECER_ESPERA:
      rot.auxiliar--;
      if (!rot.auxiliar) rot.accion = ACT_NORMAL;
      break;
    case ACT_DESAPARECER_INICIAR:
    case ACT_PERDER_INICIAR:
      rot.auxiliar = rot.angulo % 180 ? alto_y : alto_x;
      rot.accion = (rot.accion === ACT_DESAPARECER_INICIAR) ? ACT_DESAPARECER_ACHICAR : ACT_PERDER_ACHICAR;
      // fall through
    case ACT_DESAPARECER_ACHICAR:
    case ACT_PERDER_ACHICAR:
      alto_x = alto_y = --rot.auxiliar;
      if (rot.auxiliar <= 0) {
        rot.accion = (rot.accion === ACT_DESAPARECER_ACHICAR) ? ACT_DESAPARECER_FIN : ACT_PERDER_FIN;
      }
      break;
  }

  if (alto_x <= 0 || alto_y <= 0) return;

  // Draw diamond shape (2 triangles)
  // Triangle 1: back half (LIGHTGRAY)
  ctx.beginPath();
  ctx.moveTo(vx, vy);
  ctx.lineTo(vx - ancho_x / 2 * cos2, vy + ancho_y / 2 * sen2);
  ctx.lineTo(vx + alto_x * coseno, vy - alto_y * seno);
  ctx.closePath();
  ctx.fillStyle = EGA[LIGHTGRAY];
  ctx.fill();

  // Triangle 2: front half (WHITE)
  ctx.beginPath();
  ctx.moveTo(vx + ancho_x / 2 * cos2, vy - ancho_y / 2 * sen2);
  ctx.lineTo(vx, vy);
  ctx.lineTo(vx + alto_x * coseno, vy - alto_y * seno);
  ctx.closePath();
  ctx.fillStyle = EGA[WHITE];
  ctx.fill();
}

function puntoAdentro(x2, y2) {
  if (Math.abs(rot.angulo % 90) < 5) return false;
  const x0 = VerdX(rot.x);
  const y0 = VerdY(rot.y);
  const x1 = x0 + ROT_ALTO_X * COS[rot.angulo] / 256;
  const y1 = y0 - ROT_ALTO_Y * SIN[rot.angulo] / 256;
  if (x0 === x1 || y0 === y1) return false;
  const a = (y0 - y1) / (x0 - x1);
  const b = y1 - a * x1;
  const x3 = (y2 + x2 / a - b) * (a / (a * a + 1));
  const y3 = a * x3 + b;
  if (x3 < Math.min(x0, x1) || x3 > Math.max(x0, x1)) return false;
  const DPar = Math.hypot(x1 - x3, y1 - y3);
  const DPerp = Math.hypot(x2 - x3, y2 - y3);
  const p1 = DPerp ? DPar / DPerp : 1000;
  const p2 = ROT_ALTO_X / (ROT_ANCHO_X / 2.0);
  return p1 >= p2;
}

function posibleChoque(xp, yp) {
  return xp >= VerdX(rot.x - 1) && xp <= VerdX(rot.x + 1) &&
         yp >= VerdY(rot.y - 1) && yp <= VerdY(rot.y + 1);
}

// ---- Enemy (Bicho) ----
class Bicho {
  constructor(info) {
    this.info = { ...info };
    this.x = (info.inix + 0.5) * DIST_NODOX + MARGEN_X;
    this.y = (info.iniy + 0.5) * DIST_NODOY + MARGEN_Y;
    this.xreal = this.x;
    this.yreal = this.y;
    this.dx = 1; this.dy = 0;
    this.activo = false;
    this.existe = true;
    this.numIm = 0;
    this.contCuad = 0;
    switch (info.tipo) {
      case 1: this.vel = 1.9; this.tCambioCuad = 3; break;
      case 2: this.vel = 1.0; this.tCambioCuad = 4; break;
      case 3: this.vel = 0.3; this.tCambioCuad = 6; break;
      case 4: this.vel = 0.6; this.tCambioCuad = 5; break;
      default: this.vel = 1; this.tCambioCuad = 4;
    }
  }

  orientacion() {
    if (this.dy > 0) return 0;
    if (this.dx > 0) return 1;
    if (this.dy < 0) return 2;
    if (this.dx < 0) return 3;
    return 0;
  }

  draw() {
    if (!this.activo) return;
    drawEnemySprite(this.info.tipo, this.numIm, this.orientacion(),
                    Math.round(this.x), Math.round(this.y));
  }

  verificarROT() {
    if (!posibleChoque(Math.round(this.x), Math.round(this.y))) return NADA;
    const pts = [
      [this.x, this.y],
      [this.x - 6, this.y - 6], [this.x - 6, this.y + 6],
      [this.x + 6, this.y - 6], [this.x + 6, this.y + 6],
    ];
    for (const [px, py] of pts) {
      if (puntoAdentro(px, py)) return PERDIO;
    }
    return NADA;
  }

  actuar() {
    this.contCuad++;
    if (this.contCuad >= this.tCambioCuad) {
      this.numIm = (this.numIm + 1) % 3;
      this.contCuad = 0;
    }

    if (gameTime >= this.info.taparic + this.info.duracion - 3) return this.verificarROT();

    this.xreal += this.dx * this.vel;
    this.yreal += this.dy * this.vel;
    this.x = Math.round(this.xreal);
    this.y = Math.round(this.yreal);

    // Check if at a node center - change direction
    const xMod = (this.x - MARGEN_X + DIST_NODOX) % DIST_NODOX;
    const yMod = (this.y - MARGEN_Y + DIST_NODOY) % DIST_NODOY;
    if (xMod === Math.round(DIST_NODOX / 2) && yMod === Math.round(DIST_NODOY / 2)) {
      if (Math.random() * this.info.intel < 100) {
        switch (Math.floor(Math.random() * 4)) {
          case 0: this.dx = 1; this.dy = 0; break;
          case 1: this.dx = 0; this.dy = 1; break;
          case 2: this.dx = -1; this.dy = 0; break;
          case 3: this.dx = 0; this.dy = -1; break;
        }
      } else {
        if (Math.abs(this.x - VerdX(rot.x)) > Math.abs(this.y - VerdY(rot.y))) {
          this.dx = this.x < VerdX(rot.x) ? 1 : -1;
          this.dy = 0;
        } else {
          this.dy = this.y < VerdY(rot.y) ? 1 : -1;
          this.dx = 0;
        }
      }
    }

    // Boundary bouncing
    if ((this.x - MARGEN_X) <= -(DIST_NODOX / 2) && this.dx < 0) this.dx = -this.dx;
    if ((this.y - MARGEN_Y) <= -(DIST_NODOY / 2) && this.dy < 0) this.dy = -this.dy;
    if (this.x >= (NODOS_X + 1) * DIST_NODOX && this.dx > 0) this.dx = -this.dx;
    if (this.y >= (NODOS_Y + 1) * DIST_NODOY && this.dy > 0) this.dy = -this.dy;

    const aux = this.verificarROT();
    if (!rot.pildMBActiva) return aux;
    if (aux === NADA) return NADA;
    // Killed enemy with pill
    this.info.duracion = 0;
    rot.puntos += 10000;
    return NADA;
  }

  verificarTiempo() {
    if (gameTime >= this.info.taparic && !this.activo) {
      this.activo = true;
      playSound(S_APARECER);
    }
    if (gameTime >= this.info.taparic + this.info.duracion) {
      this.activo = false;
      this.existe = false;
    }
  }
}

// ---- Level State ----
let nodos = [];
let bichos = [];
let gameTime = 0;
let gameStartTime = 0;
let levelData = null;
let pillTimerPercent = -1;
let currentLevelIdx = 0;

let gameState = 'menu';
let nivelInicial = 1;
let maxNivel = 1;

function loadLevel(levelIdx) {
  if (levelIdx >= levels.length) return false;
  levelData = levels[levelIdx];
  currentLevelIdx = levelIdx;

  nodos = [];
  for (let x = 0; x < NODOS_X; x++) {
    nodos[x] = [];
    for (let y = 0; y < NODOS_Y; y++) {
      const cell = levelData.grid[y][x];
      nodos[x][y] = createNode(x, y, cell.type, cell.param);
      if (cell.type === 'C') { levelData.cx = x; levelData.cy = y; }
    }
  }

  bichos = levelData.enemies.map(e => new Bicho(e));
  pillTimerPercent = -1;
  gameStartTime = performance.now();
  return true;
}

function findStartLevel(majorLevel) {
  for (let i = 0; i < levels.length; i++) {
    if (levels[i].majorLevel === majorLevel && levels[i].screenNum === 1) return i;
  }
  return 0;
}

function verVecinos(x, y, a) {
  if (a > 45 && a <= 135) {
    if (!y) return NADA;
    return nodos[x][y - 1].acercarse(Math.abs(90 - a));
  }
  if (a > 135 && a <= 225) {
    if (!x) return NADA;
    return nodos[x - 1][y].acercarse(Math.abs(180 - a));
  }
  if (a > 225 && a <= 315) {
    if (y >= NODOS_Y - 1) return NADA;
    return nodos[x][y + 1].acercarse(Math.abs(270 - a));
  }
  if (x >= NODOS_X - 1) return NADA;
  return nodos[x + 1][y].acercarse(Math.abs((a + 45) % 360 - 45));
}

// ---- Drawing helpers ----
function drawBackground() {
  // Draw the laptop frame (I0)
  if (images.i0) {
    ctx.drawImage(images.i0, 0, 0);
  } else {
    ctx.fillStyle = EGA[DARKGRAY];
    ctx.fillRect(0, 0, 640, 350);
    ctx.fillStyle = EGA[LIGHTGRAY];
    ctx.fillRect(20, 290, 600, 50);
  }

  // Draw level background inside the screen area (20,15)
  // Original formula: (Nivel.Nro - 256) / 512 (integer division in C)
  const bgIdx = levelData ? Math.floor((levelData.nro - 256) / 512) : 0;
  const bgName = `f${Math.min(Math.max(bgIdx, 0), 4)}`;
  if (images[bgName]) {
    ctx.drawImage(images[bgName], 20, 15);
  } else {
    ctx.fillStyle = EGA[CYAN];
    ctx.fillRect(20, 15, 600, 270);
  }
}

function drawUI() {
  // Lives area (bottom bar of laptop)
  ctx.fillStyle = EGA[LIGHTGRAY];
  ctx.fillRect(21, 316, 108, 23);
  if (rot.vidas > 5) {
    ctx.fillStyle = EGA[WHITE];
    ctx.font = '14px monospace';
    ctx.textAlign = 'left';
    ctx.fillText(`${rot.vidas} x`, 31, 332);
    drawLifeIcon(3 * 20 + 15 + 21, 21 + 316);
  } else {
    for (let i = 0; i < rot.vidas; i++) {
      drawLifeIcon(i * 20 + 15 + 21, 21 + 316);
    }
  }

  // Points
  ctx.fillStyle = EGA[LIGHTGRAY];
  ctx.fillRect(172, 316, 67, 23);
  ctx.fillStyle = EGA[WHITE];
  ctx.font = '12px monospace';
  ctx.textAlign = 'right';
  ctx.fillText(rot.puntos.toString(), 172 + 63, 316 + 18);

  // Level
  ctx.fillStyle = EGA[LIGHTGRAY];
  ctx.fillRect(272, 316, 67, 23);
  ctx.fillStyle = EGA[WHITE];
  ctx.font = '14px monospace';
  ctx.textAlign = 'center';
  if (levelData) {
    ctx.fillText(`${levelData.majorLevel}.${levelData.screenNum}`, 272 + 33, 316 + 18);
  }

  // Pill timer
  ctx.fillStyle = EGA[BROWN];
  ctx.fillRect(364, 316, 109, 23);
  if (pillTimerPercent >= 0) {
    const fillW = Math.floor(Math.max(0, Math.min(100, pillTimerPercent)) * 109 / 100);
    ctx.fillStyle = EGA[LIGHTGREEN];
    ctx.fillRect(364, 316, fillW, 23);
    ctx.fillStyle = EGA[LIGHTRED];
    ctx.fillRect(364 + fillW, 316, 109 - fillW, 23);
  }
}

function drawLifeIcon(vx, vy) {
  ctx.beginPath();
  ctx.moveTo(vx, vy);
  ctx.lineTo(vx - 5, vy);
  ctx.lineTo(vx, vy - 18);
  ctx.closePath();
  ctx.fillStyle = EGA[RED];
  ctx.fill();

  ctx.beginPath();
  ctx.moveTo(vx + 5, vy);
  ctx.lineTo(vx, vy);
  ctx.lineTo(vx, vy - 18);
  ctx.closePath();
  ctx.fillStyle = EGA[WHITE];
  ctx.fill();
}

function drawAllNodes() {
  for (let y = 0; y < NODOS_Y; y++) {
    for (let x = 0; x < NODOS_X; x++) {
      nodos[x][y].draw();
    }
  }
}

function drawAllEnemies() {
  for (const b of bichos) {
    if (b.activo) b.draw();
  }
}

// ---- Menu ----
let menuOption = 0;
const menuLabels = ['Comenzar', 'Desde: 1.1', 'Sonido: SI', 'Records', 'Acerca de...'];

function drawMenu() {
  if (images.i4) {
    ctx.drawImage(images.i4, 0, 0);
  } else {
    ctx.fillStyle = EGA[CYAN];
    ctx.fillRect(0, 0, 640, 350);
  }

  // Menu panel
  ctx.fillStyle = EGA[LIGHTGRAY];
  ctx.fillRect(241, 81, 160, 150);
  // 3D borders
  ctx.strokeStyle = EGA[WHITE];
  ctx.strokeRect(238, 78, 166, 156);
  ctx.strokeStyle = EGA[DARKGRAY];
  ctx.strokeRect(240, 80, 162, 152);

  // Title bar
  ctx.fillStyle = EGA[LIGHTBLUE];
  ctx.fillRect(245, 85, 152, 20);
  ctx.strokeStyle = EGA[WHITE];
  ctx.strokeRect(244, 84, 154, 22);
  ctx.fillStyle = EGA[WHITE];
  ctx.font = '12px monospace';
  ctx.textAlign = 'center';
  ctx.fillText('Menu Principal', 320, 99);

  // Update labels
  menuLabels[1] = `Desde: ${nivelInicial}.1`;
  menuLabels[2] = `Sonido: ${soundEnabled ? 'SI' : 'NO'}`;

  for (let i = 0; i < menuLabels.length; i++) {
    const y = i * 17 + 110;
    const sel = i === menuOption;

    ctx.fillStyle = sel ? EGA[CYAN] : EGA[LIGHTGRAY];
    ctx.fillRect(250, y, 140, 14);

    // 3D button
    ctx.strokeStyle = sel ? EGA[DARKGRAY] : EGA[WHITE];
    ctx.beginPath();
    ctx.moveTo(250, y + 14); ctx.lineTo(250, y); ctx.lineTo(390, y);
    ctx.stroke();
    ctx.strokeStyle = sel ? EGA[WHITE] : EGA[DARKGRAY];
    ctx.beginPath();
    ctx.moveTo(390, y); ctx.lineTo(390, y + 14); ctx.lineTo(250, y + 14);
    ctx.stroke();

    ctx.fillStyle = sel ? EGA[YELLOW] : EGA[LIGHTGREEN];
    ctx.font = '11px monospace';
    ctx.textAlign = 'center';
    ctx.fillText(menuLabels[i], 320, y + 11);
  }
}

function drawRecords() {
  if (images.i2) {
    ctx.drawImage(images.i2, 0, 0);
  } else {
    ctx.fillStyle = EGA[BLACK];
    ctx.fillRect(0, 0, 640, 350);
  }

  const records = JSON.parse(localStorage.getItem('giroloco_records') || '[]');
  ctx.font = '14px monospace';
  ctx.textAlign = 'left';
  for (let i = 0; i < records.length && i < 10; i++) {
    ctx.fillStyle = i === 0 ? EGA[LIGHTBLUE] : EGA[BLUE];
    const y = i * 18 + 92;
    ctx.fillText(records[i].name.padEnd(11), 190, y);
    ctx.fillText(records[i].points.toString().padStart(8), 320, y);
    const lev = `${records[i].level >> 8}.${records[i].level & 0xFF}`;
    ctx.fillText(lev, 440, y);
  }

  ctx.fillStyle = EGA[WHITE];
  ctx.font = '12px monospace';
  ctx.textAlign = 'center';
  ctx.fillText('Presione una tecla para volver', 320, 330);
}

function drawAbout() {
  if (images.i5) {
    ctx.drawImage(images.i5, 0, 0);
  } else {
    ctx.fillStyle = EGA[BLACK];
    ctx.fillRect(0, 0, 640, 350);
  }

  ctx.fillStyle = EGA[BROWN];
  ctx.fillRect(20, 80, 280, 140);
  ctx.fillStyle = EGA[BLUE];
  ctx.fillRect(320, 80, 280, 140);

  ctx.font = '11px monospace';
  ctx.textAlign = 'left';

  ctx.fillStyle = EGA[YELLOW];
  ctx.fillText('IDEA, DISENO Y PROGRAMACION:', 40, 100);
  ctx.fillStyle = EGA[WHITE];
  ctx.fillText('Alejandro Marcu', 100, 120);

  ctx.fillStyle = EGA[YELLOW];
  ctx.fillText('COLABORARON:', 40, 145);
  ctx.fillStyle = EGA[WHITE];
  ctx.fillText('Edgardo Garelik', 100, 165);
  ctx.fillText('Sebastian Sejzer', 100, 180);
  ctx.fillText('Pablo Zimmerman', 100, 195);
  ctx.fillText('Alejandro Zylberman', 100, 210);

  ctx.fillStyle = EGA[WHITE];
  ctx.fillText('Giroloco 1997', 400, 110);
  ctx.fillText('Copyright', 400, 130);
  ctx.fillStyle = EGA[LIGHTGRAY];
  ctx.fillText('Browser port 2026', 360, 170);

  ctx.fillStyle = EGA[WHITE];
  ctx.font = '12px monospace';
  ctx.textAlign = 'center';
  ctx.fillText('Presione una tecla para volver', 320, 330);
}

function drawGameOver() {
  ctx.fillStyle = 'rgba(0,0,0,0.75)';
  ctx.fillRect(0, 0, 640, 350);

  ctx.fillStyle = EGA[RED];
  ctx.font = 'bold 36px monospace';
  ctx.textAlign = 'center';
  ctx.fillText('GAME OVER', 320, 140);

  ctx.fillStyle = EGA[WHITE];
  ctx.font = '16px monospace';
  ctx.fillText(`Puntos: ${rot.puntos}`, 320, 185);

  ctx.fillStyle = EGA[LIGHTGRAY];
  ctx.font = '12px monospace';
  ctx.fillText('Presione una tecla para continuar', 320, 230);
}

function drawWin() {
  if (images.i6) {
    ctx.drawImage(images.i6, 0, 0);
  } else {
    ctx.fillStyle = EGA[BLUE];
    ctx.fillRect(0, 0, 640, 350);
  }

  ctx.fillStyle = EGA[YELLOW];
  ctx.font = 'bold 28px monospace';
  ctx.textAlign = 'center';
  ctx.fillText('GANASTE!', 320, 60);

  ctx.fillStyle = EGA[WHITE];
  ctx.font = '16px monospace';
  ctx.fillText(`Puntos: ${rot.puntos}`, 320, 190);

  ctx.fillStyle = EGA[LIGHTGRAY];
  ctx.font = '12px monospace';
  ctx.fillText('Presione una tecla para continuar', 320, 240);
}

function drawPaused() {
  ctx.fillStyle = 'rgba(0,0,0,0.5)';
  ctx.fillRect(0, 0, 640, 350);

  const cx = 320, cy = 150;
  if (images.i1) {
    ctx.drawImage(images.i1, cx - 100, cy - 50);
  } else {
    ctx.fillStyle = EGA[LIGHTGRAY];
    ctx.fillRect(cx - 100, cy - 50, 200, 100);
    ctx.fillStyle = EGA[BLACK];
    ctx.font = '14px monospace';
    ctx.textAlign = 'center';
    ctx.fillText('Salir del GIROLOCO?', cx, cy - 10);
    ctx.fillText('S = Si, N = No', cx, cy + 20);
  }
}

// ---- Records Management ----
function addRecord(name, points, level) {
  const records = JSON.parse(localStorage.getItem('giroloco_records') || '[]');
  records.push({ name, points, level });
  records.sort((a, b) => b.points - a.points);
  if (records.length > 10) records.length = 10;
  localStorage.setItem('giroloco_records', JSON.stringify(records));
  const majorLev = level >> 8;
  if (majorLev > maxNivel) {
    maxNivel = majorLev;
    localStorage.setItem('giroloco_maxNivel', maxNivel.toString());
  }
}

function shouldAddRecord(points) {
  const records = JSON.parse(localStorage.getItem('giroloco_records') || '[]');
  return records.length < 10 || points > records[records.length - 1].points;
}

// ---- Name Input ----
let nameInput = '';
let nameInputActive = false;
let nameInputCallback = null;

function startNameInput(callback) {
  nameInput = '';
  nameInputActive = true;
  nameInputCallback = callback;
}

function drawNameInput() {
  // Dark overlay
  ctx.fillStyle = 'rgba(0,0,0,0.6)';
  ctx.fillRect(0, 0, 640, 350);

  if (images.i3) {
    ctx.drawImage(images.i3, 200, 80);
  } else {
    ctx.fillStyle = EGA[LIGHTGRAY];
    ctx.fillRect(200, 80, 240, 150);
    ctx.fillStyle = EGA[LIGHTBLUE];
    ctx.fillRect(210, 90, 220, 25);
    ctx.fillStyle = EGA[WHITE];
    ctx.font = '14px monospace';
    ctx.textAlign = 'center';
    ctx.fillText('Nuevo Record!', 320, 108);
  }

  ctx.fillStyle = EGA[LIGHTGRAY];
  ctx.fillRect(228, 198, 175, 16);
  ctx.fillStyle = EGA[BLACK];
  ctx.font = '14px monospace';
  ctx.textAlign = 'left';
  ctx.fillText(nameInput + '_', 230, 212);
}

function handleNameInput(key) {
  if (!nameInputActive) return;
  if (key === 'Enter') {
    nameInputActive = false;
    if (nameInputCallback) nameInputCallback(nameInput || 'Player');
    return;
  }
  if (key === 'Backspace') {
    nameInput = nameInput.slice(0, -1);
    return;
  }
  if (key && key.startsWith('Key') && nameInput.length < 10) {
    const letter = key.replace('Key', '');
    nameInput += keys['ShiftLeft'] || keys['ShiftRight'] ? letter : letter.toLowerCase();
  } else if (key && key.startsWith('Digit') && nameInput.length < 10) {
    nameInput += key.replace('Digit', '');
  } else if (key === 'Space' && nameInput.length < 10) {
    nameInput += ' ';
  }
}

// ---- Main game loop ----
let lastFrameTime = 0;
const FRAME_INTERVAL = 1000 / 30;

function gameLoop(timestamp) {
  requestAnimationFrame(gameLoop);

  if (!assetsLoaded) {
    ctx.fillStyle = EGA[BLACK];
    ctx.fillRect(0, 0, 640, 350);
    ctx.fillStyle = EGA[CYAN];
    ctx.font = '20px monospace';
    ctx.textAlign = 'center';
    ctx.fillText('Cargando...', 320, 175);
    return;
  }

  const delta = timestamp - lastFrameTime;
  if (delta < FRAME_INTERVAL) return;
  lastFrameTime = timestamp;

  const key = consumeKey();

  switch (gameState) {
    case 'menu':
      updateMenu(key);
      drawMenu();
      break;
    case 'playing':
      updateGame(key);
      drawGame();
      break;
    case 'paused':
      drawGame();
      drawPaused();
      if (key === 'KeyS' || key === 'KeyY') {
        gameState = 'menu'; menuOption = 0;
      } else if (key === 'KeyN' || key === 'Escape') {
        gameState = 'playing';
        gameStartTime = performance.now() - gameTime * 1000;
      }
      break;
    case 'gameover':
      drawGameOver();
      if (key) {
        if (shouldAddRecord(rot.puntos)) {
          gameState = 'enterName';
          startNameInput(name => {
            addRecord(name, rot.puntos, levelData ? levelData.nro : 0);
            gameState = 'records';
          });
        } else {
          gameState = 'menu'; menuOption = 0;
        }
      }
      break;
    case 'won':
      drawWin();
      if (key) {
        if (shouldAddRecord(rot.puntos)) {
          gameState = 'enterName';
          startNameInput(name => {
            addRecord(name, rot.puntos, levelData ? levelData.nro : 0);
            gameState = 'records';
          });
        } else {
          gameState = 'menu'; menuOption = 0;
        }
      }
      break;
    case 'enterName':
      drawNameInput();
      handleNameInput(key);
      break;
    case 'records':
      drawRecords();
      if (key) { gameState = 'menu'; menuOption = 0; }
      break;
    case 'about':
      drawAbout();
      if (key) { gameState = 'menu'; menuOption = 0; }
      break;
  }
}

function updateMenu(key) {
  if (key === 'ArrowUp') {
    menuOption = (menuOption - 1 + menuLabels.length) % menuLabels.length;
  } else if (key === 'ArrowDown') {
    menuOption = (menuOption + 1) % menuLabels.length;
  } else if (key === 'Enter') {
    switch (menuOption) {
      case 0: startGame(); break;
      case 1:
        nivelInicial++;
        if (nivelInicial > maxNivel) nivelInicial = 1;
        break;
      case 2: soundEnabled = !soundEnabled; break;
      case 3: gameState = 'records'; break;
      case 4: gameState = 'about'; break;
    }
  }
}

function startGame() {
  initRot();
  const startIdx = findStartLevel(nivelInicial);
  if (!loadLevel(startIdx)) return;
  initRotForLevel(levelData.cx, levelData.cy);
  gameState = 'playing';
}

function updateGame(key) {
  gameTime = Math.floor((performance.now() - gameStartTime) / 1000);
  let res = NADA;

  if (key === 'Escape') { gameState = 'paused'; return; }

  // Update enemies
  for (const b of bichos) { if (b.existe) b.verificarTiempo(); }
  for (const b of bichos) {
    if (b.activo) {
      const r = b.actuar();
      if (r !== NADA && res === NADA) res = r;
    }
  }

  // Update player movement
  const moveRes = updateRot();
  if (moveRes !== NADA && res === NADA) res = moveRes;

  // Update nodes
  for (let y = 0; y < NODOS_Y; y++)
    for (let x = 0; x < NODOS_X; x++)
      nodos[x][y].actuar();

  // Current node interaction
  if (rot.accion === ACT_NORMAL || rot.accion === ACT_PASAR_ENSANCHAR) {
    const useRes = nodos[rot.x][rot.y].usar(rot.vel);
    if (useRes !== NADA && res === NADA) res = useRes;
  }

  // Check neighbor threats
  const vecRes = verVecinos(rot.x, rot.y, rot.angulo);
  if (vecRes !== NADA && res === NADA) res = vecRes;

  // Handle results
  if (res === PERDIO && rot.accion === ACT_NORMAL) {
    rot.vidas--;
    playSound(S_PERDIO_VIDA);
    rot.accion = ACT_PERDER_INICIAR;
  }
  if (res === PASO_NIVEL && rot.accion === ACT_NORMAL) {
    playSound(S_APARECER);
    rot.accion = ACT_DESAPARECER_INICIAR;
  }

  if (rot.accion === ACT_PERDER_FIN) {
    rot.accion = ACT_NORMAL;
    if (rot.vidas <= 0) { gameState = 'gameover'; return; }
    // Restart current level
    if (!loadLevel(currentLevelIdx)) { gameState = 'gameover'; return; }
    initRotForLevel(levelData.cx, levelData.cy);
  }

  if (rot.accion === ACT_DESAPARECER_FIN) {
    rot.accion = ACT_NORMAL;
    rot.puntos += 1000;
    currentLevelIdx++;
    if (currentLevelIdx >= levels.length) {
      rot.puntos += 20000;
      gameState = 'won';
      return;
    }
    if (!loadLevel(currentLevelIdx)) {
      rot.puntos += 20000;
      gameState = 'won';
      return;
    }
    initRotForLevel(levelData.cx, levelData.cy);
  }
}

function updateRot() {
  // Handle PASAR midpoint transition before anything else
  if (rot.accion === ACT_PASAR) {
    nodos[rot.x][rot.y].salio();
    rot.angulo = (rot.angulo + 540) % 360;
    rot.x = rot.targetX;
    rot.y = rot.targetY;
    rot.quierePasar = false;
  }

  if (rot.accion !== ACT_NORMAL) return NADA;

  // Handle rotation
  rot.angulo = ((rot.angulo + rot.vel) % 360 + 360) % 360;

  // Read input
  if (keys['ArrowLeft']) { rot.vel = Math.abs(rot.vel); rot.quierePasar = false; }
  if (keys['ArrowRight']) { rot.vel = -Math.abs(rot.vel); rot.quierePasar = false; }

  if (keys['Space'] && !rot.quierePasar) {
    rot.quierePasar = AngPar(rot.angulo, 0) || AngPar(rot.angulo, 90) ||
                      AngPar(rot.angulo, 180) || AngPar(rot.angulo, 270);
  }

  // Try to move to next node
  if (rot.quierePasar) {
    let xx = rot.x, yy = rot.y;
    if (Math.abs(rot.angulo - 0) <= Math.abs(rot.vel)) xx++;
    if (Math.abs(rot.angulo - 90) <= Math.abs(rot.vel)) yy--;
    if (Math.abs(rot.angulo - 180) <= Math.abs(rot.vel)) xx--;
    if (Math.abs(rot.angulo - 270) <= Math.abs(rot.vel)) yy++;

    if ((yy !== rot.y || xx !== rot.x) &&
        xx >= 0 && xx < NODOS_X && yy >= 0 && yy < NODOS_Y &&
        nodos[xx][yy].estado === EXISTE) {
      if (nodos[rot.x][rot.y].autorizarSalida(rot.angulo) &&
          nodos[xx][yy].autorizarLlegada(rot.angulo)) {
        rot.angulo = Math.round((rot.angulo + Math.abs(rot.vel)) / 90) * 90 % 360;
        rot.accion = ACT_PASAR_INICIAR;
        rot.targetX = xx;
        rot.targetY = yy;
        playSound(S_PASO_NODO);
      }
    }
  }

  return NADA;
}

function drawGame() {
  drawBackground();
  drawAllNodes();
  drawAllEnemies();
  drawRot();
  drawUI();
}

// ---- Initialize ----
function init() {
  maxNivel = parseInt(localStorage.getItem('giroloco_maxNivel') || '1');

  function resize() {
    const scale = Math.min(window.innerWidth / 640, window.innerHeight / 350);
    canvas.style.width = `${Math.floor(640 * scale)}px`;
    canvas.style.height = `${Math.floor(350 * scale)}px`;
  }
  resize();
  window.addEventListener('resize', resize);

  loadAssets().then(() => {
    requestAnimationFrame(gameLoop);
  });
  requestAnimationFrame(gameLoop);
}

init();
