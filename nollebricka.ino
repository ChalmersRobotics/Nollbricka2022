// Based on code from https://github.com/GeorgiZhelezov/arduino-msgeq7

#include <FastLED.h>
#include <Button.h>

#define NUM_LEDS 67
#define LED_PIN 2
CRGB leds[NUM_LEDS];

Button modeButton(6); 

unsigned short MSGEQ[7]; //arrays for storing MSGEQ7 data
unsigned short strobePin = 4;
unsigned short resetPin = 9;
unsigned short noiseFilterPin = A2;
unsigned short MSGEQPin = 1;
unsigned short readDataDelay = 3; //delay after reading MSGEQ7 data
uint8_t hue = 0;

uint8_t mode = 0;
boolean pressed = false;


int ledSections[7][2][2] = {
        {
                {1,  4},
                {64, 66}
        },
        {
                {5,  8},
                {60, 63}
        },
        {
                {9, 12},
                {56, 59}
        },
        {
                {13, 16},
                {52, 55}
        },
        {
                {17, 20},
                {48, 51}
        },
        {
                {21, 24},
                {44, 47}
        },
        {
                {25, 34},
                {35, 43}
        }
};

void setup() {
    //Serial.begin(9600);
    modeButton.begin();
    // put your setup code here, to run once:
    FastLED.addLeds<WS2812, LED_PIN>(leds, NUM_LEDS);
  
    pinMode(strobePin, OUTPUT);
    pinMode(resetPin, OUTPUT);
    pinMode(MSGEQPin, INPUT);
    pinMode(noiseFilterPin, INPUT);

    digitalWrite(resetPin, HIGH);
    digitalWrite(resetPin, LOW);

}

void loop() {

    if (modeButton.pressed() && !pressed) {
        pressed = true;
        mode++;

        if (mode >= 4) {
          mode = 0;
        }
    }

    if (modeButton.released()) {
      pressed = false;
    }

    if (mode == 0) {
      readData();
      rainbowEq();
    } else if (mode == 1) {
      readData();
      rainbowEqMinBrightness();
    } else if (mode == 2) {
      readData();
      whiteEq();
    } else if (mode == 3) {
      rainbowFade();
    }   

}

void readData() {
    unsigned short noiseFilter = analogRead(noiseFilterPin);
    
    delay(readDataDelay);

    noiseFilter = constrain(noiseFilter, 20, 1023);


    for (short i = 0; i < 7; i++) {
        digitalWrite(strobePin, LOW);

        MSGEQ[i] = analogRead(MSGEQPin);

        digitalWrite(strobePin, HIGH);

        MSGEQ[i] = constrain(MSGEQ[i], noiseFilter, 1023);

        MSGEQ[i] = map(MSGEQ[i], noiseFilter-1, 1023, 0, 255);

        MSGEQ[i] = constrain(MSGEQ[i]*2, 0, 255);

        delay(readDataDelay);
    }

}

void rainbowEq() {
    for (int sectionIndex = 0; sectionIndex < 7; sectionIndex++) {
        unsigned short brightness = MSGEQ[sectionIndex];
        
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

void rainbowEqMinBrightness() {
    for (int sectionIndex = 0; sectionIndex < 7; sectionIndex++) {
        unsigned short brightness = MSGEQ[sectionIndex];
        brightness = constrain(brightness, 30, 255);
        
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
        brightness = constrain(brightness, 10, 150);
        
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
    leds[i] = CHSV(hue++ + i, 255, 100);
    FastLED.show();
    delay(30);
    if (modeButton.pressed() && !pressed) {
        pressed = true;
        mode++;

        if (mode >= 4) {
          mode = 0;
        }
    }

    if (modeButton.released()) {
      pressed = false;
      leds[0] = CHSV(255,255,0);

      break;
    }
  }
  
  
}
