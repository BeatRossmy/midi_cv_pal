const float voltage_range = 4.024 * 1200;

struct CV_GATE_PORT {
  int8_t cs_pin;
  int8_t gate_pin;
  int8_t dac_channel;
  int8_t pitch_offset = 60;
  int8_t eeprom_address = 0;
  int16_t cv_calibration [10] = {0,500,1000,1500,2000,2500,3000,3500,4000,4095}; // [C2, F#2, ..., F#5, C6]
  int8_t cv_cal_offset [10] = {0,0,0,0,0,0,0,0,0,0}; // index [1,8] -> eeprom[0,7]

  void init () {
    pinMode(gate_pin,OUTPUT);
    pinMode(cs_pin,OUTPUT);

    for (int i=0; i<8; i++) {
      int index = eeprom_address+i;
      cv_cal_offset[i+1] = (int8_t)EEPROM.read(index);
    }
  }

  void writeDAC (int val) {
    digitalWriteFast(cs_pin, LOW);
    int dac = dac_channel & 1;
    val = val & 4095;
    SPI.transfer(dac << 7 | 0 << 5 | 1 << 4 | val >> 8);
    SPI.transfer(val & 255);
    digitalWriteFast(cs_pin, HIGH);
  }

  void transport_clock () {}
  void transport_start () {}
  void transport_continue () {}
  void transport_stop () {}

  int calc_pitch (int pitch) {
    pitch = constrain(pitch,pitch_offset,pitch_offset+48)-pitch_offset;
    int pitch_i = pitch/6;
    int cv_lower_bound = cv_calibration[pitch_i]+cv_cal_offset[pitch_i];
    int cv_upper_bound = cv_calibration[pitch_i+1]+cv_cal_offset[pitch_i+1];
    int dac_pitch = map(pitch%6,0,6,cv_lower_bound, cv_upper_bound);
    return constrain(dac_pitch,0,4095);
  }

  void noteOn (int pitch, int velocity, int channel) {
    int dac_pitch = calc_pitch(pitch);
    writeDAC(dac_pitch);
    digitalWrite(gate_pin,HIGH);
  }
  void noteOff(int pitch, int velocity, int channel) {
    digitalWrite(gate_pin,LOW);
  }

  void controlChange (byte number, byte value, byte channel) {
    int d =  (value<64)?-1:1;
    cv_cal_offset[number] = constrain(cv_cal_offset[number]+d,-128,127);
    int index = eeprom_address+number-1;
    EEPROM.write(index,(byte)cv_cal_offset[number]);
  }
};
