Version ESP32webCAM_UDP uses UDP to control motors.This need a special UDP transmitter. See esp8266 based espnow_UDP_TX code. Understand the RC does not go from TX to RX, but TX goes to your router and is then redispatched to RX. This is by far the smoothest control. Uses first aux channel for fwd/rev and second for light on/off.

The files UDPTest.zip contain python code to receive or transmit UDP from your PC.
