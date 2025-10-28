// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif
#include "driver/rtc_io.h"
#include "esp_sleep.h"



//Neopixel constants
#define PIN 20        //Neopixel pin
#define NUMPIXELS 11  // number of LEDs
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int LEDColor[] = {
  pixels.Color(21, 100, 30),
  pixels.Color(100, 0, 0),
  pixels.Color(0, 100, 0),
  pixels.Color(0, 0, 100),
  pixels.Color(100, 100, 0),
  pixels.Color(0, 100, 100),
  pixels.Color(20, 100, 100),
  pixels.Color(100, 20, 100),
  pixels.Color(100, 100, 20),
  pixels.Color(100, 50, 50),
  pixels.Color(84, 127, 215),
  pixels.Color(50, 50, 170)
};

//Deepsleep constants
#define WAKEUP_GPIO GPIO_NUM_17  // Using GPIO0


//Button variables
const int buttonPin = 0;  // the number of the pushbutton pin
long int sleepCounter = 0;
long int buttonPressDuration = 0;  //to reset and stuff
// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status
int LEDindex = 0;


RTC_DATA_ATTR int bootCount = 0;


void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); break;
    default: Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

void gotosleep() {
  pixels.clear();
  pixels.show();
  Serial.println("Sleeping");
  uint64_t mask = (1ULL << WAKEUP_GPIO);
  esp_sleep_enable_ext1_wakeup(mask, ESP_EXT1_WAKEUP_ANY_HIGH);  // Wake up when GPIO0 is LOW
  rtc_gpio_pulldown_dis(WAKEUP_GPIO);                           // Disable pull-down
  rtc_gpio_pullup_en(WAKEUP_GPIO);                              // Enable pull-up for LOW trigger
  esp_deep_sleep_start();
}


void setup() {
  Serial.begin(115200);

  // button setup

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);



  //neopixel setup
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  for (int i = 0; i < NUMPIXELS; i = i + 1) {
    pixels.setPixelColor(i, LEDColor[3]);
    pixels.show();  // Send the updated pixel colors to the hardware.
  }
  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < NUMPIXELS; i = i + 1) {
      pixels.setPixelColor(i, LEDColor[3]);
      pixels.setPixelColor(i + 1, LEDColor[2]);
      delay(80);
      pixels.show();  // Send the updated pixel colors to the hardware.
    }
  }

  delay(500);
  pixels.clear();
  pixels.show();
}

void loop() {

  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    sleepCounter = 0;  //reset the sleepCounter

    while (buttonState == LOW) {  // prevent debounce
      buttonState = digitalRead(buttonPin);
      delay(10);
      buttonPressDuration++;
      if (buttonPressDuration > 500) {
        for (int j = 0; j < 2; j++) {
          for (int i = 0; i < NUMPIXELS; i = i + 1) {
            pixels.setPixelColor(i, LEDColor[5]);
            pixels.setPixelColor(i + 1, LEDColor[6]);
            delay(80);
            pixels.show();  // Send the updated pixel colors to the hardware.
          }
        }
        gotosleep(); 
      }
    }
    buttonPressDuration = 0;


    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    pixels.setPixelColor(LEDindex, LEDColor[LEDindex]);
    pixels.show();  // Send the updated pixel colors to the hardware.
    if (LEDindex == NUMPIXELS) {
      LEDindex = 0;
      pixels.clear();
      pixels.show();
    } else {
      LEDindex = LEDindex + 1;
    }
  }

  if (buttonState == HIGH) {
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
  }

  delay(100);
  sleepCounter = sleepCounter + 1;

  if (sleepCounter > 18000) {
    gotosleep();
  }
}
