
#define CHANNELS 6

int16_t rcValue[CHANNELS];
unsigned long nextRCtime;    

//-----------------------------------------------

#include <WiFiUdp.h>
WiFiUDP Udp;
unsigned int localUdpPort = 4210;  //  port to listen on
byte rxPacket[32];  // buffer for incoming packets

void init_RC()
{
  Udp.begin(localUdpPort);
  Serial.printf("UDP port %d\n", localUdpPort);
}

void rcvalCyclic()
{
  int packetSize = Udp.parsePacket();
  if (packetSize == 0) return;
  if (packetSize == 14)
  {
    Udp.read(rxPacket, 14);  
    if (rxPacket[0] == 0x55)
    {
      // flysky roll=0 pitch=1 thr=2 yaw=3 
      rcValue[0] = (rxPacket[ 2]<<8) + rxPacket[ 3]; 
      rcValue[1] = (rxPacket[ 4]<<8) + rxPacket[ 5]; 
      rcValue[2] = (rxPacket[ 6]<<8) + rxPacket[ 7]; 
      rcValue[3] = (rxPacket[ 8]<<8) + rxPacket[ 9]; 
      rcValue[4] = (rxPacket[10]<<8) + rxPacket[11]; 
      rcValue[5] = (rxPacket[12]<<8) + rxPacket[13]; 

      gotrc = true;
      nextRCtime = millis() + 250; // 250ms

      if (debugvalue == 'r')
      {
        Serial.printf("%4d %4d %4d ",  rcValue[0], rcValue[1], rcValue[2]);
        Serial.printf("%4d %4d %4d \n",rcValue[3], rcValue[4], rcValue[5]);
      }
    }
    else if (nextRCtime < millis()) 
    {
      motstop();
      Serial.println("RC timeout disarm");
    }
  }
  else 
  {
    Udp.flush(); 
    Serial.println("Wrong UDP size");
  }
}
