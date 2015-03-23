/*
 Wtv.h - Library to control a WTV020-SD-16P module to play voices from an Arduino board.
 Created by Diego J. Arevalo, August 6th, 2012.
 Released into the public domain.
 */

#ifndef Wtv_h
#define Wtv_h

class Wtv
{
public:
  Wtv(int clockPin,int dataPin);
  void reset();
  
  void play(unsigned int voiceNumber);
  void stop();
  void pause();
  void mute();
  void unmute();
  void setVolume(unsigned int volume);

private:
  void sendCommand(unsigned int command);
  void delayMicros(unsigned long delayMicros);
  
  int _clockPin;
  int _dataPin;
  
  
  unsigned int _currentVolume;
  unsigned long _lastMicros;
};

#endif


