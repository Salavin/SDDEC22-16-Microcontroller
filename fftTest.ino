#include "MIDIUSB.h"

int adcReadAvg(int port, int nAvg);
int lookupFret(int string, int adcVal);
int lookupNote(int str, int fret);
int findHighestNote(int notes[6]);

void findFretAvg(int str);

int strE = A0;
int strA = A1;
int strD = A2;
int strG = A3;
int strB = A4;
int strEe = A5; 
                    
int adcVal = 0;  
int fret = 0;
int notes[6] = {0, 0, 0, 0, 0, 0};    //MIDI notes associated with each string's current fret value

void setup() {
  Serial.begin(9600);       
  analogReadResolution(12); //12-bit mode
}

void loop() {
  int strings[6] = {};  //E A D G B e
  int newNotes[6] = {};    //MIDI notes associated with each string's current fret value
  int frets[6] = {};
  int nAvg = 50; //number of ADC reads to take for average (leave at 1 for now, avg function broke)
  int strum = 0;


  //wait for strum (right now its just a button press)
  while(strum<4000){
    strum = analogRead(A7);
  }

  //read string voltages
  strings[0] = adcReadAvg(strE, nAvg);
  strings[1] = adcReadAvg(strA, nAvg);
  strings[2] = adcReadAvg(strD, nAvg);
  strings[3] = adcReadAvg(strG, nAvg);
  strings[4] = adcReadAvg(strB, nAvg);
  strings[5] = adcReadAvg(strEe, nAvg);

//THIS PRINTS THE AVG ADC VAL FOR EACH STRING FOR THE PURPOSE OF BUILING LOOKUP TABLES
  Serial.print(strings[0]);
  Serial.print("\t");
  Serial.print(strings[1]);
  Serial.print("\t");
  Serial.print(strings[2]);
  Serial.print("\t");
  Serial.print(strings[3]);
  Serial.print("\t");
  Serial.print(strings[4]);
  Serial.print("\t");
  Serial.print(strings[5]);
  Serial.print("\t");

  for(int i=0; i<6; i++){ //convert ADC value to MIDI note for each string
    frets[i] = lookupFret(i, strings[i]);
    newNotes[i] = lookupNote(i, frets[i]);
  }

  for(int i = 5; i < 6; i++)
  {
    if (newNotes[i] != notes[i])
    {
      noteOff(0, notes[i] + 50, 100);
      delay(1);
      if (newNotes[i] > 0)
      {
        //Serial.print("New note at " + newNotes[i] + 50);
        noteOn(0, newNotes[i] + 50, 100);
      }
    }
    notes[i] = newNotes[i];
  }
/*
  for(int i=0; i<6;i++){
    Serial.print(frets[i]);
    Serial.print("\t");
  }
  //Serial.print("Notes");
  
  for(int i=0; i<6;i++){
    Serial.print(notes[i]);
    Serial.print("\t");
  }
  */

  Serial.print("\n");

  //nextNote = findHighestNote(notes[]);

  // wait for strum button to be released before continuing so only one note is sent per button press
  while(strum>1000){  
    strum = analogRead(A7);
    //Serial.println(strum);
  }
  for (int i = 5; i < 6; i++)
  {
    noteOff(0, notes[i], 100);
  }
  delay(10);
  
}

int adcReadAvg(int port, int nAvg){
  int current = 0;
  int sum = 0;
  int max = 0;
  int min = 99999;
  float avg = 0;
  
  for(int i=0; i<nAvg; i++){
    current = analogRead(port);
    //Serial.println(current);
    sum += current;
    if(current > max) max = current;
    if(current < min) min = current;
    delayMicroseconds(100);
  }
  avg = sum/nAvg;
/*
  Serial.print("nAvg = ");
  Serial.print(nAvg);
  Serial.print("\n Max = ");
  Serial.print(max);
  Serial.print("\n Min = ");
  Serial.print(min);
  Serial.print("\n Average = ");
  Serial.print(avg);
  Serial.print("\n\n");
    */
  return avg;
}

int lookupFret(int string, int adcVal) 
{
    if(adcVal < 200) return 0;
    else if(adcVal>=200 && adcVal<=274) return 1;
    else if(adcVal>=275 && adcVal<=299) return 2;
    else if(adcVal>=300 && adcVal<=599) return 3;
    else if(adcVal>=800 && adcVal<=899) return 4;
    else if(adcVal>=1100 && adcVal<=1199) return 5;
    else if(adcVal>=1300 && adcVal<=1499) return 6;
    else if(adcVal>=1600 && adcVal<=1799) return 7;
    else if(adcVal>=1800 && adcVal<=2199) return 8;
    else if(adcVal>=2300 && adcVal<=2399) return 9;
    else if(adcVal>=2400 && adcVal<=2699) return 10;
    else if(adcVal>=2600 && adcVal<=2924) return 11;
    else if(adcVal>=2925 && adcVal<=3214) return 12;
    else if(adcVal>=3215 && adcVal<=3399) return 13;
    else if(adcVal>=3500 && adcVal<=3799) return 14;
    else if(adcVal>=3800) return 15;
    else return 0;
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

  return note-12; //if octaves need adjusting, add or subtract 12 to this value per octave shift
}

void noteOn(byte channel, byte pitch, byte velocity) {

  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};

  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {

  midiEventPacket_t noteoff = {0x08, 0x80 | channel, pitch, velocity};

  MidiUSB.sendMIDI(noteoff);
}

int findHighestNote(int notes[6]){
  int highestNote = 0;
  for(int i=0; i<6; i++){
    if(highestNote<notes[i]) highestNote = notes[i];    
  }
  return highestNote;
}

void findFretAvg(int str){
  int onOff = 0;
  int sum = 0;
  int current = 0;
  int i = 0;
  float avg = 0;
  int max = 0;
  int min = 99999;

  while(onOff<1000){
    onOff = analogRead(A7);
  }

  sum = 0;
  current = 0;
  i = 0;
  avg = 0;
  max = 0;
  min = 99999;    
  while(onOff>1000){
    i++;
    current = analogRead(str);
    sum += current;
    if(current>max) max = current;
    if(current<min) min = current;
    onOff = analogRead(A7);    
    delay(10);

  }  
  avg = sum/i;
  Serial.print("i = ");
  Serial.print(i);
  Serial.print("\n Max = ");
  Serial.print(max);
  Serial.print("\n Min = ");
  Serial.print(min);
  Serial.print("\n Average = ");
  Serial.print(avg);
  Serial.print("\n\n");
  
}