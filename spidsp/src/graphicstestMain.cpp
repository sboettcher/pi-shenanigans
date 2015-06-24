#include <time.h>

#include "./Adafruit_GFX.h"
#include "./Adafruit_ILI9340.h"

#define _dc 24
#define _rst 25

Adafruit_ILI9340 tft = Adafruit_ILI9340(_dc, _rst);

float testFillScreen() {
  const clock_t begin_time = clock();
  tft.fillScreen(ILI9340_BLACK);
  tft.fillScreen(ILI9340_RED);
  tft.fillScreen(ILI9340_GREEN);
  tft.fillScreen(ILI9340_BLUE);
  tft.fillScreen(ILI9340_BLACK);
  return float( clock () - begin_time ) /  CLOCKS_PER_SEC;
}

float testText() {
  tft.fillScreen(ILI9340_BLACK);
  const clock_t begin_time = clock();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9340_WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.println();
  tft.setTextColor(ILI9340_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  return float( clock () - begin_time ) /  CLOCKS_PER_SEC;
}

float testLines(uint16_t color) {
  float t;
  int           x1, y1, x2, y2,
                w = tft.width(),
                h = tft.height();

  tft.fillScreen(ILI9340_BLACK);

  x1 = y1 = 0;
  y2    = h - 1;
  clock_t begin_time = clock();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t     = float( clock () - begin_time ) /  CLOCKS_PER_SEC; // fillScreen doesn't count against timing

  tft.fillScreen(ILI9340_BLACK);

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  begin_time = clock();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += float( clock () - begin_time ) /  CLOCKS_PER_SEC;

  tft.fillScreen(ILI9340_BLACK);

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  begin_time = clock();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += float( clock () - begin_time ) /  CLOCKS_PER_SEC;

  tft.fillScreen(ILI9340_BLACK);

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  begin_time = clock();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += float( clock () - begin_time ) /  CLOCKS_PER_SEC;

  return t;
}

float testFastLines(uint16_t color1, uint16_t color2) {
  int           x, y, w = tft.width(), h = tft.height();

  tft.fillScreen(ILI9340_BLACK);
  const clock_t begin_time = clock();
  for(y=0; y<h; y+=5) tft.drawFastHLine(0, y, w, color1);
  for(x=0; x<w; x+=5) tft.drawFastVLine(x, 0, h, color2);

  return float( clock () - begin_time ) /  CLOCKS_PER_SEC;
}

float testRects(uint16_t color) {
  int           n, i, i2,
                cx = tft.width()  / 2,
                cy = tft.height() / 2;

  tft.fillScreen(ILI9340_BLACK);
  n     = std::min(tft.width(), tft.height());
  const clock_t begin_time = clock();
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    tft.drawRect(cx-i2, cy-i2, i, i, color);
  }

  return float( clock () - begin_time ) /  CLOCKS_PER_SEC;
}

float testFilledRects(uint16_t color1, uint16_t color2) {
  float t = 0;
  int           n, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9340_BLACK);
  n = std::min(tft.width(), tft.height());
  clock_t begin_time = clock();
  for(i=n; i>0; i-=6) {
    i2    = i / 2;
    begin_time = clock();
    tft.fillRect(cx-i2, cy-i2, i, i, color1);
    t    += float( clock () - begin_time ) /  CLOCKS_PER_SEC;
    // Outlines are not included in timing results
    tft.drawRect(cx-i2, cy-i2, i, i, color2);
  }

  return t;
}

float testFilledCircles(uint8_t radius, uint16_t color) {
  int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

  tft.fillScreen(ILI9340_BLACK);
  const clock_t begin_time = clock();
  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      tft.fillCircle(x, y, radius, color);
    }
  }

  return float( clock () - begin_time ) /  CLOCKS_PER_SEC;
}

float testCircles(uint8_t radius, uint16_t color) {
  int           x, y, r2 = radius * 2,
                w = tft.width()  + radius,
                h = tft.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  const clock_t begin_time = clock();
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }

  return float( clock () - begin_time ) /  CLOCKS_PER_SEC;
}

float testTriangles() {
  int           n, i, cx = tft.width()  / 2 - 1,
                      cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9340_BLACK);
  n     = std::min(cx, cy);
  const clock_t begin_time = clock();
  for(i=0; i<n; i+=5) {
    tft.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      tft.Color565(0, 0, i));
  }

  return float( clock () - begin_time ) /  CLOCKS_PER_SEC;
}

float testFilledTriangles() {
  float t = 0;
  int           i, cx = tft.width()  / 2 - 1,
                   cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9340_BLACK);
  clock_t begin_time = clock();
  for(i=std::min(cx,cy); i>10; i-=5) {
    begin_time = clock();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.Color565(0, i, i));
    t += float( clock () - begin_time ) /  CLOCKS_PER_SEC;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.Color565(i, i, 0));
  }

  return t;
}

float testRoundRects() {
  int           w, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9340_BLACK);
  w     = std::min(tft.width(), tft.height());
  const clock_t begin_time = clock();
  for(i=0; i<w; i+=6) {
    i2 = i / 2;
    tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, tft.Color565(i, 0, 0));
  }

  return float( clock () - begin_time ) /  CLOCKS_PER_SEC;
}

float testFilledRoundRects() {
  int           i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9340_BLACK);
  const clock_t begin_time = clock();
  for(i=std::min(tft.width(), tft.height()); i>20; i-=6) {
    i2 = i / 2;
    tft.fillRoundRect(cx-i2, cy-i2, i, i, i/8, tft.Color565(0, i, 0));
  }

  return float( clock () - begin_time ) /  CLOCKS_PER_SEC;
}

int main(int argc, char** argv) {
  
  tft.begin();

  printf("Benchmark - Time (microseconds)\n");
  printf("Screen fill - %f\n", testFillScreen());
  bcm2835_delay(500);

  printf("Text - %f\n", testText());
  bcm2835_delay(3000);

  printf("Lines - %f\n", testLines(ILI9340_CYAN));
  bcm2835_delay(500);

  printf("Horiz/Vert Lines - %f\n", testFastLines(ILI9340_RED, ILI9340_BLUE));
  bcm2835_delay(500);

  printf("Rectangles (outline) - %f\n", testRects(ILI9340_GREEN));
  bcm2835_delay(500);

  printf("Rectangles (filled) - %f\n", testFilledRects(ILI9340_YELLOW, ILI9340_MAGENTA));
  bcm2835_delay(500);

  printf("Circles (filled) - %f\n", testFilledCircles(10, ILI9340_MAGENTA));
  bcm2835_delay(500);
  
  printf("Circles (outline) - %f\n", testCircles(10, ILI9340_WHITE));
  bcm2835_delay(500);

  printf("Triangles (outline) - %f\n", testTriangles());
  bcm2835_delay(500);

  printf("Triangles (filled) - %f\n", testFilledTriangles());
  bcm2835_delay(500);

  printf("Rounded rects (outline) - %f\n", testRoundRects());
  bcm2835_delay(500);

  printf("Rounded rects (filled) - %f\n", testFilledRoundRects());
  bcm2835_delay(500);

  printf("Done!\n");
  return 0;
}
