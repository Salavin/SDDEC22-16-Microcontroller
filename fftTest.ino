#include "MIDIUSB.h"

int adcReadAvg(int port, int nAvg);
int lookupFret(int string, int adcVal);
int lookupNote(int str, int fret);
int findHighestNote(int notes[6]);

const int strE = A0;
const int strA = A1;
const int strD = A2;
const int strG = A3;
const int strB = A4;
const int strEe = A5; 

const int BOTTOM_STRING_TO_READ = 5;
const int TOP_STRING_TO_READ = 6;
const int NUM_STRINGS = 6;
const int BAUD_RATE = 9600;
const int ANALOG_READ_RESOLUTION = 12;
                    
int adcVal = 0;  
int fret = 0;
int notes[NUM_STRINGS] = {0, 0, 0, 0, 0, 0};    //MIDI notes associated with each string's current fret value

void setup() {
  Serial.begin(BAUD_RATE);       
  analogReadResolution(ANALOG_READ_RESOLUTION); //12-bit mode
}

void loop()
{
  int strings[NUM_STRINGS] = {};  //E A D G B e
  int newNotes[NUM_STRINGS] = {};    //MIDI notes associated with each string's current fret value
  int frets[NUM_STRINGS] = {};
  int nAvg = 1; //number of ADC reads to take for average (leave at 1 for now, avg function broke)
  int strum = 0;

  //wait for strum (right now its just a button press)
  while (strum < 4000)
  {
    strum = analogRead(A7);
  }
  
  Serial.println(strum);

  //read string voltages
  strings[0] = adcReadAvg(strE, nAvg);
  strings[1] = adcReadAvg(strA, nAvg);
  strings[2] = adcReadAvg(strD, nAvg);
  strings[3] = adcReadAvg(strG, nAvg);
  strings[4] = adcReadAvg(strB, nAvg);
  strings[5] = adcReadAvg(strEe, nAvg);

  Serial.print(strings[5]);
  Serial.print("\t");

  for (int i = BOTTOM_STRING_TO_READ; i<NUM_STRINGS; i++)
  { //convert ADC value to MIDI note for each string
    frets[i] = lookupFret(i, strings[i]);
    newNotes[i] = lookupNote(i, frets[i]);
  }

  for (int i = BOTTOM_STRING_TO_READ; i < NUM_STRINGS; i++)
  {
    if (newNotes[i] != notes[i])
    {
      noteOff(0, notes[i] + 50, 100);
      delay(1);
      if (newNotes[i] > 0)
      {
        noteOn(0, newNotes[i] + 50, 100);
      }
    }
    notes[i] = newNotes[i];
  }

  for (int i = BOTTOM_STRING_TO_READ; i<NUM_STRINGS; i++)
  {
    Serial.print(frets[i]);
    Serial.print("\t");
  }

  Serial.print("Notes: ");
  for (int i = BOTTOM_STRING_TO_READ; i<NUM_STRINGS; i++)
  {
    Serial.print(notes[i]);
    Serial.print("\t");
  }
  //Serial.print(frets[5]);
  Serial.print("\n");

  //nextNote = findHighestNote(notes[]);

  // wait for strum button to be released before continuing so only one note is sent per button press
  while (strum>1000)
  {  
    strum = analogRead(A7);
  }
  delay(10);
}

int adcReadAvg(int port, int nAvg)
{
  int avgVal = 0;
  for(int i = 0; i < nAvg; i++)
  {
    avgVal += analogRead(port);
  }
  avgVal = avgVal/nAvg;
  return avgVal;
}

int lookupFret(int string, int adcVal)
{
  switch (string)
  {
    case 5:
      if(adcVal < 200) return 0;
      else if(adcVal >= 200 && adcVal <= 240) return 1;
      else if(adcVal >= 241 && adcVal <= 260) return 2;
      else if(adcVal >= 260 && adcVal <= 400) return 3;
      else if(adcVal >= 400 && adcVal <= 850) return 4;
      else if(adcVal >= 851 && adcVal <= 1200) return 5;
      else if(adcVal >= 1201 && adcVal <= 1399) return 6;
      else if(adcVal >= 1400 && adcVal <= 1550) return 7;
      else if(adcVal >= 1551 && adcVal <= 1850) return 8;
      else if(adcVal >= 1851 && adcVal <= 1999) return 9;
      else if(adcVal >= 2000 && adcVal <= 2189) return 10;
      else if(adcVal >= 2190 && adcVal <= 2300) return 11;
      else if(adcVal >= 2301 && adcVal <= 2550) return 12;
      break;
    case 4:
      if(adcVal < 200) return 0;
      else if(adcVal >= 200 && adcVal <= 240) return 1;
      else if(adcVal >= 241 && adcVal <= 260) return 2;
      else if(adcVal >= 260 && adcVal <= 400) return 3;
      else if(adcVal >= 400 && adcVal <= 850) return 4;
      else if(adcVal >= 851 && adcVal <= 1200) return 5;
      else if(adcVal >= 1201 && adcVal <= 1399) return 6;
      else if(adcVal >= 1400 && adcVal <= 1550) return 7;
      else if(adcVal >= 1551 && adcVal <= 1850) return 8;
      else if(adcVal >= 1851 && adcVal <= 1999) return 9;
      else if(adcVal >= 2000 && adcVal <= 2189) return 10;
      else if(adcVal >= 2190 && adcVal <= 2300) return 11;
      else if(adcVal >= 2301 && adcVal <= 2550) return 12;
      break;
    case 3:
      if(adcVal < 200) return 0;
      else if(adcVal >= 200 && adcVal <= 240) return 1;
      else if(adcVal >= 241 && adcVal <= 260) return 2;
      else if(adcVal >= 260 && adcVal <= 400) return 3;
      else if(adcVal >= 400 && adcVal <= 850) return 4;
      else if(adcVal >= 851 && adcVal <= 1200) return 5;
      else if(adcVal >= 1201 && adcVal <= 1399) return 6;
      else if(adcVal >= 1400 && adcVal <= 1550) return 7;
      else if(adcVal >= 1551 && adcVal <= 1850) return 8;
      else if(adcVal >= 1851 && adcVal <= 1999) return 9;
      else if(adcVal >= 2000 && adcVal <= 2189) return 10;
      else if(adcVal >= 2190 && adcVal <= 2300) return 11;
      else if(adcVal >= 2301 && adcVal <= 2550) return 12;
      break;
    case 2:
      if(adcVal < 200) return 0;
      else if(adcVal >= 200 && adcVal <= 240) return 1;
      else if(adcVal >= 241 && adcVal <= 260) return 2;
      else if(adcVal >= 260 && adcVal <= 400) return 3;
      else if(adcVal >= 400 && adcVal <= 850) return 4;
      else if(adcVal >= 851 && adcVal <= 1200) return 5;
      else if(adcVal >= 1201 && adcVal <= 1399) return 6;
      else if(adcVal >= 1400 && adcVal <= 1550) return 7;
      else if(adcVal >= 1551 && adcVal <= 1850) return 8;
      else if(adcVal >= 1851 && adcVal <= 1999) return 9;
      else if(adcVal >= 2000 && adcVal <= 2189) return 10;
      else if(adcVal >= 2190 && adcVal <= 2300) return 11;
      else if(adcVal >= 2301 && adcVal <= 2550) return 12;
      break;
    case 1:
      if(adcVal < 200) return 0;
      else if(adcVal >= 200 && adcVal <= 240) return 1;
      else if(adcVal >= 241 && adcVal <= 260) return 2;
      else if(adcVal >= 260 && adcVal <= 400) return 3;
      else if(adcVal >= 400 && adcVal <= 850) return 4;
      else if(adcVal >= 851 && adcVal <= 1200) return 5;
      else if(adcVal >= 1201 && adcVal <= 1399) return 6;
      else if(adcVal >= 1400 && adcVal <= 1550) return 7;
      else if(adcVal >= 1551 && adcVal <= 1850) return 8;
      else if(adcVal >= 1851 && adcVal <= 1999) return 9;
      else if(adcVal >= 2000 && adcVal <= 2189) return 10;
      else if(adcVal >= 2190 && adcVal <= 2300) return 11;
      else if(adcVal >= 2301 && adcVal <= 2550) return 12;
      break;
    case 0:
      if(adcVal < 200) return 0;
      else if(adcVal >= 200 && adcVal <= 240) return 1;
      else if(adcVal >= 241 && adcVal <= 260) return 2;
      else if(adcVal >= 260 && adcVal <= 400) return 3;
      else if(adcVal >= 400 && adcVal <= 850) return 4;
      else if(adcVal >= 851 && adcVal <= 1200) return 5;
      else if(adcVal >= 1201 && adcVal <= 1399) return 6;
      else if(adcVal >= 1400 && adcVal <= 1550) return 7;
      else if(adcVal >= 1551 && adcVal <= 1850) return 8;
      else if(adcVal >= 1851 && adcVal <= 1999) return 9;
      else if(adcVal >= 2000 && adcVal <= 2189) return 10;
      else if(adcVal >= 2190 && adcVal <= 2300) return 11;
      else if(adcVal >= 2301 && adcVal <= 2550) return 12;
      break;
    default:
      return 0;
  }
}

int lookupNote(int str, int fret){
  int note = 0;
  switch (str)
  {
    case 0:
      note = fret + 16;  // E
      break;
    case 1: 
      note = fret + 21;
      break;
    case 2:
      note = fret + 26;  // D
      break;
    case 3:
      note = fret + 31;  // G
      break;
    case 4:
      note = fret + 35;  // B
      break;
    case 5:
      note = fret + 40;  // e
      break;
    default:
      return 0;
  }
  return note - 12; //if octaves need adjusting, add or subtract 12 to this value per octave shift
}

void noteOn(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteoff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteoff);
}

int findHighestNote(int notes[6])
{
  int highestNote = 0;
  for (int i = 0; i < 6; i++)
  {
    if (highestNote < notes[i]) highestNote = notes[i];    
  }
  return highestNote;
}