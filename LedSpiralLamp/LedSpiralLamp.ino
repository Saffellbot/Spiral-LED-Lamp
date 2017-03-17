/*This code was written by Brandon Saffell on 17 Mar 2017 to control a NeoPixel strip. The intended design is for a 
 * NeoPixel strip mounted in a spiral holder to act as a night light. This code was designed to run on an Adafruit 5V trinket @ 8 Mhz.
 * The effect looks nice when used as a straight strip as well.
 * inspiration from Tony D @ Adafruit - https://youtu.be/OJlYxnBLBbk?list=PLjF7R1fz_OOXrI15wuXeESA0aA4VzcWSi and Lode's Computer Graphics Tutorial
 * - http://lodev.org/cgtutor/fire.html.
 * This code could easily be worked to define a start and cooling color via an extrenal interface.

*/
#include <FastLED.h>  //Used to Control the Neopixel Strip.
#define DATA_PIN 1  
const byte NUM_LEDS=30; //Number of LEDs in the stip.
const int COOLFACTOR=129;  //How much the "Fire" "Cools" if this is too low the strip will just flash obnoxiously, if it is too high the "flames" will be very small.
byte base=0;  //Base RGB Offset for Pixel 0.
byte baseRed=0; //Red offset for Pixel 0.
byte baseGreen=0;
byte baseBlue=0;
int loopCounter=0;  //Used to determine how often to change colors.
const byte MAXCOOLING=16;  //Maximum value for the 3 variables below. These variables stack with COOLFACTOR to allow the flame to change colors down it's length.
byte redCool=0;
byte greenCool=0;
byte blueCool=0;
const int LDRPIN=1; //Color the photocell is attached to for night light action.
int lightLevel=0;  //Measured light level.

const byte LDRTIME=20; //How often to check the ambient light level. On fast microprocessors this could be done every cycle, but I found it was slowing things down.
const int COOLTIME=500; //How often to randomize the flame cooling.
const int BASETIME=15; //How often to randomize the base pixel.

CRGB led[NUM_LEDS];  //Create NeoPixel object.

void setup() 
{ 
  delay(2000);  //Fast LED library reccomends this.
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(led, NUM_LEDS);  //Setup LEDS.
  for (int i=0; i<NUM_LEDS; i++)  //Set all LEDs to black.
  {
    led[i].r=(0);
    led[i].g=(0);
    led[i].b=(0);
  }
  FastLED.show();  //Push colors.
}

void loop()
{
  if (loopCounter%LDRTIME==0)  //Check the background light levels.
  {
    lightLevel=analogRead(LDRPIN);
    lightLevel=lightLevel/2;  //I found this range works well with some trial and error.
    if (lightLevel > 255)  //Set range for mathing with an uint8_t.
    {
      lightLevel=255;
    }
    FastLED.setBrightness(255-lightLevel);  //Set the strip to be off if it's bright out, and bright if it's dark out. Ala a nightlight.
  }
  
  if (loopCounter%COOLTIME==0) //Radomizes how much each individual color is cooled.
  {
      blueCool+=random8(0,2);  //Adds 0 or 1 to the base value.
      blueCool%=MAXCOOLING;  //Range limits the value.
      greenCool+=random8(0,2);
      greenCool%=MAXCOOLING;
      redCool+=random8(0,2);
      redCool%=MAXCOOLING;
      baseRed+=random8(0,2);  //Gradually offsets the individual colors of the base pixel.
      baseGreen+=random8(0,2);
      baseBlue+=random8(0,2);
  }
  
  if (loopCounter%BASETIME==0)  //Change the base pixel to make the flame flicker.
  {
  base=random8();

  led[0].r=baseRed+base;
  led[0].g=baseGreen+base;
  led[0].b=baseBlue+base;
  }
  
  for(int i=1; i<NUM_LEDS; i++)  //For each pixel, starting from the "Dark End" add the pixel itself and the pixel below it to the strip. Then the combined value is divided by a number greater than 2. This is repeated for each color. See the links in the intro for more information.
  {
    led[NUM_LEDS-i].r=(led[NUM_LEDS-i].r+led[NUM_LEDS-(i+1)].r)*64/(COOLFACTOR+redCool);
    led[NUM_LEDS-i].g=(led[NUM_LEDS-i].g+led[NUM_LEDS-(i+1)].g)*64/(COOLFACTOR+greenCool);
    led[NUM_LEDS-i].b=(led[NUM_LEDS-i].b+led[NUM_LEDS-(i+1)].b)*64/(COOLFACTOR+blueCool);
  }
  loopCounter++;  //Increment the loop counter.

  delay(5);  //Controls the animation speed.
  
  FastLED.show();  //Update the strip colors.
}


