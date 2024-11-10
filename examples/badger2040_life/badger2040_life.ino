//
// Pimoroni Badger2040 partial update example
//
// In the Arduino IDE, select Raspberry Pi Pico (Arduino Mbed OS) as the target
//
#include <bb_epaper.h>

BBEPAPER bbep(EP29_128x296);
#define BADGER_CS 17
#define BADGER_BUSY 26
#define BADGER_RES 21
#define BADGER_DC 20
#define BADGER_3V3 10
// use 2x2 pixels to see them easier, so the display res is 1/2
#define SCREEN_WIDTH  (128/2)
#define SCREEN_HEIGHT (296/2)
// Current and previous state
uint8_t ucFrame[SCREEN_WIDTH * SCREEN_HEIGHT];
uint8_t ucFrameCopy[SCREEN_WIDTH * SCREEN_HEIGHT];
int iteration = 0;

void createRandomMatrix(uint8_t *pScreen) {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            pScreen[(i*SCREEN_WIDTH) + j] = random(100) < 25;
        }
    }
} /* createRandomMatrix() */

void addGlider(int i1, int j1, uint8_t *pScreen) {
    uint8_t glider[3][3] = {{0, 0, 1}, {1, 0, 1}, {0, 1, 1}};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            pScreen[(i1 + i) * SCREEN_WIDTH + j1 + j] = glider[i][j];
        }
    }
} /* addGlider() */

void gameOfLife(void) {
    uint8_t *s;
    int total;

    for (int i = 1; i < SCREEN_HEIGHT-1; i++) {
        for (int j = 1; j < SCREEN_WIDTH-1; j++) {
            s = &ucFrameCopy[(i * SCREEN_WIDTH) + j]; // start of this row
            total = s[-1];
            total += s[1];
            total += s[-SCREEN_WIDTH];
            total += s[SCREEN_WIDTH];
            total += s[-SCREEN_WIDTH - 1];
            total += s[-SCREEN_WIDTH + 1];
            total += s[SCREEN_WIDTH - 1];
            total += s[SCREEN_WIDTH + 1];

            if (s[0] == 1) {
                if (total < 2 || total > 3) {
                    ucFrame[(i*SCREEN_WIDTH)+j] = 0;
                }
            } else if (total == 3) {
                ucFrame[(i*SCREEN_WIDTH)+j] = 1;
            }
        } // for j
    } // for i
} /* gameOfLife() */

void drawFrame()
{
  uint8_t c;
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            c = (ucFrameCopy[(i*SCREEN_WIDTH)+j]) ? BBEP_BLACK : BBEP_WHITE;
            bbep.drawPixel(j*2, i*2, c);
            bbep.drawPixel(j*2+1, i*2, c);
            bbep.drawPixel(j*2, i*2+1, c);
            bbep.drawPixel(j*2+1, i*2+1, c);
        }
    }
} /* drawFrame() */
void setup() {
  pinMode(BADGER_3V3, OUTPUT); // in case we're running on battery
  digitalWrite(BADGER_3V3, HIGH);

  bbep.initIO(BADGER_DC, BADGER_RES, BADGER_BUSY, BADGER_CS);
  bbep.allocBuffer();
  bbep.fillScreen(BBEP_WHITE);
  bbep.writePlane();
  bbep.refresh(REFRESH_FULL); // do a full refresh to start

  randomSeed(micros());
  createRandomMatrix(ucFrame);
  for (int i = 0; i < 10; i++) {
      addGlider(random(SCREEN_HEIGHT-2), random(SCREEN_WIDTH-2), ucFrame);
  }
  memcpy(ucFrameCopy, ucFrame, sizeof(ucFrame)); // keep copy of previous
  bbep.wait();
} /* setup() */

void loop() {
  while (1) {
    bbep.writePlane(PLANE_1); // old pixels need to be explicitly written on UC81xx controllers
    bbep.backupPlane(); // copy memory plane 1 to 0 (new --> old)
    gameOfLife(); // update the scene
    memcpy(ucFrameCopy, ucFrame, sizeof(ucFrame)); // new becomes old for our copy of the state
    drawFrame(); // draw the new pixels into memory plane 0 (new)
    bbep.writePlane(PLANE_0); // new plane (for partial updates)
    bbep.refresh(((iteration & 0x1f) == 0x1f) ? REFRESH_FULL : REFRESH_PARTIAL); // full refresh needs white (old) on UC81xx
    bbep.wait(); // wait for e-paper to finish its update
    bbep.sleep(LIGHT_SLEEP);
    iteration++;
  } // while (1)
} /* loop() */
