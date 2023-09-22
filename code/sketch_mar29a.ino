#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BLEMIDI.h"
#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>

#define BUFSIZE                        160   // Size of the read buffer for incoming data
#define VERBOSE_MODE                   false // If set to 'true' enables debug output
#define BLUEFRUIT_SPI_CS               8
#define BLUEFRUIT_SPI_IRQ              7
#define BLUEFRUIT_SPI_RST              4    // Optional but recommended, set to -1 if unused
#define BLUEFRUIT_SPI_SCK              13
#define BLUEFRUIT_SPI_MISO             12
#define BLUEFRUIT_SPI_MOSI             11

#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80

#define CLEARTHRESHHOLD 8000
#define LOWTONE 1000
#define HIGHTONE 2000
#define LOWKEY 64   // high C
#define HIGHKEY 76  // double high C

#define NUMPIXELS      1
#define PIXEL_PIN      13

#define min2(a,b) ((a) < (b) ? (a) : (b))
#define min3(a, b, c) (min2((c), min2((a), (b))))
#define max2(a,b) ((a) > (b) ? (a) : (b))
#define max3(a, b, c) (max2((c), max2((a), (b))))


const int ledPin = 13;
bool isConnected = false;
int note = 60;
int prevnote = 60;
int state = LOW;

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
Adafruit_BLEMIDI midi(ble);
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Piano Glove MIDI!");
// start neo-pixel
  pixels.begin();
  pixels.setPixelColor(0, pixels.Color(0,0,0));
  pixels.show();
// start sensor
  if (tcs.begin()){Serial.println("Sensor detected");} else { Serial.println("Sensor NOT detected"); return; } 
  tcs.setInterrupt(true);
// start bluetooth
  if (ble.begin(VERBOSE_MODE) ){Serial.println("BLE activated");} else { Serial.println("BLE NOT started"); return;}
  ble.echo(false);
  ble.setConnectCallback(connected);
  ble.setDisconnectCallback(disconnected);  

  Serial.flush();

// start midi services  
  midi.begin(true);
}

void connected(void) {isConnected = true;}
void disconnected(void){isConnected = false;}

void loop() {
  uint16_t clear, red, green, blue;
  ble.update(500); 
//  isConnected = true;
  if ( isConnected) {tcs.setInterrupt(false);} else {tcs.setInterrupt(true); return;}
  delay(60);  // it takes 50 ms to read
  tcs.getRawData(&red, &green, &blue, &clear);
  state = (clear < CLEARTHRESHHOLD)? LOW : HIGH;  //test if finger is down
// the color hue is converted to a musical note note code = hue/21 +  57
  note = hue(red, green, blue)/21 + 57;
  Serial.print("State: "); Serial.print(state);
  Serial.print(" R: "); Serial.print(red);  
  Serial.print(" G: "); Serial.print(green);
  Serial.print(" B: "); Serial.print(blue);
  Serial.print(" clear: "); Serial.print(clear);
  Serial.print(" -> hue: "); Serial.println(note);
  
  if (state == HIGH) {
     if (note == prevnote) return;
     midi.send(0x80, prevnote, 0x64);
     midi.send(0x90, note, 0x64);
     prevnote = note;
     pixels.setPixelColor(0, pixels.Color((int) red/125., (int) blue/125., (int) green/125.));
     pixels.show();
  } else {
     midi.send(0x80, prevnote, 0x64);
     digitalWrite(ledPin, HIGH);
     pixels.setPixelColor(0, pixels.Color(0,0,0));
     pixels.show();
  }
}


// code that combines the red, green and blue components are returns a single number (hue)
int hue(uint16_t r, uint16_t g, uint16_t b){
  uint16_t rgbMin, rgbMax;
  uint8_t h;
  
  rgbMin = min3(r, g, b);
  rgbMax = max3(r, g, b);
  if (rgbMax == r) 
    h =   0 + 43 * (g - b) / (rgbMax - rgbMin);
  else if (rgbMax == g)
    h =  85 + 43 * (b - r) / (rgbMax - rgbMin);
  else
    h = 171 + 43 * (r - g) / (rgbMax - rgbMin);

  return h;
  }




