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
  motstop();
}

void runmot(int lmot, int rmot)
{
  rmot = constrain(rmot,-255,255);
  lmot = constrain(lmot,-255,255);  
  if (debugvalue == 'm') Serial.printf("%4d %4d \n",lmot, rmot);
  
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

extern int16_t rcValue[];
void domotudp()
{
  int lmot,rmot,turn,thro;
  // rcValue[0] direction, rcValue[2] speed, rcValue[4] fwd/rev
  thro = map(rcValue[2],1000,2000,0,255);    // 1000 to 255
  turn = map(rcValue[0],1000,2000,-255,255); // +-500 to +-255
  if (rcValue[4] < 1500) // FWD
  {
    lmot = thro + turn;    
    rmot = thro - turn;    
  }
  else
  {
    lmot = -thro + turn ;    
    rmot = -thro - turn ;    
  }
  runmot(lmot, rmot);
  
  if (rcValue[5] > 1500) led_bright(100);
  else                   led_bright(0);
}


void motstop()
{
  runmot(0, 0);
  led_bright(0);
}

void led_bright(int in)
{
  ledcWrite(7, in);
}
