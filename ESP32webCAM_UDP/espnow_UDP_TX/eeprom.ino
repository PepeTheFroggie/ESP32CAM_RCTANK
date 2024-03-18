#include <EEPROM.h>

#define EEPROM_SIZE 16

void storeUDP(IPAddress udpAddress)
{
  EEPROM.write(0, 0xAA);
  EEPROM.write(1, udpAddress[0]);
  EEPROM.write(2, udpAddress[1]);
  EEPROM.write(3, udpAddress[2]);
  EEPROM.write(4, udpAddress[3]);
  EEPROM.commit();  
}

IPAddress readUDP(IPAddress udpAddress)
{
  if (EEPROM.read(0) == 0xAA)
  {
    udpAddress[0] = EEPROM.read(1);
    udpAddress[1] = EEPROM.read(2);
    udpAddress[2] = EEPROM.read(3);
    udpAddress[3] = EEPROM.read(4);
    Serial.println("eeprom address read");
  }
  return udpAddress;
}

void storeUDP(int udpPort)
{
  EEPROM.write(5, 0xAA);
  EEPROM.write(6, udpPort>>8);
  EEPROM.write(7, udpPort&0xFF);
  EEPROM.commit();  
}

int readUDP(int udpPort)
{
  if (EEPROM.read(5) == 0xAA)
  {
    udpPort  = EEPROM.read(6)<<8;
    udpPort += EEPROM.read(7);
    Serial.println("eeprom port read");
  }
  return udpPort;
}

void storeTXInt(int txi)
{
  EEPROM.write( 8, 0xAA);
  EEPROM.write( 9, txi>>8);
  EEPROM.write(10, txi&0xFF);
  EEPROM.commit();  
}

int readTXInt(int txi)
{
  if (EEPROM.read(8) == 0xAA)
  {
    txi  = EEPROM.read( 9)<<8;
    txi += EEPROM.read(10);
    Serial.println("TX interval read");
  }
  return txi;
}

void initEEPROM()
{
  EEPROM.begin(EEPROM_SIZE);
}
