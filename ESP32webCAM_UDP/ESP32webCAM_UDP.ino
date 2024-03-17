#include "esp_camera.h"
#include <WiFi.h>
#include "Arduino.h"
#include "soc/rtc_cntl_reg.h"  //disable brownout problems

//Have your PSRAM enabled (Tools/Board/PSRAM)!!
//Replace with your network credentials
const char* ssid = "FSM";
const char* password = "0101010101";

WiFiServer server(80);

#define CAMERA_MODEL_AI_THINKER
#if defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22
  
#else
  #error "Camera model not selected"
#endif

char debugvalue;
bool gotrc;

void setup() 
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
 
  Serial.begin(115200);
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 16000000;
//config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  config.grab_mode    = CAMERA_GRAB_LATEST; 
    
  if(psramFound())
  {
    config.frame_size = FRAMESIZE_HVGA; //FRAMESIZE_QVGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QQVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  ledcSetup(7, 5000, 8);
  ledcAttachPin(4, 7);  //pin4 is LED
  led_bright(50);
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) Serial.printf("Camera init failed with error 0x%x", err);

  //flip picture
  sensor_t * s = esp_camera_sensor_get();
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
  
  // Wi-Fi connection
  WiFi.mode(WIFI_STA);
  WiFi.setSortMethod(WIFI_CONNECT_AP_BY_SIGNAL);
  WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();  
  Serial.println(WiFi.localIP());
    
  server.begin();
  init_RC();
  initMotors();
  led_bright(0);
}

void loop() 
{
  delay(50);
  if (Serial.available()) 
  {
    char ch = Serial.read();
    if (ch != 10) debugvalue = ch;
  }
  rcvalCyclic();
  if (gotrc) domotudp();

  WiFiClient client = server.available();  // listen for incoming clients
  if (client) 
  { // if you get a client,
    initjpegStream(&client);
    while (client.connected()) // loop while the client's connected
    { 
      delay(70);  
      if (client.available()) jpegStream(&client);
      rcvalCyclic();
      if (gotrc) domotudp();
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
