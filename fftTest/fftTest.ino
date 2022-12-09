#include <arduinoFFT.h>
#include "MIDIUSB.h"
#include <math.h>

void waitForStrumDecay(int nStrAvgMov);
int adcReadAvg(int port, int nAvg);
int lookupFret(int string, int adcVal);
int lookupNote(int str, int fret);
int findHighestNote(int notes[6]);
void findFretAvg(int str);

const int strE = A0;
const int strA = A1;
const int strD = A2;
const int strG = A3;
const int strB = A4;
const int strEe = A5; 
const int stringVals[6] = {strE, strA, strD, strG, strB, strEe};

const int BOTTOM_STRING_TO_READ = 0;
const int TOP_STRING_TO_READ = 6;
const int NUM_STRINGS = 6;
const int BAUD_RATE = 9600;
const int ANALOG_READ_RESOLUTION = 12;
const int nStrAvg = 1000;
           
int adcVal = 0;  
int fret = 0;
int notes[NUM_STRINGS] = {0, 0, 0, 0, 0, 0};    //MIDI notes associated with each string's current fret value
int strings[NUM_STRINGS] = {};  //E A D G B e
int frets[NUM_STRINGS] = {};
int nAvg = 100; //number of ADC reads to take for average when reading fret values
int strum = 0;
int strumAvg = 0;
int zeroPoint = 2035;   // Reassigned in setup(), used in waitForStrumDecay()
int decayThresh = 2100; // Reassigned in setup(), used in waitForStrumDecay()
int attackThresh = 2100; //Reassigned in setup(), used in loop() to determine when a string has been plucked
int currentNote = 0;
int maxFret = 0;
int currentString = 0;

int offset[6] = {16, 21, 26, 31, 35, 40};

void setup()
{
  Serial.begin(BAUD_RATE);       
  analogReadResolution(ANALOG_READ_RESOLUTION); //12-bit mode

  //These next few lines determines what the "zero point" will be for the ADC input used for strum detection
  //Make sure guitar is silenced when powering on microcontroller!
  int i = 0;
  int sum = 0;
  delay(100);
  while(i<100000){
    sum += analogRead(A7);
    zeroPoint = sum/i;
    i++;
  }
  decayThresh = zeroPoint + 20;
  attackThresh = zeroPoint + 125;
}

void loop()
{
  maxFret = 0;        //resets for each loop - used for "currentNote"
  currentString = 0;  //resets for each loop - used for "currentNote"
  int newNotes[NUM_STRINGS] = {};    //MIDI notes associated with each string's current fret value
  bool noteChanged[6] = {false, false, false, false, false, false};
  bool isOpen = false;
  int numSamples = 1024;
  float samples[numSamples];
  float samImg[numSamples];

  //wait for strum
  while (strum < attackThresh)
  {
    receiveAndSetConfig();
    strum = analogRead(A7);
  }

  //Read strings
  for (int i = 0; i < 6; i++)
  {
    strings[i] = adcReadAvg(stringVals[i], nAvg);
    frets[i] = lookupFret(i, strings[i]);

    Serial.print(frets[i]);
    Serial.print("\t");
  }

  //Highest *fret* (not necessarialy highest *note*) becomes current note to be sent
  for(int i = BOTTOM_STRING_TO_READ; i < NUM_STRINGS; i++){
    if (frets[i] > maxFret)
    {
      maxFret = frets[i];
      currentString = i;
    }
  }
  currentNote = lookupNote(currentString, maxFret);

  //Send note
  noteOn(0, currentNote, 100);
  Serial.print("Note: ");
  Serial.println(currentNote);

  //Wait for note to decay, then turn off
  waitForStrumDecay(nStrAvg);
  strum = 0;
  noteOff(0, currentNote, 100);  
  
  //Check for new Midi config and set config 
  //receiveAndSetConfig();
  delay(1); //this delay might not be needed anymore
}

/***************************************************
  End Main Loop                                    
****************************************************/

void waitForStrumDecay(int nStrAvgMov){
  int strAvg = 0;
  int rms = 0;
  int current = 0;
  int sum = 0;
  int i = 1;
  int j = 0;


  int sum1 = 0;  
  int sampleSize = 10000;
  while(1){
    current = analogRead(A7);
    if(current < zeroPoint){ //rectify around "zero" point of ADC
      current = zeroPoint - current;
      current += zeroPoint;
    }
    if ((i % sampleSize) == 0){
      strAvg = sum1/sampleSize;
      sum1 = 0;
    }
    
    sum1 += current; 

    if ((i % sampleSize) == 0) {
      if (strAvg < decayThresh) break;
    }  

    i++;  
  }
}


void receiveAndSetConfig(){
  int SOM = 6;
  int EOM = 5;
  midiEventPacket_t general = MidiUSB.read();
  
  if(general.byte2 == SOM)
  {
    Serial.print("Uploading config...");
    int iter = 0; //offset list iterator
    int dupIter = 0;
    int seen[30]; 
    midiEventPacket_t data;
    while(true)
    {       

      midiEventPacket_t data = MidiUSB.read();

 
      if(data.byte2 == EOM) {
        break;
      }
      else if(data.byte2 != 0)
      {
        
        if(offset[iter-1] != data.byte2 && data.byte2 != SOM && data.byte2 != EOM) {

          offset[iter++] = data.byte2;
        }
        
        seen[dupIter++] == data.byte2;
      }

    }
    Serial.print("Done loading config");
  }
  
}

int adcReadAvg(int port, int nAvg){
  int current = 0;
  int sum = 0;
  int max = 0;
  int min = 99999;
  float avg = 0;
  
  for (int i = 0; i < nAvg; i++)
  {
    current = analogRead(port);
    sum += current;
    if (current > max) max = current;
    if (current < min) min = current;
    delayMicroseconds(100);
  }
  avg = sum / nAvg;
  return avg;
}



int lookupFret(int string, int adcVal) 
{
    if (adcVal < 200) return 0;                           //Average Values:
    else if (adcVal >= 200 && adcVal <= 274) return 1;    //264
    else if (adcVal >= 275 && adcVal <= 295) return 2;    //284
    else if (adcVal >= 296 && adcVal <= 699) return 3;    //576 E-B, e switches between 306 and 576
    else if (adcVal >= 700 && adcVal <= 999) return 4;    //867
    else if (adcVal >= 1000 && adcVal <= 1300) return 5;  //1157, sometimes 1350 on low E
    else if (adcVal >= 1301 && adcVal <= 1635) return 6;  //1448, 1621, sometimes 1350 on low E (more often than on 5)
    else if (adcVal >= 1636 && adcVal <= 1900) return 7;  //1737, 1891
    else if (adcVal >= 1901 && adcVal <= 2100) return 8;  //2029, 2164 on low E
    else if (adcVal >= 2101 && adcVal <= 2399) return 9;  //2164 (more often than 8), 2322
    else if (adcVal >= 2400 && adcVal <= 2699) return 10; //2438, 2615
    else if (adcVal >= 2700 && adcVal <= 2924) return 11; //2819, 2909, 2715, 2985
    else if (adcVal >= 2925 && adcVal <= 3220) return 12; //3059, 3206
    else if (adcVal >= 3221 && adcVal <= 3510) return 13; //3342, 3501, 3262
    else if (adcVal >= 3511 && adcVal <= 3950) return 14; //3341, 3682, 3799, 3818
    else if (adcVal >= 3951) return 15;                   //4095
    else return 0;
}



int lookupNote (int str, int fret)
{
  int note = 0;
  switch (str)
  {
    case 0:
      note = fret + offset[0];  // E
      break;
    case 1: 
      note = fret + offset[1];  //A
      break;
    case 2:
      note = fret + offset[2];  // D
      break;
    case 3:
      note = fret + offset[3];  // G
      break;
    case 4:
      note = fret + offset[4];  // B
      break;
    case 5:
      note = fret + offset[5];  // e
      break;
    default:
      return 0;
  }
  return note + 36; //if octaves need adjusting, add or subtract 12 to this value per octave shift
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


//This function is for building lookup tables. Not needed for actual operation of guitar.
void findFretAvg(int str){
  int onOff = 0;
  int sum = 0;
  int current = 0;
  int i = 0;
  float avg = 0;
  int max = 0;
  int min = 99999;

  while (onOff < 1000)
  {
    onOff = analogRead(A7);
  }

  sum = 0;
  current = 0;
  i = 0;
  avg = 0;
  max = 0;
  min = 99999;    
  while (onOff > 1000)
  {
    i++;
    current = analogRead(str);
    sum += current;
    if (current>max) max = current;
    if (current<min) min = current;
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
