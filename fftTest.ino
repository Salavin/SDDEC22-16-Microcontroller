#include "MIDIUSB.h"

int adcReadAvg(int port, int nAvg);
int lookupFret(int adcVal);
int lookupNote(int str, int fret);

int strE = A0;
int strA = A1;
int strD = A2;
int strG = A3;
int strB = A4;
int strEe = A5; 
                    
int adcVal = 0;  
int fret = 0;
int notes[6] = {};    //MIDI notes associated with each string's current fret value

void setup() {
  Serial.begin(9600);       
  analogReadResolution(12); //12-bit mode
}

void loop() {
  int strings[6] = {};  //E A D G B e
  int newNotes[6] = {};    //MIDI notes associated with each string's current fret value
  int nAvg = 1; //number of ADC reads to take for average
  strings[0] = adcReadAvg(strE, nAvg);
  strings[1] = adcReadAvg(strA, nAvg);
  strings[2] = adcReadAvg(strD, nAvg);
  strings[3] = adcReadAvg(strG, nAvg);
  strings[4] = adcReadAvg(strB, nAvg);
  strings[5] = adcReadAvg(strEe, nAvg);

  Serial.println(strings[0]);

  for(int i=0; i<6; i++){ //convert ADC value to MIDI note for each string
    fret = lookupFret(strings[i]);
    newNotes[i] = lookupNote(i, fret);
  }
  for(int i = 0; i < 6; i++)
  {
    if (newNotes[i] != notes[i])
    {
      noteOff(0, notes[i], 100);
      delay(1);
      noteOn(0, newNotes[i], 100);
    }
    notes[i] = newNotes[i];
  }

  //Serial.println(fret);
  delay(100);
  
}

int adcReadAvg(int port, int nAvg){
  int avgVal = 0;
  for(int i=0; i<nAvg; i++){
    avgVal += analogRead(port);
  }
  avgVal = avgVal/nAvg;
  return avgVal;
}

int lookupFret(int adcVal){
  if(adcVal < 100) return 0;
  else if(adcVal>=200 && adcVal<=240) return 1;
  else if(adcVal>=241 && adcVal<=260) return 2;
  else if(adcVal>=260 && adcVal<=400) return 3;
  else if(adcVal>=400 && adcVal<=599) return 4;
  else if(adcVal>=700 && adcVal<=1200) return 5;
  else if(adcVal>=1201 && adcVal<=1499) return 6;
  else if(adcVal>=1500 && adcVal<=1699) return 7;
  else if(adcVal>=1700 && adcVal<=1999) return 8;
  else if(adcVal>=2000 && adcVal<=2199) return 9;
  else if(adcVal)
  // finish

  else return 12345;
}

int lookupNote(int str, int fret){
  int note = 0;
  if (str==0) note = fret+16;       //E
  else if(str==1) note = fret + 21; //A
  else if(str==2) note = fret + 26; //D
  else if(str==3) note = fret + 31; //G
  else if(str==4) note = fret + 35; //B
  else if(str==5) note = fret + 40; //e
  else return 0; //invalid string passed

  return note; //if octaves need adjusting, add or subtract 12 to this value per octave shift
}

void noteOn(byte channel, byte pitch, byte velocity) {

  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};

  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {

  midiEventPacket_t noteoff = {0x08, 0x80 | channel, pitch, velocity};

  MidiUSB.sendMIDI(noteoff);
}
