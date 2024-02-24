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
state newstate = stp;
state actstate = stp;

enum dir {left,center,right};
dir newdir = center;
dir actdir = center;

int motspeed = 150;
bool moving;

#define rspeedfact 1.0  // use to equalize different motor turn direction
#define lspeedfact 1.0  // use to equalize different motor turn direction

void domot()
{
  int lmot,rmot;
  
  if (actstate == fwd) 
  {
    rmot = motspeed * rspeedfact;
    lmot = motspeed * lspeedfact;
  }
  if (actstate == rev)
  {
    rmot = -motspeed * lspeedfact;
    lmot = -motspeed * rspeedfact;
  }
  if (actstate == stp) lmot = rmot =  0;
  
  if (actdir == right) 
  {
    rmot -= motspeed;
    lmot += motspeed;
  }
  if (actdir == left)  
  {
    rmot += motspeed;
    lmot -= motspeed;
  }

  rmot = constrain(rmot,-255,255);
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
  
  lmot = constrain(lmot,-255,255);
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
  moving = ((rmot != 0) || (lmot != 0)); 
}

int turnct = 0;

void motcmd(int val)
{
  if      (val==1)   newstate = fwd;
  else if (val==2) { newdir = left;  turnct = 8; }
  else if (val==4) { newdir = right; turnct = 8; }
  else if (val==5)   newstate = rev;
  else             { newstate = stp; Serial.println("MotStop"); } 
}

void motcyclic()
{
  if (newstate != actstate) 
  { 
    if ((actstate != stp) && (newstate != stp)) actstate = stp;
    else                                        actstate = newstate;
    domot();
  }
  if (newdir != actdir) 
  {
    actdir = newdir;   
    domot();
  }
  if (turnct > 0)
  {
    turnct--;
    if (turnct == 0) newdir = center;
  }
}

void motstop()
{
    newstate = stp;
    motcyclic(); // stop motors   
}
