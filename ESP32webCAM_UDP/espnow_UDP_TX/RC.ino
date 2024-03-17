
#define CHANNELS 6
#define IN_PIN 4 // GPIO, D2 on wemos D1 mini

//------------------------------------------------------------------

#define PPMIN_CHANNELS 6  // dont raise this

volatile uint32_t last = 0;
volatile uint8_t  chan = 0;

int16_t rcValue[CHANNELS];  // in us, center = 1500

ICACHE_RAM_ATTR void rxInt() 
{
    uint32_t now,diff; 
    now = micros();
    diff = now - last;
    last = now;

    if      (diff > 3000) chan = 0; // Sync gap
    else if (chan < CHANNELS)
    {
      if (950<diff && diff<2050)
      {
        rcValue[chan] = diff;
        chan++;
      }
    }
    if (chan == PPMIN_CHANNELS) gotRC = true;
}

void init_RC()
{
  attachInterrupt(IN_PIN,rxInt,RISING);
}

//------------------------------------------------------------------
