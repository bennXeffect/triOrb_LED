#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define DATA_PIN 5
//#define CLOCK_PIN 13

#define NUM_GROUPS 9
#define LED_P_GROUP 1
#define NUM_LEDS NUM_GROUPS * LED_P_GROUP


//int LEDpattern = 0;

int LEDcol[5]; //the RGB values and the state of blink

unsigned int groupCol[NUM_GROUPS][3]; //group to edit, then the 3 colors and the pattern

unsigned char bri=0;//changing brightnness on the fly for tenmetsu
unsigned char multipl[NUM_GROUPS]; //multiplicator of light intensity for tenmetsu

unsigned int stateCode = 0; //recieved list of LED states

bool blinkState[NUM_GROUPS]; //say if the group is blinking or not

// Define the array of leds
CRGB leds[NUM_LEDS];

unsigned long stamp=0;

//void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(195); } }


void setup() {
//Serial.begin(115200);
Wire.begin(); 
FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
}

void loop() {
//static uint8_t hue = 0

bri=sin8(millis()/3);

 Wire.requestFrom(5,10);

LEDcol[0]=Wire.read();
LEDcol[1]=Wire.read();
LEDcol[2]=Wire.read();
LEDcol[3]=Wire.read();
LEDcol[4]=Wire.read();

stateCode = (LEDcol[3] << 8) + LEDcol[4];



for(int a = 0 ; a < NUM_GROUPS ; a++){ //put the colors

if(bitRead(stateCode, 0)==1 && bitRead(stateCode, a+1)==1){ //the groups that need blinking state
blinkState[a] = 1;
}

if(bitRead(stateCode, 0)==0 && bitRead(stateCode, a+1)==1){ //the groups that need NO blinking state
blinkState[a] = 0;
}

if(bitRead(stateCode, a+1) == 1){ //determine if the group needs to change values
//if we are here, we need to set the incoming RGB color into the the corresponding LED
for(int c = 0 ; c < 3 ; c++){ //changinng the color values
groupCol[a][c]=LEDcol[c];
}
}
}

for(int a = 0 ; a < NUM_GROUPS ; a++){ //go trough all the groups
/*
for(int i = 0; i < NUM_LEDS/3 ; i++) {
		// Set the i'th led to red 
		//leds[i] = CHSV(40*LEDpattern, 255-LEDpattern*LEDpattern*4, 255);
		leds[i] = CRGB(groupCol[a][0],groupCol[a][1],groupCol[a][2]);
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		//fadeall();
		// Wait a little bit before we loop around and do it again
	//	delay(5);
	}
	for(int i = NUM_LEDS/3; i < NUM_LEDS/3*2 ; i++) {
		// Set the i'th led to red 
		//leds[i] = CHSV(40*LEDpattern, 255-LEDpattern*LEDpattern*4, 255);
		leds[i] = CRGB(groupCol[a][0],groupCol[a][1],groupCol[a][2]);
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		//fadeall();
		// Wait a little bit before we loop around and do it again
	//	delay(5);
	}
*/

if(blinkState[a]==1) multipl[a]=bri;
		else multipl[a]=0;

	for(int i = LED_P_GROUP*a ; i < LED_P_GROUP*(a+1) ; i++) {
		// Set the i'th led to red 
		//leds[i] = CHSV(40*LEDpattern, 255-LEDpattern*LEDpattern*4, 255);
		

		leds[i] = CRGB(groupCol[a][0]-map(multipl[a],0,255,0,groupCol[a][0]),groupCol[a][1]-map(multipl[a],0,255,0,groupCol[a][1]),groupCol[a][2]-map(multipl[a],0,255,0,groupCol[a][2]));
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		//fadeall();
		// Wait a little bit before we loop around and do it again
	//	delay(5);
	}

}
		// Show the leds
		FastLED.show();


if(millis()>stamp + 1500){
    stamp=millis();

Serial.print("LED color is : ");
Serial.print(LEDcol[0]);
Serial.print(LEDcol[1]);
Serial.print(LEDcol[2]);
Serial.print("   LED pattern is : ");
Serial.println(stateCode,BIN);
Serial.print("LED pattern reconnstruct, 1st part = ");
Serial.print(LEDcol[3], BIN);
Serial.print("    2nd part = ");
Serial.println(LEDcol[4], BIN);
}

}