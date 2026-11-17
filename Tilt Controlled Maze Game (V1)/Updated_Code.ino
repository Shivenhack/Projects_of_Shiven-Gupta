#include <Wire.h>
#include <U8g2lib.h>

// SSD1306 OLED
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// If your display is SH1106 use this instead:
// U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// MPU6050
const int MPU_ADDR = 0x68;

// Maze settings
#define COLS 16
#define ROWS 8
#define CELL_SIZE 8
#define TOTAL_CELLS (COLS * ROWS)
#define BUZZER_PIN 9
#define MUTE_BUTTON 8

bool soundEnabled = true;
bool lastButtonState = LOW;

#define NOTE_C6 1047
#define NOTE_D6 1175
#define NOTE_E5 659
#define NOTE_E6 1319
#define NOTE_G5 784
#define NOTE_G6 1568

const byte WALL_TOP = 1;
const byte WALL_RIGHT = 2;
const byte WALL_BOTTOM = 4;
const byte WALL_LEFT = 8;

byte maze[ROWS][COLS];
bool visited[ROWS][COLS];

int stackX[TOTAL_CELLS];
int stackY[TOTAL_CELLS];
int stackTop = 0;

int playerX = 0;
int playerY = 0;

int level = 1;

unsigned long lastMoveTime = 0;
const unsigned long moveDelay = 180;

const int TILT_THRESHOLD = 6000;

void setup() {

  pinMode(MUTE_BUTTON, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Wire.begin();

  u8g2.begin();

  pinMode(BUZZER_PIN, OUTPUT);

  initMPU6050();

  randomSeed(analogRead(A0));

  generateNewMaze();
  drawMaze();
}


void loop() {

  bool buttonState = digitalRead(MUTE_BUTTON);

  if (buttonState == HIGH && lastButtonState == LOW) {
    soundEnabled = !soundEnabled;

    delay(200);  // debounce
  }

  lastButtonState = buttonState;
  int16_t ax, ay, az;
  readMPU6050(ax, ay, az);

  if (millis() - lastMoveTime > moveDelay) {
    if (ay > TILT_THRESHOLD)
      movePlayer(-1, 0);

    else if (ay < -TILT_THRESHOLD)
      movePlayer(1, 0);

    else if (ax > TILT_THRESHOLD)
      movePlayer(0, -1);

    else if (ax < -TILT_THRESHOLD)
      movePlayer(0, 1);
  }
  if (playerX == COLS - 1 && playerY == ROWS - 1) {
    level++;

    playVictoryTune();

    showWinScreen();

    delay(1000);

    generateNewMaze();

    drawMaze();
  }
}

// Victory tune
void playVictoryTune()
{
  if(!soundEnabled)
    return;

  int melody[] = {
    NOTE_E5,
    NOTE_G5,
    NOTE_E6,
    NOTE_C6,
    NOTE_D6,
    NOTE_G6
  };

  int noteDurations[] = {
    100,
    100,
    100,
    100,
    100,
    300
  };

  for(int i = 0; i < 6; i++)
  {
    tone(BUZZER_PIN, melody[i]);
    delay(noteDurations[i]);

    noTone(BUZZER_PIN);
    delay(30);
  }
}

void initMPU6050() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

void readMPU6050(int16_t &ax,
                 int16_t &ay,
                 int16_t &az) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_ADDR, 6, true);

  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();
}

void generateNewMaze() {
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      maze[y][x] = WALL_TOP | WALL_RIGHT | WALL_BOTTOM | WALL_LEFT;

      visited[y][x] = false;
    }
  }

  playerX = 0;
  playerY = 0;

  stackTop = 0;

  int cx = 0;
  int cy = 0;

  visited[cy][cx] = true;

  stackX[stackTop] = cx;
  stackY[stackTop] = cy;
  stackTop++;

  while (stackTop > 0) {
    cx = stackX[stackTop - 1];
    cy = stackY[stackTop - 1];

    int dirs[4];
    int count = 0;

    if (cy > 0 && !visited[cy - 1][cx])
      dirs[count++] = 0;

    if (cx < COLS - 1 && !visited[cy][cx + 1])
      dirs[count++] = 1;

    if (cy < ROWS - 1 && !visited[cy + 1][cx])
      dirs[count++] = 2;

    if (cx > 0 && !visited[cy][cx - 1])
      dirs[count++] = 3;

    if (count > 0) {
      int dir = dirs[random(count)];

      int nx = cx;
      int ny = cy;

      if (dir == 0) {
        ny--;
        maze[cy][cx] &= ~WALL_TOP;
        maze[ny][nx] &= ~WALL_BOTTOM;
      } else if (dir == 1) {
        nx++;
        maze[cy][cx] &= ~WALL_RIGHT;
        maze[ny][nx] &= ~WALL_LEFT;
      } else if (dir == 2) {
        ny++;
        maze[cy][cx] &= ~WALL_BOTTOM;
        maze[ny][nx] &= ~WALL_TOP;
      } else {
        nx--;
        maze[cy][cx] &= ~WALL_LEFT;
        maze[ny][nx] &= ~WALL_RIGHT;
      }

      visited[ny][nx] = true;

      stackX[stackTop] = nx;
      stackY[stackTop] = ny;
      stackTop++;
    } else {
      stackTop--;
    }
  }
}

void movePlayer(int dx, int dy) {
  byte walls = maze[playerY][playerX];

  if (dx == -1 && (walls & WALL_LEFT))
    return;

  if (dx == 1 && (walls & WALL_RIGHT))
    return;

  if (dy == -1 && (walls & WALL_TOP))
    return;

  if (dy == 1 && (walls & WALL_BOTTOM))
    return;

  playerX += dx;
  playerY += dy;

  lastMoveTime = millis();

  drawMaze();
}

void drawMaze() {
  u8g2.clearBuffer();

  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      int px = x * CELL_SIZE;
      int py = y * CELL_SIZE;

      byte walls = maze[y][x];

      if (walls & WALL_TOP)
        u8g2.drawLine(px, py,
                      px + CELL_SIZE, py);

      if (walls & WALL_RIGHT)
        u8g2.drawLine(px + CELL_SIZE - 1,
                      py,
                      px + CELL_SIZE - 1,
                      py + CELL_SIZE);

      if (walls & WALL_BOTTOM)
        u8g2.drawLine(px,
                      py + CELL_SIZE - 1,
                      px + CELL_SIZE,
                      py + CELL_SIZE - 1);

      if (walls & WALL_LEFT)
        u8g2.drawLine(px,
                      py,
                      px,
                      py + CELL_SIZE);
    }
  }

  // Goal
  u8g2.drawBox(124, 58, 4, 4);

  // Player
  int ballX =
    playerX * CELL_SIZE + CELL_SIZE / 2;

  int ballY =
    playerY * CELL_SIZE + CELL_SIZE / 2;

  u8g2.drawDisc(ballX, ballY, 2);

  // Level display
  char buf[20];
  sprintf(buf, "L%d", level);
  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.drawStr(2, 7, buf);

  u8g2.sendBuffer();
}

void showWinScreen() {
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(18, 22, "YOU WIN!");

  char buf[20];
  sprintf(buf, "LEVEL %d", level);

  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.drawStr(25, 45, buf);

  u8g2.sendBuffer();
}
