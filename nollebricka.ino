// Based on code from https://github.com/GeorgiZhelezov/arduino-msgeq7

#include <FastLED.h>

#define NUM_LEDS 70
#define LED_PIN 2
CRGB leds[NUM_LEDS];


unsigned short MSGEQ[7]; //arrays for storing MSGEQ7 data
unsigned short strobePin = 4;
unsigned short resetPin = 9;
unsigned short MSGEQPin = 1;
unsigned short noiseFilter = 300;
unsigned short readDataDelay = 3; //delay after reading MSGEQ7 data
uint8_t hue = 0;

int ledSections[7][2][2] = {
        {
                {0,  4},
                {65, 69}
        },
        {
                {5,  9},
                {60, 64}
        },
        {
                {10, 14},
                {55, 59}
        },
        {
                {15, 19},
                {50, 54}
        },
        {
                {20, 24},
                {45, 49}
        },
        {
                {25, 29},
                {40, 44}
        },
        {
                {30, 34},
                {35, 39}
        }
};

void setup() {
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
    rainbowEq();

}

void readData() {
    for (short i = 0; i < 7; i++) {
        digitalWrite(strobePin, LOW);

        MSGEQ[i] = analogRead(MSGEQPin);

        digitalWrite(strobePin, HIGH);

        MSGEQ[i] = constrain(MSGEQ[i], noiseFilter, 1023);

        MSGEQ[i] = map(MSGEQ[i], noiseFilter, 1023, 0, 255);

        delay(readDataDelay);


    }

}

void rainbowEq() {
    for (int sectionIndex = 0; sectionIndex < 7; sectionIndex++) {
        unsigned short brightness = MSGEQ[sectionIndex];
        brightness = constrain(brightness, 10, 255);
        
        for (int side = 0; side < 2; side++) {
            for (int dot = ledSections[sectionIndex][side][0]; dot <= ledSections[sectionIndex][side][1]; dot++) {
              int thisSection = 32 * sectionIndex;
              int nextSection = 32 * (sectionIndex + 1);

              // To make each section fade to next section
              int extra = map(dot, ledSections[sectionIndex][side][0], ledSections[sectionIndex][side][1], 0, nextSection - thisSection - 1);

              // Invert on other side
              if (side == 1) {
                extra = (extra * -1) + 32;
              }
              
              leds[dot] = CHSV(thisSection + extra, 255, brightness);
            }
        }
    }
    
    FastLED.show();
}

void whiteEq() {
    for (int sectionIndex = 0; sectionIndex < 7; sectionIndex++) {
        unsigned short brightness = MSGEQ[sectionIndex];
        brightness = constrain(brightness, 10, 100);
        
        for (int side = 0; side < 2; side++) {
            for (int dot = ledSections[sectionIndex][side][0]; dot <= ledSections[sectionIndex][side][1]; dot++) {
              leds[dot] = CHSV(0, 0, brightness);
            }
        }
    }
    
    FastLED.show();
}

void rainbowFade() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue++ + i, 255, 50);
    FastLED.show();
    delay(30);
  }
  
  
}
