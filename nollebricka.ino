// Based on code from https://github.com/GeorgiZhelezov/arduino-msgeq7

#include <FastLED.h>
#define NUM_LEDS 69
#define LED_PIN 2
CRGB leds[NUM_LEDS];


unsigned short MSGEQ[7]; //arrays for storing MSGEQ7 data
unsigned short strobePin = 4;
unsigned short resetPin = 9;
unsigned short MSGEQPin = 1;
unsigned short noiseFilter = 300;
unsigned short readDataDelay = 3; //delay after reading MSGEQ7 data

int ledSections[7][4] = {
  {0,9, 0, 255},
  {10, 19, 64, 255},
  {20, 29, 96, 255},
  {30, 39, 128, 255},
  {40, 49, 160, 255},
  {50, 59, 192, 255},
  {60, 69, 224, 255}
};

void setup() {
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  FastLED.addLeds<WS2812, LED_PIN>(leds, NUM_LEDS);

  pinMode(strobePin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  pinMode(MSGEQPin, INPUT);

  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);

}

void loop() {

  readData();
  

}

void readData() 
{
  for (short i = 0; i < 7; i++)
  {
    digitalWrite(strobePin, LOW); 

    MSGEQ[i] = analogRead(MSGEQPin); 

    digitalWrite(strobePin, HIGH); 

    MSGEQ[i] = constrain(MSGEQ[i], noiseFilter, 1023); 
   
    MSGEQ[i] = map(MSGEQ[i], noiseFilter, 1023, 0, 255); 

    delay(readDataDelay);

    led(i, MSGEQ[i]);
     
  }

}

void led(short sectionIndex, unsigned short brightness) {
    for(int dot = ledSections[sectionIndex][0]; dot <= ledSections[sectionIndex][1]; dot++) { 
            leds[dot] = CHSV(ledSections[sectionIndex][2], ledSections[sectionIndex][3], brightness);
    }
    FastLED.show();
}
