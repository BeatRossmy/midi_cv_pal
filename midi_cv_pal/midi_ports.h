#include <MIDI.h>

int NoteOff               = 0x80; ///< Note Off
int NoteOn                = 0x90; ///< Note On
int AfterTouchPoly        = 0xA0; ///< Polyphonic AfterTouch
int ControlChange         = 0xB0; ///< Control Change / Channel Mode
int ProgramChange         = 0xC0; ///< Program Change
int AfterTouchChannel     = 0xD0; ///< Channel (monophonic) AfterTouch
int PitchBend             = 0xE0; ///< Pitch Bend
int SystemExclusive       = 0xF0; ///< System Exclusive
int TimeCodeQuarterFrame  = 0xF1; ///< System Common - MIDI Time Code Quarter Frame
int SongPosition          = 0xF2; ///< System Common - Song Position Pointer
int SongSelect            = 0xF3; ///< System Common - Song Select
int TuneRequest           = 0xF6; ///< System Common - Tune Request
int Clock                 = 0xF8; ///< System Real Time - Timing Clock
int Start                 = 0xFA; ///< System Real Time - Start
int Continue              = 0xFB; ///< System Real Time - Continue
int Stop                  = 0xFC; ///< System Real Time - Stop
int ActiveSensing         = 0xFE; ///< System Real Time - Active Sensing
int SystemReset           = 0xFF; ///< System Real Time - System Reset
int InvalidType           = 0x00; ///< For notifying errors

struct MIDI_PORT {
  midi::MidiInterface<HardwareSerial> * port;
  boolean forward_clock = true;
  boolean forward_cc = false;

  void init () {
    port->begin();  
  }

  void transport_clock () {
    if (!forward_clock) return;
    port->sendRealTime((midi::MidiType)Clock);
  }
  void transport_start () {
    if (!forward_clock) return;
    port->sendRealTime((midi::MidiType)Start);
  }
  void transport_continue () {
    if (!forward_clock) return;
    port->sendRealTime((midi::MidiType)Continue);  
  }
  void transport_stop () {
    if (!forward_clock) return;
    port->sendRealTime((midi::MidiType)Stop);
  }

  void noteOn (int pitch, int velocity, int channel) {
    port->sendNoteOn(pitch, velocity, channel);
  }
  void noteOff(int pitch, int velocity, int channel) {
    port->sendNoteOff(pitch, velocity, channel);
  }

  void controlChange (int number, int value, int channel) {
    if (!forward_cc) return;
    port->sendControlChange(number,value,channel);  
  }
};
