#include <esp32-hal-ledc.h>

const int MotPin0 = 12;  
const int MotPin1 = 13;  
const int MotPin2 = 14;  
const int MotPin3 = 15;  

void initMotors() 
{
  ledcSetup(3, 2000, 8); // 2000 hz PWM, 8-bit resolution
  ledcSetup(4, 2000, 8); // 2000 hz PWM, 8-bit resolution
  ledcSetup(5, 2000, 8); // 2000 hz PWM, 8-bit resolution
  ledcSetup(6, 2000, 8); // 2000 hz PWM, 8-bit resolution
  ledcAttachPin(MotPin0, 3); 
  ledcAttachPin(MotPin1, 4); 
  ledcAttachPin(MotPin2, 5); 
  ledcAttachPin(MotPin3, 6); 
}

enum state {fwd,rev,stp};
state actstate = stp;

enum dir {left,center,right};
dir actdir = center;

int motspeed = 150;
int turnspeed = 150;

void domot()
{
  int lmot,rmot;
  
  if (actstate == fwd) 
  {
    rmot = motspeed;
    lmot = motspeed;
  }
  if (actstate == rev)
  {
    rmot = -motspeed;
    lmot = -motspeed;
  }
  if (actstate == stp) lmot = rmot =  0;
  
  if (actdir == right) 
  {
    rmot -= turnspeed;
    lmot += turnspeed;
  }
  if (actdir == left)  
  {
    rmot += turnspeed;
    lmot -= turnspeed;
  }
  
  rmot = constrain(rmot,-255,255);
  lmot = constrain(lmot,-255,255);
  
  if (rmot == 0) 
  {
    ledcWrite(3, 0);
    ledcWrite(4, 0);      
  }
  else if (rmot > 0) 
  {
    ledcWrite(3,0);    // pin 13
    ledcWrite(4,rmot); // pin 12
  }
  else if (rmot < 0) 
  {
    ledcWrite(4,0);
    ledcWrite(3,-rmot); 
  }
  
  if (lmot == 0) 
  {
    ledcWrite(5, 0);
    ledcWrite(6, 0);      
  }
  else if (lmot > 0) 
  {
    ledcWrite(6,0);
    ledcWrite(5,lmot); 
  }
  else if (lmot < 0) 
  {
    ledcWrite(5,0);   
    ledcWrite(6,-lmot); 
  }
}

void motcmd(int val, bool down)
{
  // 38 fwd, 40 back, 37 left, 39 right, 12 stop
  if (val==38){if (down) actstate = fwd; else actstate = stp;}
  if (val==40){if (down) actstate = rev; else actstate = stp;}
  if (val==37){if (down) actdir = left;  else actdir = center;}
  if (val==39){if (down) actdir = right; else actdir = center;}
  if (val==12){actstate = stp; actdir = center;}
  domot();
}

void motstop()
{
    motcmd(12,true);
}
