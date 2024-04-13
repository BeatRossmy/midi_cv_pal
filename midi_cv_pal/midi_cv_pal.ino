#include <SPI.h>
#include <EEPROM.h>

#include "cv_gate_port.h"
//                    cs,g,dac
CV_GATE_PORT cvg_A = {21,2,0,60,0};
CV_GATE_PORT cvg_B = {21,3,1,60,16};

// MIDI OUT PINS: 1,10,8
#include "midi_ports.h"

#define led_pin 13

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI_A);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI_B);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI_C);

MIDI_PORT midi_A = {&MIDI_A,32};
MIDI_PORT midi_B = {&MIDI_B,48};
MIDI_PORT midi_C = {&MIDI_C,64};
MIDI_PORT* ports [] = {&midi_A, &midi_B, &midi_C};

boolean activity;
//elapsedMillis ledOnMillis;

void onClock () {
  for (int p=0; p<3; p++) {
    if (ports[p]->forward_clock) ports[p]->transport_clock();
  }
}
void onStart () {
  for (int p=0; p<3; p++) {
    if (ports[p]->forward_clock) ports[p]->transport_start();
  }
}
void onContinue () {
  for (int p=0; p<3; p++) {
    if (ports[p]->forward_clock) ports[p]->transport_continue();
  }
}
void onStop () {
  for (int p=0; p<3; p++) {
    if (ports[p]->forward_clock) ports[p]->transport_stop();
  }
}

void onNoteOn (byte channel, byte pitch, byte velocity, byte cable) {
  if (channel>=1 && channel<=2)
    ports[0]->noteOn(pitch,velocity,channel);
  else if (channel>=3 && channel<=6)
    ports[1]->noteOn(pitch,velocity,channel);
  else if (channel>=7 && channel<=14)
    ports[2]->noteOn(pitch,velocity,channel);
  else if (channel==15)
    cvg_A.noteOn(pitch,velocity,channel);
  else if (channel==16) 
    cvg_B.noteOn(pitch,velocity,channel);
}

void onNoteOff (byte channel, byte pitch, byte velocity, byte cable) {
  if (channel>=1 && channel<=2)
    ports[0]->noteOff(pitch,velocity,channel);
  else if (channel>=3 && channel<=6)
    ports[1]->noteOff(pitch,velocity,channel);
  else if (channel>=7 && channel<=14)
    ports[2]->noteOff(pitch,velocity,channel);
  else if (channel==15)
    cvg_A.noteOff(pitch,velocity,channel);
  else if (channel==16)
    cvg_B.noteOff(pitch,velocity,channel);
}

void onControlChange (byte channel, byte control, byte value) {
  if (channel==15)
    cvg_A.controlChange(control,value,channel);
  else if (channel==16)
    cvg_B.controlChange(control,value,channel);
}

void setup() {
  SPI.begin();

  //pinMode(led_pin,OUTPUT);

  // CLEAR EEPROM -> RESET SETTINGS
  //for (int i=0; i<128; i++) EEPROM.write(i,0);

  cvg_A.init();
  cvg_B.init();

  midi_A.init();
  midi_B.init();
  midi_C.init();
  
  //usbMIDI.setHandleNoteOff(onNoteOff);
  //usbMIDI.setHandleNoteOn(onNoteOn);
  //usbMIDI.setHandleClock(onClock);
  //usbMIDI.setHandleStart(onStart);
  //usbMIDI.setHandleContinue(onContinue);
  //usbMIDI.setHandleStop(onStop);
  //usbMIDI.setHandleControlChange(onControlChange);
}

void handle_cv_pal (byte type, byte data1, byte data2, byte channel) {
  if (type==144) { // 0x90
    activity = true;
    if (channel==1) cvg_A.noteOn(data1,data2,channel);
    else if (channel==2) cvg_B.noteOn(data1,data2,channel);
  }
  else if (type==128) { // 0x80
    if (channel==1) cvg_A.noteOff(data1,data2,channel);
    else if (channel==2) cvg_B.noteOff(data1,data2,channel);
  }
}

void loop() {
  //while (usbMIDI.read()) {}

  activity = false;

  if (usbMIDI.read()) {
    byte type = usbMIDI.getType();
    byte channel = usbMIDI.getChannel();
    byte data1 = usbMIDI.getData1();
    byte data2 = usbMIDI.getData2();
    byte cable = usbMIDI.getCable();

    if (type != usbMIDI.SystemExclusive) {
      midi::MidiType mtype = (midi::MidiType)type;
      switch (cable) {
        case 0:
          MIDI_A.send(mtype, data1, data2, channel);
          break;
        case 1:
          MIDI_B.send(mtype, data1, data2, channel);
          break;
        case 2:
          MIDI_C.send(mtype, data1, data2, channel);
          break;
        case 3:
          handle_cv_pal(type, data1, data2, channel);
          break;
      }

    } else {}
    //activity = true;
  }

  // blink the LED when any activity has happened
  //if (activity) {
    //digitalWriteFast(13, HIGH); // LED on
    //ledOnMillis = 0;
  //}
  //if (ledOnMillis > 15) {
    //digitalWriteFast(13, LOW);  // LED off
  //}
}
