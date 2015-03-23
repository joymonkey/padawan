///////////////////////////////////////////////////////////////////////////////////                         
//                  ____   _    ____    ___        ___    _   _                  //
//                 |  _ \ / \  |  _ \  / \ \      / / \  | \ | |                 //
//                 | |_) / _ \ | | | |/ _ \ \ /\ / / _ \ |  \| |                 //
//                 |  __/ ___ \| |_| / ___ \ V  V / ___ \| |\  |                 //
//                 |_| /_/   \_\____/_/   \_\_/\_/_/   \_\_| \_|                 //                                                             
//                                                                               //                           
///////////////////////////////////////////////////////////////////////////////////
/////////  PANGALACTIC SKETCH  FOR PS2 / PS3 / PS4 / XBOX360 CONTROLLERS  /////////
///////////////////////////////////////////////////////////////////////////////////                                                                                                          

//       written with Arduino Mega ADK & PADAWAN Universal Shield in mind        //
//       Tested with Arduino IDE v1.6.1                                          //

// WHAT KIND OF CONTROLLER ARE WE USING?
#define CONTROL 4 // 0=PS2 (wireless PS2 Receiver connected to pins )
				  // 1=XB360 Standard Xbox 360  (USB Xbox 360 Receiver dongle)
                  // 2=PS3 Move Navigator (USB Bluetooth Class 1 dongle)
                  // 3=PS3 Standard DualShock 3 (USB Bluetooth Class 1 dongle)                  
                  // 4=PS4 Standard DualShock 4 (USB Bluetooth Class 1 dongle)
                  // NOTE : For Bluetooth control a Class 1 USB dongle is essential.

// SOUND OPTIONS....
// Primary Audio Player...
#define AUDIO1  2 // 1=SparkfunMP3 Trigger
                  // 2=WTV020 module
// Secondary Audio Player (used for music and background sfx)...
#define AUDIO2  2 // 0=No secondary audio player
                  // 1=SparkfunMP3 Trigger
                  // 2=WTV020 module
#define SNDSTRT   52 // Sound number to play on startup
#define SNDDRVON  52 // Sound number to play when foot drives enabled
#define SNDDRVOFF 53 // Sound number to play when foot drives disabled
#define SNDAUTO   52 // Sound number to play when Dome Auto Mode engaged
byte vol = 4; // for WTV 7 = full volume, 0 off . for MP3 Tirgger 0 = full volume, 255 off 


// AUTO DOME MODE OPTIONS...
#define AUTODLY 5000 // Maximum length of time between Auto Dome movements
#define AUTOSPD 2000 // Maximum speed of Auto Dome movements
#define AUTOTIM 3000 // Maximum length of time dome motor will spin during Auto Dome Mode

// IS THERE AN 12C DISPLAY CONNECTED?
#define DISPLAY 1 // 1 for 128x64 OLED display (labeled Heltec.cn)
                  // 2 for 16x2 character LCD. Default I2C address is #0 (A0-A2 not jumpered), can be changed in LiquidWTI2 below
                  
// VOLTAGE/BATTERY METER
#define VRATION 2.4  //if using 24K and 10K resistors our ratio is 2.4
#define VOLTPIN A3   //the pin used to meature our main battery voltage
float voltage;

// DEFINE WHICH SOUNDS ARE MUSIC OR BACKGROUND FX HERE...
// (IF USING TWO AUDIO MODULES THEN THESE WILL BE PLAYED BY THE SECONDARY MODULE)
#include <avr/pgmspace.h> //to save SRAM, this stuff all gets stored in flash memory
const byte muzak[]PROGMEM = { 5,9,10,11,12 }; //see Sound File References at end of sketch
const byte vLevels[]PROGMEM = { 255,100,80,60,40,20,10,0 }; //volume levels for SparkFun MP3 Trigger, 255 is off, over 100 is almost inaudible
// for WTV 7 = full volume, 0 off . for MP3 Tirgger 0 = full volume, 255 off 

// Include whichever audio libraries are needed...
#if (AUDIO1==2)
  //settings for WTV020 module...
  #include <Wtv.h>
  Wtv priPlayer(A1,A2);
#else 
  //settings for Sparkfun MP3 Trigger...
  #include <MP3Trigger.h>
  MP3Trigger priPlayer;
#endif

#if (AUDIO2==2)
  //settings for WTV020 module...
  #if (AUDIO1!=2) //include wtv020 library if it wasn't already included earlier
  #include <Wtv.h>
  #endif
  Wtv secPlayer(6,9);
#else 
  //settings for Sparkfun MP3 Trigger...
  #include <MP3Trigger.h>
  MP3Trigger secPlayer;
#endif

#if (DISPLAY==1)
 #include "U8glib.h"
 U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
#elif (DISPLAY==2)
 #include <Wire.h>
 #include <LiquidTWI2.h> //this is the speedy i2C LCD library - get it here: http://forums.adafruit.com/viewtopic.php?f=19&t=21586
 LiquidTWI2 lcd(0);      //
#endif

//
//
//

// CONTROLLERS...
#if (CONTROL==0)
	//trusty old PS2 controller
#elif (CONTROL==1) 
	// XB360 Standard Xbox 360
	#include <XBOXRECV.h>
#elif (CONTROL==2 || CONTROL==3) 
	// PS3 Move Navigator or  PS3 Standard DualShock 3
	#include <PS3BT.h>
#elif (CONTROL==4) 
	// PS4 Standard DualShock 4 
	#include <PS4BT.h>
#endif

#if (CONTROL>1)
#include <usbhub.h> //might be needed for some bluetooth dongles
#endif

// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

#if (CONTROL>0)
	USB Usb;
	#if (CONTROL>1)
		//USBHub Hub1(&Usb); // Some dongles have a hub inside
		BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
	#endif
	#if (CONTROL==1)
		XBOXRECV Xbox(&Usb);
	#elif (CONTROL==2 || CONTROL==3) 
		PS3BT PS3(&Btd); 
	#elif (CONTROL==4) 
		PS4BT PS4(&Btd, PAIR); // un-comment this line to pair with your PS4 controller, run once, pair then comment it out again
                //PS4BT PS4(&Btd);
	#endif
#endif
/*
button naming conventions used by different controllers...
PS2/3     PS4       XBX
=========================
SQUARE    SQUARE    X
TRIANGLE  TRIANGLE  Y
CROSS     CROSS     A
CIRCLE    CIRCLE    B
SELECT    SHARE     BACK
START     OPTIONS   START
PS        PS        XBOX
common buttons...
L1,L2,L3,R1,R2,R3,UP,DOWN,LEFT,RIGHT
*/
/*const char but_0[] PROGMEM ="SQUARE";
const char but_1[] PROGMEM ="TRIANGLE";
const char but_2[] PROGMEM ="CROSS";
const char but_3[] PROGMEM ="CIRCLE";
const char but_4[] PROGMEM ="SHARE";
const char but_5[] PROGMEM ="OPTIONS";
const char but_6[] PROGMEM ="PS";
const char but_7[] PROGMEM ="L1";
const char but_8[] PROGMEM ="L2";
const char but_9[] PROGMEM ="L3";
const char but_10[] PROGMEM ="R1";
const char but_11[] PROGMEM ="R2";
const char but_12[] PROGMEM ="R3";
const char but_13[] PROGMEM ="UP";
const char but_14[] PROGMEM ="DOWN";
const char but_15[] PROGMEM ="LEFT";
const char but_16[] PROGMEM ="RIGHT";
const char but_17[] PROGMEM ="TOUCHPAD";
const char* const buttons[18] PROGMEM = { but_0,but_1,but_2,but_3,but_4,but_5,but_6,but_7,but_8,but_9,but_10,but_11,but_12,but_13,but_14,but_15,but_16,but_17 };
char buttbuffer[8];
#define NUMBUTTONS 18
*/
//
//
//

void setup() {
	Serial.begin(9600); //output text to serial for debugging
	setVolume(vol);
	playSound(SNDSTRT);
        while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
        if (Usb.Init() == -1) {
          Serial.print(F("\r\nOSC did not start"));
          while (1); // Halt
        }
        Serial.print(F("\r\nPS4 BT Started\n"));
        #if (DISPLAY==1)
          u8g.setRot180(); //rotate OLED 180 degrees
        #endif
}

//
//
//

///////function to play a sound file
void playSound(int soundNumber) {
	#if (DISPLAY==2)
	  lcd.setCursor(0, 0); 
	  lcd.print("Snd"+String(soundNumber));
	#endif
	#if (AUDIO2>0) //dual audio players are configured, see if this shound is a background fx
		byte sec=0;
		for (byte i = 0; i < sizeof(muzak); i++) {
			if (soundNumber==muzak[i]) {
				secPlayer.play(soundNumber); //play file on secondary player
				sec=1;
			}
		}
		if (sec==0) priPlayer.play(soundNumber); //play file on primary player
	#else
		priPlayer.play(soundNumber); //only one audio player is configured. very sad.
	#endif
}

//
//
//

///////function to set volume levels
void setVolume(byte vol) {
	// for WTV 7 = full volume, 0 off . for MP3 Tirgger 0 = full volume, 255 off 
	#if (AUDIO1==2)
		priPlayer.setVolume(vol); //set WTV volume
	#else
		priPlayer.setVolume(vLevels[vol]); //set Trigger volume
	#endif
	#if (AUDIO2>0)
		#if (AUDIO2==2)
			secPlayer.setVolume(vol); //set WTV volume
		#else
			secPlayer.setVolume(vLevels[vol]); //set Trigger volume
		#endif
	#endif
	playSound(54); //play a short sound so we know what the volume level is at
}

//
//
//

byte color=0; //initial PS4 LED color
unsigned int loopcount=0;
boolean reallyconnected=false; //initial connection state


void loop() {
	//
        Usb.Task();   
        
        if (PS4.connected()) {
          
          if (reallyconnected==0) {
            Serial.print(F("\r\nConnected"));
            reallyconnected=true;
            PS4.setLed(Green);
          }  
          
          //if (color!=1) { PS4.setLed(Green); color=1; }
          
          if (PS4.getButtonClick(TRIANGLE)) {
            Serial.print(F("\r\nPS"));
            PS4.setLed(Red); color=2;
          }  
          if (PS4.getButtonClick(SQUARE)) {
            Serial.print(F("\r\nSQ"));
            PS4.setLed(Blue); color=1;
          }  
          if (PS4.getButtonClick(CIRCLE)) {
            Serial.print(F("\r\nO"));
            PS4.setLed(Yellow); color=1;
          } 
          if (PS4.getButtonClick(CROSS)) {
            Serial.print(F("\r\nX"));
            PS4.setLed(Green); color=1;
          } 
          
        }  
        else {
          //Serial.print(F("\r\nNot Connected"));
          Serial.print(".");          
          if (loopcount==100) { loopcount=0; Serial.print("\nPair"); }
          delay(20);
        }
        
        
        //Serial.println(loopcount);
        if (loopcount==0) {
          //get battery voltage
          voltage=(float) (analogRead(VOLTPIN)/(VRATION*25));   
          //display some stuff      
          #if (DISPLAY==1)
            // picture loop
            u8g.firstPage();  
            do {
              //draw();
              u8g.setFont(u8g_font_unifont);
              /*u8g.drawStr( 0, 10, "P.A.D.A.W.A.N");
              u8g.drawStr( 0, 28, "Line Two");
              u8g.drawStr( 0, 46, "Third Line");*/
              
              //draw the battery...
              u8g.drawFrame(99,0,27,10);
              u8g.drawBox(101,2,23,6);
              u8g.drawBox(126,3,2,4);
              
              u8g.drawStr( 30, 62, "Battery ");
              u8g.setPrintPos(96,62);
              u8g.print(voltage);
              //u8g.print(voltage);
              
            } while( u8g.nextPage() );
          #endif
        
        }        
        loopcount++;
        if (loopcount==1000) loopcount=0;
        
}








/*
///////////////////////////////////////////////////////////////////////////////////
Sound file references...
0000.ad4	//Scream
0001.ad4	//Scream
0002.ad4	//Chortle
0003.ad4	//DooDoo
0004.ad4	//Wolf Whistle
0005.ad4	//Leia
0006.ad4	//Short Circuit
0007.ad4	//Patrol
0008.ad4	//Annoyed
0009.ad4	//Star Wars Theme
0010.ad4	//Cantina Song
0011.ad4	//Imperial March
0012.ad4	//Dual of the Fates
0013.ad4	//Alarm
0014.ad4	//Alarm
0015.ad4	//Alarm
0016.ad4	//Alarm
0017.ad4	//Misc
0018.ad4	//Misc
0019.ad4	//Misc
0020.ad4	//Misc
0021.ad4	//Misc
0022.ad4	//Misc
0023.ad4	//Misc
0024.ad4	//Misc
0025.ad4	//Ooh
0026.ad4	//Ooh
0027.ad4	//Ooh
0028.ad4	//Ooh
0029.ad4	//Ooh
0030.ad4	//Ooh
0031.ad4	//Ooh
0032.ad4	//Sent
0033.ad4	//Sent
0034.ad4	//Sent
0035.ad4	//Sent
0036.ad4	//Sent
0037.ad4	//Sent
0038.ad4	//Sent
0039.ad4	//Sent
0040.ad4	//Sent
0041.ad4	//Sent
0042.ad4	//Sent
0043.ad4	//Sent
0044.ad4	//Sent
0045.ad4	//Sent
0046.ad4	//Sent
0047.ad4	//Sent
0048.ad4	//Sent
0049.ad4	//Sent
0050.ad4	//Sent
0051.ad4	//Sent
0052.ad4	//Hum
0053.ad4	//Hum
///////////////////////////////////////////////////////////////////////////////////
*/



























