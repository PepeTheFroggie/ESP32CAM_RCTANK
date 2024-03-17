#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPUpdateServer.h>
#include <EEPROM.h>

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

const char *ssid = "FSM";
const char *password = "0101010101";

#define LED 2

uint8_t seqno;
volatile boolean gotRC;
boolean RCinOK;
bool fakeRC;

WiFiUDP Udp;
unsigned int localUdpPort = 5005;

String udpAddress = "192.168.178.45";
int remoteUdpPort = 4210;

void setup() 
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  
  Serial.begin(115200); Serial.println();

  WiFi.mode(WIFI_STA); // Station mode for esp-now 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected to WiFi");
  Serial.println(WiFi.localIP());
  
  httpUpdater.setup(&server,"/up"); 
  server.on("/", handleCmd);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  init_RC();
  Udp.begin(localUdpPort);
  digitalWrite(LED, HIGH);
  Serial.println("R for RC values");
  Serial.println("S for timing");
}

uint32_t lastRC; 
uint32_t nextSend; 
uint32_t sendInterval = 50; 

uint8_t buffer[14];
extern int16_t rcValue[];

void storeRC(int16_t in, uint8_t * out)
{
  out[0] = in>>8;
  out[1] = in&0xFF;
}

char debugvalue;

void loop() 
{
  uint32_t now; 
  delay(2);
  now = millis(); // actual time

  // parser part
  if (Serial.available()) 
  {
    char ch = Serial.read();
    if (ch != 10) debugvalue = ch;
  }

  if (fakeRC)
  {
    if (now >= lastRC + 50)
    {
      gotRC = true;
      rcValue[0] = rcValue[1] = 1000;
      rcValue[2] = rcValue[3] = 1000;
      rcValue[4] = rcValue[5] = 1000;
    }
  }
  
  if (gotRC)
  {
    gotRC = false;
    lastRC = now;
    RCinOK = true;
    
    // orange 0-thro 1-roll 2-pitch 3-rud 1100 1930
    // flysky roll=0 pitch=1 thr=2 yaw=3 
    if (debugvalue == 'R') 
    {
      Serial.print(rcValue[0]); Serial.print("  ");
      Serial.print(rcValue[1]); Serial.print("  ");
      Serial.print(rcValue[2]); Serial.print("  ");
      Serial.print(rcValue[3]); Serial.print("  ");
      Serial.print(rcValue[4]); Serial.print("  ");
      Serial.print(rcValue[5]); Serial.println();
    }
    
    buffer[ 0] = 0x55;
    buffer[ 1] = 00;//seqno++;
    storeRC(rcValue[1],&buffer[ 2]);
    storeRC(rcValue[2],&buffer[ 4]);
    storeRC(rcValue[0],&buffer[ 6]);
    storeRC(rcValue[3],&buffer[ 8]);
    storeRC(rcValue[4],&buffer[10]);
    storeRC(rcValue[5],&buffer[12]);

    if (now >= nextSend)
    {
      nextSend = now + sendInterval;
      Udp.beginPacket(udpAddress.c_str(), remoteUdpPort);
      Udp.write(buffer, 14);
      Udp.endPacket();
      if (debugvalue == 'S') Serial.println("S");
    }
    digitalWrite(LED, HIGH);
    if (debugvalue == 'S') Serial.println("R");
  }
  else if (now >= lastRC + 250)
  {
    lastRC = now;
    RCinOK = false;
    if (debugvalue == 'R') Serial.println("No PPM input"); 
    digitalWrite(LED, !digitalRead(LED)); 
  }
  server.handleClient();
}
