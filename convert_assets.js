// Convert Giroloco DOS assets to modern formats
// IBP (16-color bitplane images) -> PNG
// PANT.DAT (level data) -> JSON
// NODOS.DAT + MASCARAS.DAT (node sprites) -> PNG
// BICHOS.PUN (enemy sprites) -> PNG

const fs = require('fs');
const path = require('path');

const OUT_DIR = path.join(__dirname, 'web', 'assets');
fs.mkdirSync(path.join(OUT_DIR, 'images'), { recursive: true });

// EGA 16-color palette (standard VGA)
const EGA_PALETTE = [
  [0x00, 0x00, 0x00], // 0  BLACK
  [0x00, 0x00, 0xAA], // 1  BLUE
  [0x00, 0xAA, 0x00], // 2  GREEN
  [0x00, 0xAA, 0xAA], // 3  CYAN
  [0xAA, 0x00, 0x00], // 4  RED
  [0xAA, 0x00, 0xAA], // 5  MAGENTA
  [0xAA, 0x55, 0x00], // 6  BROWN
  [0xAA, 0xAA, 0xAA], // 7  LIGHTGRAY
  [0x55, 0x55, 0x55], // 8  DARKGRAY
  [0x55, 0x55, 0xFF], // 9  LIGHTBLUE
  [0x55, 0xFF, 0x55], // 10 LIGHTGREEN
  [0x55, 0xFF, 0xFF], // 11 LIGHTCYAN
  [0xFF, 0x55, 0x55], // 12 LIGHTRED
  [0xFF, 0x55, 0xFF], // 13 LIGHTMAGENTA
  [0xFF, 0xFF, 0x55], // 14 YELLOW
  [0xFF, 0xFF, 0xFF], // 15 WHITE
];

// ---- IBP Converter ----
function convertIBP(inputPath, outputPath) {
  const data = fs.readFileSync(inputPath);
  // Header: 2 bytes ancho (width), 2 bytes alto (height), 124 bytes padding
  const width = data.readInt16LE(0);
  const height = data.readInt16LE(2);
  const headerSize = 128;
  const bytesPerLine = width / 8;
  const planeSize = bytesPerLine * height;

  console.log(`  IBP: ${inputPath} -> ${width}x${height}`);

  // Read 4 bitplanes
  const pixels = new Uint8Array(width * height);
  for (let bp = 0; bp < 4; bp++) {
    const planeOffset = headerSize + bp * planeSize;
    for (let y = 0; y < height; y++) {
      for (let xByte = 0; xByte < bytesPerLine; xByte++) {
        const b = data[planeOffset + y * bytesPerLine + xByte];
        for (let bit = 0; bit < 8; bit++) {
          const px = xByte * 8 + (7 - bit);
          if (px < width) {
            pixels[y * width + px] |= ((b >> bit) & 1) << bp;
          }
        }
      }
    }
  }

  // Write as raw PPM, return path for sips conversion
  return writePNG(outputPath, width, height, pixels);
}

// Minimal PNG writer (no dependencies)
function writePNG(outputPath, width, height, indexedPixels) {
  const rgba = Buffer.alloc(width * height * 4);
  for (let i = 0; i < indexedPixels.length; i++) {
    const c = EGA_PALETTE[indexedPixels[i] & 0x0F];
    rgba[i * 4] = c[0];
    rgba[i * 4 + 1] = c[1];
    rgba[i * 4 + 2] = c[2];
    rgba[i * 4 + 3] = 255;
  }
  // Use raw PPM for simplicity, then convert via sips
  const ppmPath = outputPath.replace('.png', '.ppm');
  const header = `P6\n${width} ${height}\n255\n`;
  const rgb = Buffer.alloc(width * height * 3);
  for (let i = 0; i < indexedPixels.length; i++) {
    const c = EGA_PALETTE[indexedPixels[i] & 0x0F];
    rgb[i * 3] = c[0];
    rgb[i * 3 + 1] = c[1];
    rgb[i * 3 + 2] = c[2];
  }
  fs.writeFileSync(ppmPath, Buffer.concat([Buffer.from(header), rgb]));
  return ppmPath;
}

// ---- PANT.DAT Level Converter ----
function convertLevels(inputPath, outputPath) {
  const data = fs.readFileSync(inputPath);
  const TAM_PANT = 512;
  const NODOS_X = 14, NODOS_Y = 8;
  const levelCount = Math.floor(data.length / TAM_PANT);
  const levels = [];

  console.log(`  Levels: ${levelCount} found in PANT.DAT`);

  for (let l = 0; l < levelCount; l++) {
    const offset = l * TAM_PANT;
    const level = {};

    // Info array: 14*8 = 112 unsigned ints (16-bit LE)
    level.grid = [];
    for (let y = 0; y < NODOS_Y; y++) {
      const row = [];
      for (let x = 0; x < NODOS_X; x++) {
        const idx = y * NODOS_X + x;
        const val = data.readUInt16LE(offset + idx * 2);
        const type = val >> 8;
        const param = val & 0xFF;
        const typeChar = type ? String.fromCharCode(type) : ' ';
        row.push({ type: typeChar, param });
      }
      level.grid.push(row);
    }

    // After Info array (224 bytes), CantBichos (1 byte)
    let pos = offset + 224;
    level.enemyCount = data[pos++];

    // SInfoBicho: tipo(1) + inix(1) + iniy(1) + pad(1) + intel(2) + taparic(2) + duracion(2) = 10 bytes each
    // Actually let me check the struct layout more carefully
    // struct { byte tipo, inix, iniy; int intel, taparic, duracion; }
    // In Turbo C++, byte=1, int=2, so struct is 1+1+1+2+2+2=9 bytes
    // But structs may be padded. Let me check...
    // Actually for Borland Turbo C++, default packing is byte-aligned for this struct
    // tipo(1) + inix(1) + iniy(1) + intel(2) + taparic(2) + duracion(2) = 9 bytes
    // 20 enemies * 9 = 180 bytes
    level.enemies = [];
    for (let e = 0; e < 20; e++) {
      const ePos = pos + e * 9;
      const enemy = {
        tipo: data[ePos],
        inix: data[ePos + 1],
        iniy: data[ePos + 2],
        intel: data.readInt16LE(ePos + 3),
        taparic: data.readInt16LE(ePos + 5),
        duracion: data.readInt16LE(ePos + 7),
      };
      if (e < level.enemyCount) {
        level.enemies.push(enemy);
      }
    }

    // After 20 enemies (180 bytes)
    pos += 180;
    level.cx = data[pos++];
    level.cy = data[pos++];
    level.nro = data.readUInt16LE(pos); pos += 2;
    level.nroFondo = data[pos];

    // Derive level/screen numbers
    level.majorLevel = level.nro >> 8;
    level.screenNum = level.nro & 0xFF;

    levels.push(level);
  }

  fs.writeFileSync(outputPath, JSON.stringify(levels, null, 2));
  return levels;
}

// ---- NODOS.DAT Sprite Converter ----
// Nodes are stored as BGI putimage format: 2 bytes width, 2 bytes height, then bitmap data
// Each node image occupies MEMORIA_NODO (94) bytes
function convertNodeSprites(nodosPath, mascarasPath, outputPath) {
  const nodosData = fs.readFileSync(nodosPath);
  const mascarasData = fs.readFileSync(mascarasPath);

  // BGI image format: first 4 bytes are width (2) and height (2), then bitplane data
  // For 16-color mode: 4 bitplanes
  // RADIO_NODO_X=7, RADIO_NODO_Y=5 -> image is 15x11 pixels
  // BGI stores width-1 and height-1 in the header
  const MEMORIA_NODO = 94;
  const nodeCount = Math.floor(nodosData.length / MEMORIA_NODO);

  console.log(`  Node sprites: ${nodeCount} found (${MEMORIA_NODO} bytes each)`);

  const sprites = [];
  for (let i = 0; i < nodeCount; i++) {
    const off = i * MEMORIA_NODO;
    const w = nodosData.readUInt16LE(off) + 1;
    const h = nodosData.readUInt16LE(off + 2) + 1;
    if (w > 100 || h > 100 || w <= 0 || h <= 0) {
      sprites.push(null);
      continue;
    }

    // BGI 16-color image: 4 bitplanes INTERLEAVED PER ROW
    // Layout: for each row: plane0 bytes, plane1 bytes, plane2 bytes, plane3 bytes
    const bytesPerRow = Math.ceil(w / 8);
    const headerBytes = 4;

    const pixels = new Uint8Array(w * h);
    const maskPixels = new Uint8Array(w * h);

    for (let y = 0; y < h; y++) {
      for (let bp = 0; bp < 4; bp++) {
        for (let xb = 0; xb < bytesPerRow; xb++) {
          const byteIdx = off + headerBytes + y * (bytesPerRow * 4) + bp * bytesPerRow + xb;
          if (byteIdx < nodosData.length) {
            const b = nodosData[byteIdx];
            const mb = mascarasData[byteIdx];
            for (let bit = 7; bit >= 0; bit--) {
              const px = xb * 8 + (7 - bit);
              if (px < w) {
                pixels[y * w + px] |= ((b >> bit) & 1) << bp;
                maskPixels[y * w + px] |= ((mb >> bit) & 1) << bp;
              }
            }
          }
        }
      }
    }

    sprites.push({ w, h, pixels: Array.from(pixels), mask: Array.from(maskPixels) });
  }

  fs.writeFileSync(outputPath, JSON.stringify({ sprites, palette: EGA_PALETTE }));
  console.log(`  First sprite dimensions: ${sprites[0] ? sprites[0].w + 'x' + sprites[0].h : 'null'}`);
}

// ---- BICHOS.PUN Enemy Sprite Converter ----
function convertEnemySprites(puntPath, outputPath) {
  const data = fs.readFileSync(puntPath);
  // First 2 bytes: tam (size of each sprite in bytes)
  const tam = data.readInt16LE(0);
  // Data: tam * 24 * TIPO_BICHOS (4) sprites
  // 24 = 3 frames * 2 (normal+mask) * 4 orientations
  // Actually: 3 frames * 2 norms * 4 orientations = 24 per type
  // Total: tam * 24 * 4 bytes of image data
  const TIPO_BICHOS = 4;
  const totalSprites = 24 * TIPO_BICHOS; // 96 sprites

  console.log(`  Enemy sprites: tam=${tam}, total=${totalSprites}`);

  // Each sprite is a BGI putimage format: 4-byte header + bitplane data
  // B_SAncho=10, B_SAlto=10 -> sprites are about 16x16 (from PCX description)
  const sprites = [];
  for (let i = 0; i < totalSprites; i++) {
    const off = 2 + i * tam;
    if (off + 4 > data.length) break;
    const w = data.readUInt16LE(off) + 1;
    const h = data.readUInt16LE(off + 2) + 1;
    if (w > 64 || h > 64 || w <= 0 || h <= 0) {
      sprites.push(null);
      continue;
    }

    const bytesPerRow = Math.ceil(w / 8);
    const pixels = new Uint8Array(w * h);

    // BGI 16-color: bitplanes interleaved per row
    for (let y = 0; y < h; y++) {
      for (let bp = 0; bp < 4; bp++) {
        for (let xb = 0; xb < bytesPerRow; xb++) {
          const byteIdx = off + 4 + y * (bytesPerRow * 4) + bp * bytesPerRow + xb;
          if (byteIdx < data.length) {
            const b = data[byteIdx];
            for (let bit = 7; bit >= 0; bit--) {
              const px = xb * 8 + (7 - bit);
              if (px < w) {
                pixels[y * w + px] |= ((b >> bit) & 1) << bp;
              }
            }
          }
        }
      }
    }

    sprites.push({ w, h, pixels: Array.from(pixels) });
  }

  fs.writeFileSync(outputPath, JSON.stringify({ tam, sprites, palette: EGA_PALETTE }));
}

// Run all conversions
console.log('Converting Giroloco assets...');

// Convert IBP images
const ibpFiles = ['I0', 'I1', 'I2', 'I3', 'I4', 'I5', 'I6', 'F0', 'F1', 'F2', 'F3', 'F4', 'F5'];
const ppmFiles = [];
for (const name of ibpFiles) {
  const ibpPath = path.join(__dirname, `${name}.IBP`);
  if (fs.existsSync(ibpPath)) {
    const ppm = convertIBP(ibpPath, path.join(OUT_DIR, 'images', `${name.toLowerCase()}.png`));
    ppmFiles.push({ ppm, png: path.join(OUT_DIR, 'images', `${name.toLowerCase()}.png`) });
  }
}

// Convert levels
convertLevels(
  path.join(__dirname, 'PANT.DAT'),
  path.join(OUT_DIR, 'levels.json')
);

// Convert node sprites
convertNodeSprites(
  path.join(__dirname, 'NODOS.DAT'),
  path.join(__dirname, 'MASCARAS.DAT'),
  path.join(OUT_DIR, 'nodes.json')
);

// Convert enemy sprites
convertEnemySprites(
  path.join(__dirname, 'BICHOS.PUN'),
  path.join(OUT_DIR, 'enemies.json')
);

// Output sips conversion commands
console.log('\nTo convert PPM to PNG, run:');
for (const { ppm, png } of ppmFiles) {
  console.log(`sips -s format png "${ppm}" --out "${png}"`);
}
console.log('\nDone!');
