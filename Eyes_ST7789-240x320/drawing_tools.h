#include <Arduino.h>
#include <Adafruit_ST7789.h> //https://github.com/adafruit/Adafruit-ST7735-Library

//#include <Arduino_ST7789_Fast.h> // https://github.com/cbm80amiga/Arduino_ST7789_Fast
//#include <ST7789v_arduino.h>   //https://github.com/deirvlon/ST7789v-Arduino
#include <SPI.h>
#include <Adafruit_GFX.h>



#ifndef _DRAWING_TOOLSH_
#define _DRAWING_TOOLSH_

#define TFT_DC    7
#define TFT_RST   8 
#define TFT_CS    9 // only for displays with CS pin
//#define TFT_MOSI  11   // for hardware SPI data pin (all of available pins)
//#define TFT_SCLK  13   // for hardware SPI sclk pin (all of available pins)


void init_tft();
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void drawFilledInscribedEllipse(int x_center, int y_center, int a0, int b0, int a1, int b1, uint16_t color_ellipse_0, uint16_t color_ellipse_1 , bool fill_ellipse0, int second_ellipse_offset = -1, int happy_eye_offset = -1 );
void drawFilledEllipseWithBackgroundBox(int x_center, int y_center, int a, int b, int w, int h, uint16_t color_ellipse, uint16_t color_background , bool draw_ellipse, int second_ellipse_offset = -1 );
void fillEllipse(int16_t x_center, int16_t y_center, int16_t a0, int16_t b0, uint16_t color);
void draw_image(uint16_t* face2_sm, int h, int w);
void draw_triangle(int16_t p0x, int16_t p0y, int16_t p1x,int16_t p1y, int16_t p2x,int16_t p2y, uint16_t color);
void clear_tft();

//convert to RGB565
int draw_image_rle(uint16_t* image_rle, int image_rle_length, int h, int w, int index_img=0);
int draw_image_rle_conv(uint8_t* image_rle, uint32_t image_rle_length, int h, int w, uint32_t index_img=0);

#endif