// used to image stream
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE =
    "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART =
    "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

camera_fb_t * fb = NULL;
static int64_t last_frame = 0;

static void initjpegStream(WiFiClient* client) 
{
  Serial.println("Image stream start");
  client->println("HTTP/1.1 200 OK");
  client->printf("Content-Type: %s\r\n", _STREAM_CONTENT_TYPE);
  client->println("Content-Disposition: inline; filename=capture.jpg");
  client->println("Access-Control-Allow-Origin: *");
  client->println();
  if (!last_frame) last_frame = esp_timer_get_time();
}

static void jpegStream(WiFiClient* client) 
{
  {
    fb = esp_camera_fb_get();
    if (fb) 
    {
      client->print(_STREAM_BOUNDARY);
      client->printf(_STREAM_PART, fb);
      int32_t to_sends    = fb->len;
      int32_t now_sends   = 0;
      uint8_t* out_buf    = fb->buf;
      uint32_t packet_len = 8 * 1024;
      while (to_sends > 0) 
      {
          now_sends = to_sends > packet_len ? packet_len : to_sends;
          if (client->write(out_buf, now_sends) == 0) goto client_exit;
          out_buf += now_sends;
          to_sends -= packet_len;
      }

      int64_t fr_end     = esp_timer_get_time();
      int64_t frame_time = fr_end - last_frame;
      last_frame         = fr_end;
      frame_time /= 1000;
      Serial.printf("MJPG: %luKB %lums (%.1ffps)\r\n",
                    (long unsigned int)(fb->len / 1024),
                    (long unsigned int)frame_time,
                    1000.0 / (long unsigned int)frame_time);

      esp_camera_fb_return(fb);
    }
  }
  return;

client_exit:
  esp_camera_fb_return(fb);
  client->stop();
  Serial.printf("Image stream end\r\n");
}
