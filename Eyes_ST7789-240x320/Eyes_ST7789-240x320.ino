//   Intellar.ca
//   LCD display used for eye animation inside a face 
//   check the blog on intellar.ca for all details
//   https://www.intellar.ca/
//   
//   copyright intellar.ca  
//   free for non commercial use. contact author intellar@intellar.ca for a commercial licence. 
//   check licence.md for details
//   


#include <avr/pgmspace.h>

#include "img_rle.h"  
#include "compression.h"
#include "drawing_tools.h"

#define SCR_WD   240
#define SCR_HT   320
#define RGB(r, g, b) (((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))




struct Eye
{
  float radius_x = 38;
  float radius_y = 38*1.2;
  float pos_x = 80;
  float pos_y = 142;    
  int offset_happy = -1;
  int eye_pitch = 40;
};



const Eye eye_ref;
Eye eyes[2];





void eye_reset()
{
  eyes[0] = eye_ref;
  eyes[1] = eye_ref;
  //adjust position x  
  eyes[1].pos_x = eyes[0].pos_x+2*eyes[0].eye_pitch;
}

static const uint16_t cbg = RGB(0,0, 0);
static const uint16_t c0 = RGB(1,3, 86);
static const uint16_t c1 = RGB(1,4, 172);
static const uint16_t c2 = RGB(1,178, 226);



void draw_eyes(Eye eye_to_draw, int offset_second_eye)
{
  //draw both eyes with a serie of inscribed ellipses, draw second eye exactly same as first, if offset is not -1
  drawFilledInscribedEllipse(        eye_to_draw.pos_x,eye_to_draw.pos_y,0.7*eye_to_draw.radius_x,0.7*eye_to_draw.radius_y,1.0*eye_to_draw.radius_x,  1.1*eye_to_draw.radius_y,c0,cbg,  false,offset_second_eye );
  //drawFilledInscribedEllipse(        eye_to_draw.pos_x,eye_to_draw.pos_y,0.7*eye_to_draw.radius_x,0.7*eye_to_draw.radius_y,0.9*eye_to_draw.radius_x,  0.9*eye_to_draw.radius_y,RGB(1,4,172),  RGB(1,3,86), false,offset_second_eye );
  drawFilledInscribedEllipse(        eye_to_draw.pos_x,eye_to_draw.pos_y,0.5*eye_to_draw.radius_x,0.5*eye_to_draw.radius_y,0.7*eye_to_draw.radius_x,  0.7*eye_to_draw.radius_y,c2,c0,true,offset_second_eye, eye_to_draw.offset_happy);
}



float interpolate_at(float val_ini, float val_final, int nb_step, int current_step)
{
  return val_ini + (val_final - val_ini)/nb_step*current_step;

}

void clear_eyes()
{
  fillEllipse(eyes[0].pos_x,eyes[0].pos_y, eyes[0].radius_x, eyes[0].radius_y*1.1, cbg);
  fillEllipse(eyes[1].pos_x,eyes[1].pos_y, eyes[1].radius_x, eyes[1].radius_y*1.1, cbg);

}
void draw_hearth(Eye eye, int circle_radius_ref)
{
  uint16_t c = RGB(224,12,12);
  int x = eye.pos_x;
  int y = eye.pos_y-5;
  
  int nb_step=8;
  for(int i=0;i<nb_step;i++)
  {
    int circle_radius = interpolate_at(circle_radius_ref,circle_radius_ref*1.75,nb_step,i);

    int offset_circle = circle_radius;
    fillEllipse(x-offset_circle,y,circle_radius,circle_radius,c);
    fillEllipse(x+offset_circle,y,circle_radius,circle_radius,c);
    // Given vertices
    int x0 = x-(offset_circle+circle_radius), y0 = y+circle_radius/3;
    int x1 = x+(offset_circle+circle_radius), y1 = y+circle_radius/3;
    int x2 = x, y2 = y+3*circle_radius;
    draw_triangle(x0,y0,x1,y1,x2,y2,c);

  }
  

}

void draw_look_at(int tx=5)
{
  //tx is the amplitude and direction of the movement
  //tx = 5,  move and look on the right with movement of +5
  //tx = -5,  move and look on the left with movement of -5

  int nb_step=4;
  Eye c_eye0 = eyes[0];
  Eye c_eye1 = eyes[1];  


  int ry = 24;
  int ry_oversize0 = tx>=0?0:18;
  int ry_oversize1 = tx>=0?18:0;

  for(int i=0;i<nb_step;i++)
  {
    eyes[0].pos_x = interpolate_at(c_eye0.pos_x,c_eye0.pos_x+tx,nb_step,i);
    eyes[0].radius_y = interpolate_at(c_eye0.radius_y,c_eye0.radius_y-ry,nb_step,i);
    eyes[1].pos_x = interpolate_at(c_eye1.pos_x,c_eye1.pos_x+tx,nb_step,i);
    eyes[1].radius_y = interpolate_at(c_eye1.radius_y,c_eye1.radius_y-ry,nb_step,i);
    draw_eyes(eyes[0],-1);
    draw_eyes(eyes[1],-1);
  }
  c_eye0 = eyes[0];
  c_eye1 = eyes[1];
  for(int i=0;i<nb_step;i++)
  {
    eyes[0].pos_x = interpolate_at(c_eye0.pos_x,c_eye0.pos_x+tx,nb_step,i);
    eyes[0].radius_x = interpolate_at(c_eye0.radius_x,c_eye0.radius_x,nb_step,i);
    eyes[0].radius_y = interpolate_at(c_eye0.radius_y,c_eye0.radius_y+ry+ry_oversize0,nb_step,i);
    eyes[1].pos_x = interpolate_at(c_eye1.pos_x,c_eye1.pos_x+tx,nb_step,i);
    eyes[1].radius_y = interpolate_at(c_eye1.radius_y,c_eye1.radius_y+ry+ry_oversize1,nb_step,i);
    draw_eyes(eyes[0],-1);
    draw_eyes(eyes[1],-1);
  }
  c_eye0 = eyes[0];
  c_eye1 = eyes[1];
  for(int i=0;i<nb_step;i++)
  {
    eyes[0].pos_x = interpolate_at(c_eye0.pos_x,c_eye0.pos_x-tx,nb_step,i);
    eyes[0].radius_x = interpolate_at(c_eye0.radius_x,c_eye0.radius_x,nb_step,i);
    eyes[0].radius_y = interpolate_at(c_eye0.radius_y,c_eye0.radius_y-ry-ry_oversize0,nb_step,i);
    eyes[1].pos_x = interpolate_at(c_eye1.pos_x,c_eye1.pos_x-tx,nb_step,i);
    eyes[1].radius_y = interpolate_at(c_eye1.radius_y,c_eye1.radius_y-ry-ry_oversize1,nb_step,i);
    draw_eyes(eyes[0],-1);
    draw_eyes(eyes[1],-1);
  }
  c_eye0 = eyes[0];
  c_eye1 = eyes[1];
  for(int i=0;i<nb_step;i++)
  {
    eyes[0].pos_x = interpolate_at(c_eye0.pos_x,c_eye0.pos_x-tx,nb_step,i);
    eyes[0].radius_y = interpolate_at(c_eye0.radius_y,c_eye0.radius_y+ry,nb_step,i);
    eyes[1].pos_x = interpolate_at(c_eye1.pos_x,c_eye1.pos_x-tx,nb_step,i);
    eyes[1].radius_y = interpolate_at(c_eye1.radius_y,c_eye1.radius_y+ry,nb_step,i);
    draw_eyes(eyes[0],-1);
    draw_eyes(eyes[1],-1);
  }

}

void draw_eyes_surprised_helper(float f0,float f1,float f2, float f3, float f4, float f5, float f6, float f7, int nb_step, int i)
{
    eyes[0].radius_x = interpolate_at(f0*eye_ref.radius_x, f1*eye_ref.radius_x, nb_step, i);        
    eyes[0].radius_y = interpolate_at(f2*eye_ref.radius_y, f3*eye_ref.radius_y, nb_step, i);
    eyes[1].radius_x = interpolate_at(f4*eye_ref.radius_x, f5*eye_ref.radius_x, nb_step, i);
    eyes[1].radius_y = interpolate_at(f6*eye_ref.radius_y, f7*eye_ref.radius_y, nb_step, i);
}

void draw_eyes_surprised()
{
  eye_reset();
  
  int nb_step = 3;
  float f = 1.2;
  for(int i=0;i<nb_step;i++)
  {
    
    //eyes[0].radius_x = interpolate_at(eye_ref.radius_x, f*eye_ref.radius_x, nb_step, i);        
    //eyes[0].radius_y = interpolate_at(eye_ref.radius_y, f*eye_ref.radius_y, nb_step, i);
    //eyes[1].radius_x = interpolate_at(eye_ref.radius_x, f*eye_ref.radius_x, nb_step, i);
    //eyes[1].radius_y = interpolate_at(eye_ref.radius_y, f*eye_ref.radius_y, nb_step, i);
    draw_eyes_surprised_helper(1,f,1,f,1,f,1,f, nb_step, i);
    draw_eyes(eyes[0],2*eyes[0].eye_pitch);
    //draw_eyes(eyes[1],-1); 
  }
  nb_step = 3;
  for(int i=0;i<nb_step;i++)
  {
    //eyes[0].radius_x = interpolate_at(f*eye_ref.radius_x, eye_ref.radius_x, nb_step, i);        
    //eyes[0].radius_y = interpolate_at(f*eye_ref.radius_y, eye_ref.radius_y, nb_step, i);
    //eyes[1].radius_x = interpolate_at(f*eye_ref.radius_x, eye_ref.radius_x, nb_step, i);
    //eyes[1].radius_y = interpolate_at(f*eye_ref.radius_y, eye_ref.radius_y, nb_step, i);
    draw_eyes_surprised_helper(f,1,f,1,f,1,f,1, nb_step, i);
    draw_eyes(eyes[0],2*eyes[0].eye_pitch);
    //draw_eyes(eyes[0],-1);      
    //draw_eyes(eyes[1],-1); 
  }


  nb_step = 5;
  for(int i=0;i<nb_step;i++)
  {
    //eyes[0].radius_x = interpolate_at(eye_ref.radius_x, 1.2*eye_ref.radius_x, nb_step, i);        
    //eyes[0].radius_y = interpolate_at(eye_ref.radius_y, 1.4*eye_ref.radius_y, nb_step, i);
    //eyes[1].radius_x = interpolate_at(eye_ref.radius_x, 0.5*eye_ref.radius_x, nb_step, i);
    //eyes[1].radius_y = interpolate_at(eye_ref.radius_y, 0.5*eye_ref.radius_y, nb_step, i);    
    draw_eyes_surprised_helper(1,1.2,1,1.4,1,0.5,1,0.5,nb_step,i);
    
    draw_eyes(eyes[0],-1);      
    draw_eyes(eyes[1],-1); 
  }

  nb_step = 3;
  for(int i=0;i<nb_step;i++)
  {
    //eyes[0].radius_x = interpolate_at(1.2*eye_ref.radius_x, eye_ref.radius_x, nb_step, i);        
    //eyes[0].radius_y = interpolate_at(1.4*eye_ref.radius_y, eye_ref.radius_y, nb_step, i);
    //eyes[1].radius_x = interpolate_at(0.5*eye_ref.radius_x, eye_ref.radius_x, nb_step, i);
    //eyes[1].radius_y = interpolate_at(0.5*eye_ref.radius_y, eye_ref.radius_y, nb_step, i);    
    draw_eyes_surprised_helper(1.2,1,1.4,1,0.5,1,0.5,1,nb_step,i);
    draw_eyes(eyes[0],-1);      
    draw_eyes(eyes[1],-1); 
  }

}

void eye_make_happy(bool one_way)
{
  float offset_temp = 0;
  float offset_happy_temp = 0;
  int nb_step = 7;

  for( int i=0;i<nb_step;i++)
  {
    eyes[0].offset_happy = interpolate_at(80,35,nb_step,i);
    draw_eyes(eyes[0],2*eyes[0].eye_pitch);  
  }
  if(one_way)
    return;

  delay(800);
  for( int i=0;i<nb_step;i++)
  {
    eyes[0].offset_happy = interpolate_at(35,80,nb_step,i);
    draw_eyes(eyes[0],2*eyes[0].eye_pitch);  
  }
}


void eye_wink(int eye_index)
{
  float offset_temp = 0;
  float offset_happy_temp = 0;
  int nb_step = 7;

  for( int i=0;i<nb_step;i++)
  {
    eyes[eye_index].offset_happy = interpolate_at(80,35,nb_step,i);
    draw_eyes(eyes[eye_index],-1);  
  }
  
  for( int i=0;i<nb_step;i++)
  {
    eyes[0].offset_happy = interpolate_at(35,80,nb_step,i);
    draw_eyes(eyes[eye_index],-1);
  }
}

void wake_up()
{
  int nb_step=10;
  for(int i=0;i<nb_step;i++)
  {
    eyes[0].radius_x = interpolate_at(eye_ref.radius_x*1.5, eye_ref.radius_x, nb_step, i);
    eyes[0].radius_y = interpolate_at(eye_ref.radius_y/8, eye_ref.radius_y, nb_step, i);
    draw_eyes(eyes[0],2*eyes[0].eye_pitch);
    
  }
  
}
void sleep()
{
    eyes[0].radius_x = eye_ref.radius_x*1.5;
    eyes[0].radius_y = eye_ref.radius_y/8;
    draw_eyes(eyes[0],2*eyes[0].eye_pitch);
    
}


void setup() {
  
  init_tft();  
  eye_reset(); 
  draw_image_rle_conv(img_rle_0, img_rle_0_length, img_height, img_width);  
}



void loop() {
  
  clear_eyes();
  sleep();
  delay(500);
  eye_reset();
  wake_up();
  
  draw_look_at(6);
  delay(250);
  //draw_look_at(-6);
  //delay(250);
  eye_make_happy(true);
  delay(700); 
  //draw_eyes(eyes[0],2*eyes[0].eye_pitch);
  clear_eyes();
  draw_hearth(eyes[0],10);
  draw_hearth(eyes[1],10);
  delay(1000);  
  clear_eyes();
  //eye_make_happy(one_way=true);

  
  //eye_wink(0);
  
  draw_eyes_surprised();
  draw_look_at(6);
  draw_look_at(-6);
  draw_look_at(6);
  draw_look_at(-6);
  
  return;
  
}
