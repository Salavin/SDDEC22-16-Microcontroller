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

//Midi value offset for each string
int offsets[6] = {16, 21, 26, 31, 35, 40};
                    
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
  int nAvg = 100; //number of ADC reads to take for average (leave at 1 for now, avg function broke)
  int strum = 0;

  //while(1){ //THIS IS JUST FOR FINDING FRET ADC VALS DONT LEAVE IN!
  //  findFretAvg(strB);
  //}

  //wait for strum (right now its just a button press)
  while(strum<4000){
    strum = analogRead(A7);
  }
  
  //Serial.println(strum);

  //read string voltages
  strings[0] = adcReadAvg(strE, nAvg);
  strings[1] = adcReadAvg(strA, nAvg);
  strings[2] = adcReadAvg(strD, nAvg);
  strings[3] = adcReadAvg(strG, nAvg);
  strings[4] = adcReadAvg(strB, nAvg);
  strings[5] = adcReadAvg(strEe, nAvg);

  Serial.print(strings[5]);
  Serial.print("\t");

  for(int i=5; i<6; i++){ //convert ADC value to MIDI note for each string
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

  for(int i=5; i<6;i++){
    Serial.print(frets[i]);
    Serial.print("\t");
  }
  Serial.print("Notes");
  for(int i=5; i<6;i++){
    Serial.print(notes[i]);
    Serial.print("\t");
  }
  //Serial.print(frets[5]);
  Serial.print("\n");

  //nextNote = findHighestNote(notes[]);

  // wait for strum button to be released before continuing so only one note is sent per button press
  while(strum>1000){  
    strum = analogRead(A7);
  }
  for (int i = 5; i < 6; i++)
  {
    noteOff(0, notes[i], 100);
  }
  delay(10);

  //Detect Application Communication
  receiveAndSetConfig();
  
}

void receiveAndSetConfig(){
  int SOM = 60;
  int EOM = 50;
  midiEventPacket_t general = MidiUSB.read();
  
  if(general.byte2 == SOM)
  {
    int iter = 0; //offset list iterator
    midiEventPacket_t data;
    while(true)
    {        
      midiEventPacket_t data = MidiUSB.read();
      if(data.byte2 == EOM)  break;
      else if(data.byte2 != 0)
      {
        offset[iter++] = data.byte2;
      }
    }
  }
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

  Serial.print("nAvg = ");
  Serial.print(nAvg);
  Serial.print("\n Max = ");
  Serial.print(max);
  Serial.print("\n Min = ");
  Serial.print(min);
  Serial.print("\n Average = ");
  Serial.print(avg);
  Serial.print("\n\n");
    
  return avg;
}

int lookupFret(int string, int adcVal){
  if(string==5){ //high e
    if(adcVal < 200) return 0;
    else if(adcVal>=200 && adcVal<=240) return 1;
    else if(adcVal>=241 && adcVal<=260) return 2;
    else if(adcVal>=260 && adcVal<=400) return 3;
    else if(adcVal>=400 && adcVal<=850) return 4;
    else if(adcVal>=851 && adcVal<=1200) return 5;
    else if(adcVal>=1201 && adcVal<=1399) return 6;
    else if(adcVal>=1400 && adcVal<=1550) return 7;
    else if(adcVal>=1551 && adcVal<=1850) return 8;
    else if(adcVal>=1851 && adcVal<=1999) return 9;
    else if(adcVal>=2000 && adcVal<=2189) return 10;
    else if(adcVal>=2190 && adcVal<=2300) return 11;
    else if(adcVal>=2301 && adcVal<=2550) return 12;
    else return 0;    
  }

  if(string==4){ //B
    if(adcVal < 50) return 0;
    else if(adcVal>=200 && adcVal<=240) return 1;
    else if(adcVal>=241 && adcVal<=260) return 2;
    else if(adcVal>=300 && adcVal<=550) return 3;
    else if(adcVal>=551 && adcVal<=900) return 4;
    else if(adcVal>=901 && adcVal<=1150) return 5;
    else if(adcVal>=1151 && adcVal<=1350) return 6;
    else if(adcVal>=1351 && adcVal<=1600) return 7;
    else if(adcVal>=1601 && adcVal<=1850) return 8;
    else if(adcVal>=1851 && adcVal<=2100) return 9;
    else if(adcVal>=2101 && adcVal<=2270) return 10;
    else if(adcVal>=2271 && adcVal<=2350) return 11;
    else if(adcVal>=2351 && adcVal<=2550) return 12;
    //else if(adcVal>=2351 && adcVal<=2550) return 13;
    //else if(adcVal>=2351 && adcVal<=2550) return 14;
    //else if(adcVal>=2351 && adcVal<=2550) return 15;
    //else if(adcVal>=2351 && adcVal<=2550) return 16;
    //else if(adcVal>=2351 && adcVal<=2550) return 17;
    else return 0;
  }

  if(string==3){ //G
    if(adcVal < 200) return 0;
    else if(adcVal>=200 && adcVal<=240) return 1;
    else if(adcVal>=241 && adcVal<=260) return 2;
    else if(adcVal>=260 && adcVal<=400) return 3;
    else if(adcVal>=400 && adcVal<=850) return 4;
    else if(adcVal>=851 && adcVal<=1200) return 5;
    else if(adcVal>=1201 && adcVal<=1399) return 6;
    else if(adcVal>=1400 && adcVal<=1550) return 7;
    else if(adcVal>=1551 && adcVal<=1850) return 8;
    else if(adcVal>=1851 && adcVal<=1999) return 9;
    else if(adcVal>=2000 && adcVal<=2189) return 10;
    else if(adcVal>=2190 && adcVal<=2300) return 11;
    else if(adcVal>=2301 && adcVal<=2550) return 12;
    else return 0;
  }

  if(string==2){ //D
    if(adcVal < 200) return 0;
    else if(adcVal>=200 && adcVal<=240) return 1;
    else if(adcVal>=241 && adcVal<=260) return 2;
    else if(adcVal>=260 && adcVal<=400) return 3;
    else if(adcVal>=400 && adcVal<=850) return 4;
    else if(adcVal>=851 && adcVal<=1200) return 5;
    else if(adcVal>=1201 && adcVal<=1399) return 6;
    else if(adcVal>=1400 && adcVal<=1550) return 7;
    else if(adcVal>=1551 && adcVal<=1850) return 8;
    else if(adcVal>=1851 && adcVal<=1999) return 9;
    else if(adcVal>=2000 && adcVal<=2189) return 10;
    else if(adcVal>=2190 && adcVal<=2300) return 11;
    else if(adcVal>=2301 && adcVal<=2550) return 12;
    else return 0;
  }

  if(string==1){ //A
    if(adcVal < 200) return 0;
    else if(adcVal>=200 && adcVal<=240) return 1;
    else if(adcVal>=241 && adcVal<=260) return 2;
    else if(adcVal>=260 && adcVal<=400) return 3;
    else if(adcVal>=400 && adcVal<=850) return 4;
    else if(adcVal>=851 && adcVal<=1200) return 5;
    else if(adcVal>=1201 && adcVal<=1399) return 6;
    else if(adcVal>=1400 && adcVal<=1550) return 7;
    else if(adcVal>=1551 && adcVal<=1850) return 8;
    else if(adcVal>=1851 && adcVal<=1999) return 9;
    else if(adcVal>=2000 && adcVal<=2189) return 10;
    else if(adcVal>=2190 && adcVal<=2300) return 11;
    else if(adcVal>=2301 && adcVal<=2550) return 12;
    else return 0;
  }

  if(string==0){ //low E
    if(adcVal < 200) return 0;
    else if(adcVal>=200 && adcVal<=240) return 1;
    else if(adcVal>=241 && adcVal<=260) return 2;
    else if(adcVal>=260 && adcVal<=400) return 3;
    else if(adcVal>=400 && adcVal<=850) return 4;
    else if(adcVal>=851 && adcVal<=1200) return 5;
    else if(adcVal>=1201 && adcVal<=1399) return 6;
    else if(adcVal>=1400 && adcVal<=1550) return 7;
    else if(adcVal>=1551 && adcVal<=1850) return 8;
    else if(adcVal>=1851 && adcVal<=1999) return 9;
    else if(adcVal>=2000 && adcVal<=2189) return 10;
    else if(adcVal>=2190 && adcVal<=2300) return 11;
    else if(adcVal>=2301 && adcVal<=2550) return 12;
    else return 0;
  }

  return 0;
}

int lookupNote(int str, int fret){
  int note = 0;
  if (str==0) note = fret     + offset[0]; //E
  else if(str==1) note = fret + offset[1]; //A
  else if(str==2) note = fret + offset[2]; //D
  else if(str==3) note = fret + offset[3]; //G
  else if(str==4) note = fret + offset[4]; //B
  else if(str==5) note = fret + offset[5]; //e
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
