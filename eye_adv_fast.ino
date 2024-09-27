//   Intellar.ca
//   LCD display used for eye animation inside a face 
//   check the blog on intellar.ca for all details
//   https://www.intellar.ca/
//    


#include <TFT_ST7735.h> // Hardware-specific library  https://github.com/Bodmer/TFT_ST7735
#include <SPI.h>
#include <avr/pgmspace.h>
#include "image.h"  

TFT_ST7735 tft = TFT_ST7735();       // Invoke library

#define RGB(r, g, b) (((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))

const int eye_pitch = 50;

const int eye_radius_x_ref = 25;
const int eye_radius_y_ref = eye_radius_x_ref*1.2;
const int eye_pos_x_ref = 0;
const int eye_pos_y_ref = 0;

const int eye_offset_happy_ref = -1;
const int eye_center_x_ref = 95;
  


const uint16_t c_bgnd = RGB(0,0,0);
const uint16_t c_0 = RGB(0,0,0);
const uint16_t c_1 = RGB(0,0,0);
const uint16_t c_2 = RGB(0,0,0);
const uint16_t c_4 = RGB(0,0,0);

int eye_radius_x = eye_radius_x_ref;
int eye_radius_y = eye_radius_y_ref;
int eye_pos_x = eye_pos_x_ref;
int eye_pos_y = eye_pos_y_ref;
int eye_offset_happy = eye_offset_happy_ref;
int eye_center_x = eye_center_x_ref;

void eye_reset()
{
  eye_radius_x = eye_radius_x_ref;
  eye_radius_y = eye_radius_y_ref;
  eye_pos_x = eye_pos_x_ref;
  eye_pos_y = eye_pos_y_ref;
  eye_offset_happy = eye_offset_happy_ref;
  eye_center_x = eye_center_x_ref;
}

// https://notisrac.github.io/FileToCArray/
#define FACE2_SM_HEIGHT 64
#define FACE2_SM_WIDTH 70

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
        tft.drawFastHLine(x2, y, x1 - x2, color_ellipse_1);
        if(second_ellipse_offset>0)
        {
          tft.drawFastHLine(x2+second_ellipse_offset, y, x1 - x2, color_ellipse_1);
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
        tft.drawFastHLine(x_e1_0, y, x_e0_0 - x_e1_0, color_ellipse_1);
        tft.drawFastHLine(x_e0_1, y, x_e1_1 - x_e0_1, color_ellipse_1);
        if(second_ellipse_offset>0)
        {
          tft.drawFastHLine(x_e1_0+second_ellipse_offset, y, x_e0_0 - x_e1_0, color_ellipse_1);
          tft.drawFastHLine(x_e0_1+second_ellipse_offset, y, x_e1_1 - x_e0_1, color_ellipse_1);      
        }
        
        if(fill_ellipse0)
        {          
          //this fill the inside ellipse, this is where disc_he is used to fill inside with background ellipse 1 color
          if(disc_he<=0)
          {
            //not in the HE ellipse, fill e0
            tft.drawFastHLine(x_e0_0, y, x_e0_1 - x_e0_0, color_ellipse_0);
            if(second_ellipse_offset>0)
            {
              tft.drawFastHLine(x_e0_0+second_ellipse_offset, y, x_e0_1 - x_e0_0, color_ellipse_0);
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
              tft.drawFastHLine(x_e0_0, y, x_eHE_0 - x_e0_0, color_ellipse_0);//left portion inside e0 with color e0
              tft.drawFastHLine(x_eHE_0, y, x_eHE_1 - x_eHE_0, color_ellipse_1);//center portion of the cutout
              tft.drawFastHLine(x_eHE_1, y, x_e0_1 - x_eHE_1, color_ellipse_0);//right portion still inside the pupil
              if(second_ellipse_offset>0)
              {
                tft.drawFastHLine(x_e0_0+second_ellipse_offset, y, x_eHE_0 - x_e0_0, color_ellipse_0);//left portion inside e0 with color e0
                tft.drawFastHLine(x_eHE_0+second_ellipse_offset, y, x_eHE_1 - x_eHE_0, color_ellipse_1);//center portion of the cutout  
                tft.drawFastHLine(x_eHE_1+second_ellipse_offset, y, x_e0_1 - x_eHE_1, color_ellipse_0);//right portion still inside the pupil
              }
            }else
            {
              //bellow, draw only inside e0, with e1 color
              tft.drawFastHLine(x_e0_0, y, x_e0_1 - x_e0_0+1, color_ellipse_1);
              if(second_ellipse_offset>0)
              {
                tft.drawFastHLine(x_e0_0+second_ellipse_offset, y, x_e0_1 - x_e0_0+1, color_ellipse_1);
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
        tft.drawFastHLine(x_start, y, x2-x_start, color_background);
        tft.drawFastHLine(x1, y, x_end-x1, color_background);
        if(second_ellipse_offset>0)
        {
          //draw second ellipse
          tft.drawFastHLine(x_start+second_ellipse_offset, y, x2-x_start, color_background);
          tft.drawFastHLine(x1+second_ellipse_offset, y, x_end-x1, color_background);        
        }

        if(draw_ellipse)
        {          
          //draw the actual ellipse
          tft.drawFastHLine(x2, y, x1 - x2, color_ellipse);
          if(second_ellipse_offset>0)
          {
            tft.drawFastHLine(x2+second_ellipse_offset, y, x1 - x2, color_ellipse);
          }
        
        }
        
    }else
    {
      //draw full
      tft.drawFastHLine(x_start, y, w, color_background);
      if(second_ellipse_offset>0)
      {
        tft.drawFastHLine(x_start+second_ellipse_offset, y, w, color_background);
      }
    }

    
  }
}


void draw_eyes(int x, int y, int offset_second_eye, int offset_happy_eye)
{
  //draw both eyes with a serie of inscribed ellipses
  drawFilledInscribedEllipse(        x,y,0.9*eye_radius_x,0.9*eye_radius_y,1.0*eye_radius_x,  1.2*eye_radius_y,RGB(1,3, 86),RGB(0,0,0),false,offset_second_eye );
  drawFilledInscribedEllipse(        x,y,0.7*eye_radius_x,0.7*eye_radius_y,0.9*eye_radius_x,  0.9*eye_radius_y,RGB(1,4,172),RGB(1,3, 86),false,offset_second_eye );
  drawFilledInscribedEllipse(        x,y,0.5*eye_radius_x,0.5*eye_radius_y,0.7*eye_radius_x,  0.7*eye_radius_y,RGB(1,178,226),RGB(1,4,172),true,offset_second_eye, offset_happy_eye);
}

void draw_eyes_no_raster(int x, int y, int offset_second_eye)
{
  //for demo purpose, draw without raster
  tft.fillEllipse(x,y,eye_radius_x*0.9,eye_radius_y*0.9,RGB(1,3,86));
  tft.fillEllipse(x,y,eye_radius_x*0.7,eye_radius_y*0.7,RGB(1,4,172));
  tft.fillEllipse(x,y,eye_radius_x*0.5,eye_radius_y*0.5,RGB(1,178,226));

  tft.fillEllipse(x+offset_second_eye,y,eye_radius_x*0.9,eye_radius_y*0.9,RGB(1,3,86));
  tft.fillEllipse(x+offset_second_eye,y,eye_radius_x*0.7,eye_radius_y*0.7,RGB(1,4,172));
  tft.fillEllipse(x+offset_second_eye,y,eye_radius_x*0.5,eye_radius_y*0.5,RGB(1,178,226));
  
}




void eye_make_happy(bool reverse_to_normal=false)
{
  float offset_temp = 0;
  float offset_happy_temp = 0;

  for( eye_offset_happy = 40;eye_offset_happy>=20;eye_offset_happy-=2)
  {
    //change order, and use same variable to control the width/height of the eyes
    //40->20 : 0->6
    offset_temp = reverse_to_normal?(40-eye_offset_happy)+20:eye_offset_happy;
    offset_happy_temp = reverse_to_normal?(40-eye_offset_happy)+20:eye_offset_happy;
    offset_temp = (40-offset_temp)/20*6;
    eye_radius_x = eye_radius_x_ref+offset_temp;
    eye_radius_y = eye_radius_y_ref-offset_temp;

    draw_eyes(eye_center_x-eye_pitch, 72, eye_pitch,offset_happy_temp );
  }
}


void demo_draw_eyes(  )
{  
  eye_reset();
  draw_eyes(eye_center_x-eye_pitch, 72, eye_pitch,eye_offset_happy );
  delay(2000);
  eye_blink();
  eye_reset();
  delay(500);
  eye_blink();
  eye_make_happy();  
  delay(500);
  eye_make_happy(true);
  delay(500);
}
void eye_blink()
{
  int eye_center_x = 95;
  int i=0;
  float ratio[] = {1.0, 0.85, 0.75, 0.55, 0.4, 0.3, 0.2, 0.1, 0.05};
  int nb_ratio = 9;
  for(i=0;i<nb_ratio;i++)
  {
    eye_radius_y = eye_radius_y_ref*ratio[i];
    draw_eyes(eye_center_x-eye_pitch, 72, eye_pitch, -1 );
  }
  for(i=0;i<nb_ratio;i++)
  {
    eye_radius_y = eye_radius_y_ref*ratio[nb_ratio-i-1];
    draw_eyes(eye_center_x-eye_pitch, 72, eye_pitch, -1 );
  }
}

void draw_image()
{
  int x=0;
  int y=0;
  int w = FACE2_SM_WIDTH;
  int h = FACE2_SM_HEIGHT;

  for(uint16_t i=0;i<h;i++)
  {
    for(uint16_t j=0;j<w;j++)
    {
      uint16_t v = pgm_read_word(&face2_sm[i*w+j]);
      tft.drawPixel(2*j,2*i,v);
      tft.drawPixel(2*j,2*i+1,v);
      tft.drawPixel(2*j+1,2*i,v);
      tft.drawPixel(2*j+1,2*i+1,v);
    }
  }

}
void setup() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);

  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(1);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0, 2);
  tft.println(F("Eyes animation demo"));
  delay(2000);
  draw_image();
}

void loop() {
  
  
  demo_draw_eyes();
  
  //int max_iter=0;
  //demo_raster(max_iter=6);  
  //demo_raster(max_iter=1); 
}


void demo_raster(int max_iter)
{
  int index_ratio=0;
  float ratio[] = {1.0, 0.85, 0.75, 0.55, 0.4, 0.3, 0.2, 0.1, 0.05};
  int nb_ratio = 9;
  int i=0;
 
  tft.setCursor(0, 0, 2);
  tft.println(F("attempt 1."));
  //tft.println("clear and redraw");
  if(max_iter>1)
    delay(2000);

  for(int iter=0;iter<max_iter;iter++)
  {

    
    for(i=0;i<nb_ratio;i++)
    {
      eye_radius_y = eye_radius_y_ref*ratio[i];
      tft.fillScreen(ST7735_BLACK);
      draw_eyes_no_raster(eye_center_x-eye_pitch, 72,eye_pitch);
    }
    for(i=0;i<nb_ratio;i++)
    {
      eye_radius_y = eye_radius_y_ref*ratio[nb_ratio-i-1];
      tft.fillScreen(ST7735_BLACK);
      draw_eyes_no_raster(eye_center_x-eye_pitch, 72,eye_pitch);
    }    

  }
  
  tft.setCursor(0, 0, 2);
  tft.println(F("attempt 2."));
  //tft.println("raster black background and ellipses");
  if(max_iter>1)
    delay(2000);
  tft.fillScreen(ST7735_BLACK);
  for(int iter=0;iter<max_iter;iter++)
  {
    
    for(i=0;i<nb_ratio;i++)
    {
      eye_radius_y = eye_radius_y_ref*ratio[i];
      
      drawFilledInscribedEllipse(        eye_center_x-eye_pitch,72,0.9*eye_radius_x,0.9*eye_radius_y,1.0*eye_radius_x,  1.2*eye_radius_y,RGB(1,3, 86),RGB(0,0,0),false  );
      drawFilledInscribedEllipse(        eye_center_x-eye_pitch,72,0.7*eye_radius_x,0.7*eye_radius_y,0.9*eye_radius_x,  0.9*eye_radius_y,RGB(1,4,172),RGB(1,3, 86),false );
      drawFilledInscribedEllipse(        eye_center_x-eye_pitch,72,0.5*eye_radius_x,0.5*eye_radius_y,0.7*eye_radius_x,  0.7*eye_radius_y,RGB(1,178,226),RGB(1,4,172),true );

      drawFilledInscribedEllipse(        eye_center_x,72,0.9*eye_radius_x,0.9*eye_radius_y,1.0*eye_radius_x,  1.2*eye_radius_y,RGB(1,3, 86),RGB(0,0,0),false  );
      drawFilledInscribedEllipse(        eye_center_x,72,0.7*eye_radius_x,0.7*eye_radius_y,0.9*eye_radius_x,  0.9*eye_radius_y,RGB(1,4,172),RGB(1,3, 86),false );
      drawFilledInscribedEllipse(        eye_center_x,72,0.5*eye_radius_x,0.5*eye_radius_y,0.7*eye_radius_x,  0.7*eye_radius_y,RGB(1,178,226),RGB(1,4,172),true );


    }
    for(i=0;i<nb_ratio;i++)
    {
      eye_radius_y = eye_radius_y_ref*ratio[nb_ratio-i-1];
      drawFilledInscribedEllipse(        eye_center_x-eye_pitch,72,0.9*eye_radius_x,0.9*eye_radius_y,1.0*eye_radius_x,  1.2*eye_radius_y,RGB(1,3, 86),RGB(0,0,0),false  );
      drawFilledInscribedEllipse(        eye_center_x-eye_pitch,72,0.7*eye_radius_x,0.7*eye_radius_y,0.9*eye_radius_x,  0.9*eye_radius_y,RGB(1,4,172),RGB(1,3, 86),false );
      drawFilledInscribedEllipse(        eye_center_x-eye_pitch,72,0.5*eye_radius_x,0.5*eye_radius_y,0.7*eye_radius_x,  0.7*eye_radius_y,RGB(1,178,226),RGB(1,4,172),true );

      drawFilledInscribedEllipse(        eye_center_x,72,0.9*eye_radius_x,0.9*eye_radius_y,1.0*eye_radius_x,  1.2*eye_radius_y,RGB(1,3, 86),RGB(0,0,0),false  );
      drawFilledInscribedEllipse(        eye_center_x,72,0.7*eye_radius_x,0.7*eye_radius_y,0.9*eye_radius_x,  0.9*eye_radius_y,RGB(1,4,172),RGB(1,3, 86),false );
      drawFilledInscribedEllipse(        eye_center_x,72,0.5*eye_radius_x,0.5*eye_radius_y,0.7*eye_radius_x,  0.7*eye_radius_y,RGB(1,178,226),RGB(1,4,172),true );
    }   
  }


  tft.setCursor(0, 0, 2);
  tft.println(F("attempt 3."));
  //tft.println("raster simultaneously");
  if(max_iter>1)
    delay(2000);
  tft.fillScreen(ST7735_BLACK);
  for(int iter=0;iter<max_iter;iter++)
  {
    for(i=0;i<nb_ratio;i++)
    {
      eye_radius_y = eye_radius_y_ref*ratio[i];
      drawFilledInscribedEllipse(        eye_center_x-eye_pitch,72,0.9*eye_radius_x,0.9*eye_radius_y,1.0*eye_radius_x,  1.2*eye_radius_y,RGB(1,3, 86),RGB(0,0,0),false,eye_pitch  );
      drawFilledInscribedEllipse(        eye_center_x-eye_pitch,72,0.7*eye_radius_x,0.7*eye_radius_y,0.9*eye_radius_x,  0.9*eye_radius_y,RGB(1,4,172),RGB(1,3, 86),false,eye_pitch );
      drawFilledInscribedEllipse(        eye_center_x-eye_pitch,72,0.5*eye_radius_x,0.5*eye_radius_y,0.7*eye_radius_x,  0.7*eye_radius_y,RGB(1,178,226),RGB(1,4,172),true,eye_pitch );

    }
    for(i=0;i<nb_ratio;i++)
    {
      eye_radius_y = eye_radius_y_ref*ratio[nb_ratio-i-1];
      drawFilledInscribedEllipse(        eye_center_x-eye_pitch,72,0.9*eye_radius_x,0.9*eye_radius_y,1.0*eye_radius_x,  1.2*eye_radius_y,RGB(1,3, 86),RGB(0,0,0),false,eye_pitch  );
      drawFilledInscribedEllipse(        eye_center_x-eye_pitch,72,0.7*eye_radius_x,0.7*eye_radius_y,0.9*eye_radius_x,  0.9*eye_radius_y,RGB(1,4,172),RGB(1,3, 86),false,eye_pitch );
      drawFilledInscribedEllipse(        eye_center_x-eye_pitch,72,0.5*eye_radius_x,0.5*eye_radius_y,0.7*eye_radius_x,  0.7*eye_radius_y,RGB(1,178,226),RGB(1,4,172),true,eye_pitch );
    }   
  }
}


