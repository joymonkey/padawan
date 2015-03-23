/*
 Wtv.cpp Version 1.1 
 Library to control a WTV020-SD-16P module to play voices from an Arduino board.
 Created by Diego J. Arevalo, August 6th, 2012.
 Modifed by Ryszard Malinowski November 30, 2014.
 Modifed by Danf, Feb. 2015
  
 Released into the public domain.
 
 Changes:
 V1.0 - Original library written by Diego J. Arevalo
 V1.1 - Changes by Ryszard Malinowski:
        1. Add "delayMicros" function execute exact delay from the last call
        2. Fix clock timing in "setCommand" function
        3. Add "setVolume" function
        4. Fix "unmute" function to restore original volume

Danf-- removed reset and busy lines
 */

#include "Arduino.h"
#include "Wtv.h"

const unsigned int PLAY_PAUSE = 0xFFFE;
const unsigned int STOP = 0xFFFF;
const unsigned int VOLUME_MIN = 0xFFF0;
const unsigned int VOLUME_MAX = 0xFFF7;

Wtv::Wtv(int clockPin,int dataPin)
{
  // Save private values from constructor
 
  _clockPin=clockPin;
  _dataPin=dataPin;
  
  
  // Set initial values
  
  _currentVolume = VOLUME_MAX;

  // Initialize timer
  _lastMicros = 0;
  delayMicros(0);

  // Set pins
  
  pinMode(_clockPin, OUTPUT);
  pinMode(_dataPin, OUTPUT);
 

}


// Public: Start playing song and return
void Wtv::play(unsigned int voiceNumber){
  sendCommand(voiceNumber);
}

// Public: Stop playing song
void Wtv::stop(){
  sendCommand(STOP);
}

// Public: Pause/Resume song
void Wtv::pause(){
  sendCommand(PLAY_PAUSE);
}

// Public: Mute sound - same as setVolume(0)
void Wtv::mute(){
  sendCommand(VOLUME_MIN);
}

// Public: Unmute sound restoring the previous volume
void Wtv::unmute(){
  sendCommand(_currentVolume);
}

// Public: Set volume
void Wtv::setVolume(unsigned int volume){
  if(volume < 0x0008) {
    _currentVolume = VOLUME_MIN + volume;
    sendCommand(_currentVolume);
  }
}


// Private: Send command to sound module
void Wtv::sendCommand(unsigned int command) {
  
  //Start bit Low level pulse.
  digitalWrite(_clockPin, LOW);
  // Initialize timer
  delayMicros(0);
  // Wait Start bit length minus 50 us
  delayMicros(1950);
  for (unsigned int mask = 0x8000; mask > 0; mask >>= 1) {
    //Clock low level pulse.
    digitalWrite(_clockPin, LOW);
    delayMicros(50);
    //Write data setup.
    if (command & mask) {
      digitalWrite(_dataPin, HIGH);
    } else {
      digitalWrite(_dataPin, LOW);
    }
    //Write data hold.
    delayMicros(50);
    //Clock high level pulse.
    digitalWrite(_clockPin, HIGH);
    delayMicros(100);
  }
  //Stop bit high level pulse (additional 1900us).
  delayMicros(1900);
}


// Private: Wait specified time since the last wait
void Wtv::delayMicros(unsigned long delayMicros) {
  unsigned long stopMicros;
  unsigned long currentMicros;
  // set current time
  if (delayMicros == 0) {
    _lastMicros = micros();
	return;
  }
  // Start waiting loop
  stopMicros = _lastMicros + delayMicros;
  currentMicros = micros();
  // If time already passed then return withoit delay
  if ((stopMicros  == currentMicros) || (_lastMicros < stopMicros  && stopMicros < currentMicros) || (stopMicros < currentMicros && currentMicros < _lastMicros)) {
	  _lastMicros = currentMicros;
  } else {
    // Wait remaining time left 
    delayMicroseconds(stopMicros - currentMicros);
    _lastMicros = stopMicros;
  }
  
}
  

