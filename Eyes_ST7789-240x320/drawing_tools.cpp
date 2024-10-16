#include "drawing_tools.h"
#include <avr/pgmspace.h>


//VCC use 3.3v
//cda is 11   hardware spi
//scl is 13   11   hardware spi

//ST7789v_arduino tft = ST7789v_arduino(TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_CS); //for display with CS pin and DC via 9bit SPI
//Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST, TFT_CS);
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


uint16_t rgb332_to_rgb565(uint8_t rgb332) {
    uint16_t rgb565;
    // Extract RGB components from the uint8 value
    uint8_t red = rgb332 & 0xE0; //11100000
    uint8_t green = rgb332 & 0x1C; //00011100
    uint8_t blue = rgb332 & 0x03; //00000011
    // Convert RGB components to RGB565 format
    rgb565 = ((red << 8) | (green << 6) | blue << 3);
    return rgb565;
}


void init_tft()
{
  tft.init(240,320);
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(3);
}

void clear_tft()
{
  tft.fillScreen(ST77XX_BLACK);
}

int sub2ind(const int row,const int col,const int cols,const int rows)
{
   return row*cols+col;
}

void ind2sub(const uint32_t sub,const int cols,const int rows,int &row,int &col)
{
   row=sub/cols;
   col=sub%cols;
}

int draw_image_rle_conv(uint8_t* image_rle, uint32_t image_rle_length, int h, int w, uint32_t index_img)
{
  uint16_t count = 0;
  uint8_t val_b = 0;
  uint16_t val = 0;
  
  int x=0;
  int y=0;
  for(uint32_t ind_rle=0;ind_rle<image_rle_length;ind_rle+=2)
  {
    count = pgm_read_byte(&image_rle[ind_rle]);
    val_b = pgm_read_byte(&image_rle[ind_rle+1]);
    if(val_b>255)
      val_b = 255;  
    val = rgb332_to_rgb565(val_b);    
    ind2sub(index_img,w,h,y,x);
    drawFastHLine(x,y,count,val);
    index_img+=count;
  }

  return index_img;
  
}

int draw_image_rle(uint16_t* image_rle, int image_rle_length, int h, int w, int index_img)
{
  uint16_t count = 0;
  uint16_t val = 0;  
  int x=0;
  int y=0;
  for(int ind_rle=0;ind_rle<image_rle_length;ind_rle+=2)
  {
    count = pgm_read_word(&image_rle[ind_rle]);
    val = pgm_read_word(&image_rle[ind_rle+1]);    
    ind2sub(index_img,w,h,y,x);
    drawFastHLine(x,y,count,val);
    index_img+=count;  
  }
  return index_img;
}



void draw_image(uint16_t* image, int h, int w)
{
  int x=0;
  int y=0;

  for(uint16_t i=0;i<h;i++)
  {
    for(uint16_t j=0;j<w;j++)
    {
      uint16_t v = pgm_read_word(&image[i*w+j]);
      for(int ii=0;ii<4;ii++)
        for(int jj=0;jj<4;jj++)
          tft.drawPixel(4*j+jj,4*i+ii,v);
      
      
    }
  }

}


void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  //delay(draw_delay);  
  tft.drawFastHLine(x,y,w,color);  
  
}

void draw_triangle(int16_t x0, int16_t y0, int16_t x1,int16_t y1, int16_t x2,int16_t y2, uint16_t color)
{
   tft.fillTriangle(x0,y0,x1,y1,x2,y2,color);
}

void fillEllipse(int16_t x_center, int16_t y_center, int16_t a0, int16_t b0, uint16_t color)
{
  for (int y = y_center - b0; y <= y_center + b0; y++) 
  {
    float y_ = y;
    float disc0 = (1 - (y_ - y_center)*(y_ - y_center) / (b0*b0));
    
    if (disc0 >= 0) 
    {
      //in ellipse
      float sqrt_disc = sqrt(disc0);
      float x1 = x_center + a0 * sqrt_disc;
      float x2 = x_center - a0 * sqrt_disc;
      drawFastHLine(x2, y, x1 - x2+1, color);
    }
  }
}



void drawFilledInscribedEllipse(int x_center, int y_center, int a0, int b0, int a1, int b1, uint16_t color_ellipse_0, uint16_t color_ellipse_1 , bool fill_ellipse0, int second_ellipse_offset = -1, int happy_eye_offset = -1 ) 
{

  //draw 2 inscribe ellipses. option to fill the smaller one. possible to draw a second set off ellipse, like both eyes, simultaneously by setting second_ellipse_offset to something else that -1
  //fill the section between both ellipses contour
  //x_center,y_center,  axis of the smallest ellipse, (a0,b0),  axis of the larger ellipse (a1,b1), 
  //color_ellipse_0  uint16 rgb565 color of the small ellipse 0.
  //color_ellipse_1  uint16 rgb565 color of the big ellipse 1.
  // *check the macro RGB(r, g, b) (((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))
  //fill_ellipse0   fill small ellipse with color0
  //second_ellipse_offset   offset of the x_center of the second ellipse pair. If -1, the second set is not drawn
  //happy_eye_offset   vertical offset of an ellipse that will bne drawn instead of the smallest ellipse. This will create the happy eye. this works with the factorHE, an oversize on this offset ellispe that make nicer happy eye.


  //happy_eye_offset is the bvertical offset of the happy eye ellipse drawn bellow the center of the central ellipse 0
  //its size it twice that of ellipse 0.  it is drawn filled with the color of ellipse 1
  //happy eye is he in the code
  float factorHE = 1.4;
  
  for (int y = y_center - b1; y <= y_center + b1; y++) 
  {
    float y_ = y;
    float disc0 = (1 - (y_ - y_center)*(y_ - y_center) / (b0*b0));
    float disc1 = (1 - (y_ - y_center)*(y_ - y_center) / (b1*b1));
    float disc_he = -1;
    if(happy_eye_offset>-1)
    {    
      disc_he = (1 - (y_ - y_center - happy_eye_offset)*(y_ - y_center - happy_eye_offset) / (factorHE*b1*b1));
    }

    if (disc1 >= 0) 
    {
      //in largest ellipse
      if(disc0<=0)
      {
        //not crossing small ellipse, draw only large
        float sqrt_disc = sqrt(disc1);
        float x1 = x_center + a1 * sqrt_disc;
        float x2 = x_center - a1 * sqrt_disc;
        drawFastHLine(x2, y, x1 - x2+1, color_ellipse_1);
        if(second_ellipse_offset>0)
        {
          drawFastHLine(x2+second_ellipse_offset, y, x1 - x2+1, color_ellipse_1);
        }

      }else
      {
        //inside the small ellipse

        //cross small ellipse
        // .....x_e1_0.....x_e0_0.....x_e0_1.....x_e1_1
        //
        float sqrt_disc0 = sqrt(disc0);
        float x_e0_0 = x_center - a0 * sqrt_disc0;
        float x_e0_1 = x_center + a0 * sqrt_disc0;

        float sqrt_disc1 = sqrt(disc1);
        float x_e1_0 = x_center - a1 * sqrt_disc1;
        float x_e1_1 = x_center + a1 * sqrt_disc1;
        
        //draw outter ellipse, the background ellipse
        drawFastHLine(x_e1_0, y, x_e0_0 - x_e1_0+1, color_ellipse_1);
        drawFastHLine(x_e0_1, y, x_e1_1 - x_e0_1+1, color_ellipse_1);
        if(second_ellipse_offset>0)
        {
          drawFastHLine(x_e1_0+second_ellipse_offset, y, x_e0_0 - x_e1_0+1, color_ellipse_1);
          drawFastHLine(x_e0_1+second_ellipse_offset, y, x_e1_1 - x_e0_1+1, color_ellipse_1);      
        }
        
        if(fill_ellipse0)
        {          
          //this fill the inside ellipse, this is where disc_he is used to fill inside with background ellipse 1 color
          if(disc_he<=0)
          {
            //not in the HE ellipse, fill e0
            drawFastHLine(x_e0_0, y, x_e0_1 - x_e0_0+1, color_ellipse_0);
            if(second_ellipse_offset>0)
            {
              drawFastHLine(x_e0_0+second_ellipse_offset, y, x_e0_1 - x_e0_0+1, color_ellipse_0);
            }
          }else
          {
            float sqrt_disc_HE = sqrt(disc_he);
            float x_eHE_0 = x_center - factorHE*a1 * sqrt_disc_HE;
            float x_eHE_1 = x_center + factorHE*a1 * sqrt_disc_HE;
            //HE cross small ellipse
            // .....x_e0_0.....x_eHE_0.....x_eHE_1.....x_e0_1
            if(x_e0_0<=x_eHE_0)
            {
              //up of the crossing of the ellipses
              drawFastHLine(x_e0_0, y, x_eHE_0 - x_e0_0+1, color_ellipse_0);//left portion inside e0 with color e0
              drawFastHLine(x_eHE_0, y, x_eHE_1 - x_eHE_0+1, color_ellipse_1);//center portion of the cutout
              drawFastHLine(x_eHE_1, y, x_e0_1 - x_eHE_1+1, color_ellipse_0);//right portion still inside the pupil
              if(second_ellipse_offset>0)
              {
                drawFastHLine(x_e0_0+second_ellipse_offset, y, x_eHE_0 - x_e0_0+1, color_ellipse_0);//left portion inside e0 with color e0
                drawFastHLine(x_eHE_0+second_ellipse_offset, y, x_eHE_1 - x_eHE_0+1, color_ellipse_1);//center portion of the cutout  
                drawFastHLine(x_eHE_1+second_ellipse_offset, y, x_e0_1 - x_eHE_1+1, color_ellipse_0);//right portion still inside the pupil
              }
            }else
            {
              //bellow, draw only inside e0, with e1 color
              drawFastHLine(x_e0_0, y, x_e0_1 - x_e0_0+1, color_ellipse_1);
              if(second_ellipse_offset>0)
              {
                drawFastHLine(x_e0_0+second_ellipse_offset, y, x_e0_1 - x_e0_0+1, color_ellipse_1);
              }
            }          
          }
        }   
      }
    }
  }
}


void drawFilledEllipseWithBackgroundBox(int x_center, int y_center, int a, int b, int w, int h, uint16_t color_ellipse, uint16_t color_background , bool draw_ellipse, int second_ellipse_offset = -1 ) {
  // raster draw a rectangle with an inscribed ellipse 
  // a: ellipse demi-grand axe, 
  // b: ellipse demi-petit axe
  // w  width of the rectangle
  // h height of the rectangle
  // color_ellipse ,  color of the inside of the ellipse
  // color_background, color of the inside of the rectangle
  // second_ellipse_offset,  offset to x_center for a second set of rectangle/ellipse if not -1, used to draw both eyes, simultaneously by setting second_ellipse_offset to something else that -1

  int x_start = x_center-w/2;
  int x_end = x_center+w/2;

  for (int y = y_center - h; y <= y_center + h; y++) {
    float y_ = y;
    float disc = (1 - (y_ - y_center)*(y_ - y_center) / (b*b));
    

    if (disc >= 0) {
        float sqrt_disc = sqrt(disc);
        float x1 = x_center + a * sqrt_disc;
        float x2 = x_center - a * sqrt_disc;

        //inside ellipse, draw the rectangular background around ellipse
        drawFastHLine(x_start, y, x2-x_start, color_background);
        drawFastHLine(x1, y, x_end-x1, color_background);
        if(second_ellipse_offset>0)
        {
          //draw second ellipse
          drawFastHLine(x_start+second_ellipse_offset, y, x2-x_start, color_background);
          drawFastHLine(x1+second_ellipse_offset, y, x_end-x1, color_background);        
        }

        if(draw_ellipse)
        {          
          //draw the actual ellipse
          drawFastHLine(x2, y, x1 - x2, color_ellipse);
          if(second_ellipse_offset>0)
          {
            drawFastHLine(x2+second_ellipse_offset, y, x1 - x2, color_ellipse);
          }
        
        }
        
    }else
    {
      //draw full
      drawFastHLine(x_start, y, w, color_background);
      if(second_ellipse_offset>0)
      {
        drawFastHLine(x_start+second_ellipse_offset, y, w, color_background);
      }
    }

    
  }
}
