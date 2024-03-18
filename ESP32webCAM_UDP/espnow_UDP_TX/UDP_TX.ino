#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPUpdateServer.h>

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

String ssid = "FSM";
String pass = "0101010101";

#define LED 2

uint8_t seqno;
volatile boolean gotRC;
boolean RCinOK;
bool fakeRC;

WiFiUDP Udp;
unsigned int localUdpPort = 5005;

IPAddress localAddress;
IPAddress udpAddress = {192,168,178,45};
int remoteUdpPort = 4210;
uint32_t sendInterval = 50; 

void setup() 
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.begin(115200); Serial.println();

  initEEPROM();
  udpAddress = readUDP(udpAddress);
  remoteUdpPort = readUDP(remoteUdpPort);
  sendInterval = readTXInt(sendInterval);
  
  /*
  Serial.read();
  Serial.println();
  Serial.println("Enter SSID");
  while (Serial.available() == 0) {}  
  ssid = Serial.readStringUntil('\n');        // store SSID
  Serial.println("Enter Password");
  while (Serial.available() == 0) {}  
  pass = Serial.readStringUntil('\n');       // store password
  Serial.printf("\n'%s' '%s'\n", ssid.c_str(), pass.c_str());
  /**/
  
  WiFi.mode(WIFI_STA); // Station mode for esp-now 
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected to WiFi");
  localAddress = WiFi.localIP();
  udpAddress[0] = localAddress[0];
  udpAddress[1] = localAddress[1];
  udpAddress[2] = localAddress[2];
  Serial.println(localAddress);
  
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
    buffer[ 1] = seqno++;
    storeRC(rcValue[1],&buffer[ 2]);
    storeRC(rcValue[2],&buffer[ 4]);
    storeRC(rcValue[0],&buffer[ 6]);
    storeRC(rcValue[3],&buffer[ 8]);
    storeRC(rcValue[4],&buffer[10]);
    storeRC(rcValue[5],&buffer[12]);

    if (now >= nextSend)
    {
      nextSend = now + sendInterval;
      Udp.beginPacket(udpAddress, remoteUdpPort);
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
