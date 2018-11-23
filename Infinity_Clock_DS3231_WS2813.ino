// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <RTClib.h>
//Functions to light the WS2813 LEDs

#include <FastLED.h>
//OFFSET is which the first LED to allow for overlap
#define OFFSET 3
//Clock must have 60 usable plus some overlap or not used.
#define NUM_LEDS 60+OFFSET
#define DATA_PIN 7

//Which LED is 1 second led. in my version top is 30
#define CLOCKSTART  30
CRGB leds[NUM_LEDS];// Define the array of leds 
/*
 * Clock Settings
 * LED colors for background.  use 0 for black.
 * I wanted to make this east and use HTML colors.
 * https://htmlcolorcodes.com/
 * Use these like HTML colors swap # for 0x
 * #98AFC7 (blue grey) is 0x#98AFC7  
 */
//Change background color for AM/PM
const unsigned long backgroundAM =0x070707;
const unsigned long backgroundPM =0x646D7E;

//LED Hands of the clock
const unsigned long hourHand =0x000FFF;
const unsigned long minHand =0x00CC00;
const unsigned long secHand = 0xcc0000;

//Want 3 led for hour?
bool theeledhour=true; //false is one.
//if you have 3 led for hour do you want the two on the
//ouside to be different colors?
const unsigned long extraHourHand =0x00001F;


/*Real Time Clock
 * DS3231
 * Connect GND to Ground
 * Connect VCC to 5V
 *  A4 to SDA
 *  A5 to SCL 
 */
RTC_DS3231 rtc;
int hour, minute, second;
bool pm=false; // check to see if PM or AM.

void setup () {
  Serial.begin(9600);
  delay(3000); // wait for console opening

  //initialize the LEDS
  LEDS.addLeds<WS2813,DATA_PIN,GRB>(leds,NUM_LEDS);
  LEDS.setBrightness(128);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
   }

//This clever little command will reset your DS3132 with your PC on compile if the battery has failed. 
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

}

void loop () {
    getTime();
    setStrip();
}


//lets me use HTML colors in the format #FF00FF is  0xFF00FF  
CRGB setColor(unsigned long htmlColor){  return CRGB(htmlColor >>16,htmlColor >>8 & 0xFF,htmlColor & 0xFF); }

void getTime(){
  
  DateTime now = rtc.now();
  hour=now.hour();
  
  pm=false;
  if( hour>12) {
    hour -=12;
    pm=true;
  }
  minute=now.minute();
  second=now.second();
  
 /* uncomment to send serial date/time
  Serial.print(hour, DEC);
  Serial.print(':');
  Serial.print(minute, DEC);
  Serial.print(':');
  Serial.print(second, DEC);
  if(pm){
    Serial.println("PM");
  } else {
    Serial.println("AM");
  }
  */
}

/*
 * Set the strip colors
 * one led/second 60 led total
 * 5 per hour
 * use offset to hide overlap
 */
void setStrip(){
  // Fancy way of doing an if else. 
  int stripSec=((CLOCKSTART+second)< 60 +OFFSET)?(CLOCKSTART+second):((second-CLOCKSTART));
  int stripMin=((CLOCKSTART+minute)< 60 +OFFSET)?(CLOCKSTART+minute):((minute-CLOCKSTART));
  int stripHour=((CLOCKSTART+(hour*5))< 60 +OFFSET)?(CLOCKSTART+(hour*5)):(((hour*5)-CLOCKSTART));

  //Set any leading LED to black
  for (int x=0; x< OFFSET; x++){ 
    leds[x]=setColor(0);
  }
  //Set Background.
  for(int i = OFFSET; i < 60+OFFSET; i++) {
    if (pm){
        leds[i]=setColor(backgroundPM); 
    } else {
        leds[i]=setColor(backgroundAM);
    }
  }

//if we want 3 leds for hour hand.  
if (theeledhour) {
  if (hour *5 +CLOCKSTART -1 < 60+OFFSET){
     leds[hour*5+CLOCKSTART -1 ]=setColor(extraHourHand);
  } else {
     leds[hour*5 -CLOCKSTART-1]=setColor(extraHourHand);
  }
   if (hour *5 +CLOCKSTART +1 < 60+OFFSET){
     leds[hour*5+CLOCKSTART+1]=setColor(extraHourHand);
   } else {
     leds[hour*5 -CLOCKSTART+1]=setColor(extraHourHand);
   }
}

leds[stripHour]=setColor(hourHand);
leds[stripMin]=setColor(minHand);
leds[stripSec]=setColor(secHand);

FastLED.show();
 
delay(200);

}


