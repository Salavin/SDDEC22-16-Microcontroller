int adcReadAvg(int port, int nAvg);
int lookupFret(int adcVal);
int lookupNote(int str, int fret);
int findHighestNote(int notes[6]);

int strE = A0;
int strA = A1;
int strD = A2;
int strG = A3;
int strB = A4;
int strEe = A5; 
                    
int adcVal = 0;  
int fret = 0;

void setup() {
  Serial.begin(9600);       
  analogReadResolution(12); //12-bit mode
}

void loop() {
  int strings[6] = {};  //E A D G B e
  int frets[6] = {};
  int notes[6] = {};    //MIDI notes associated with each string's current fret value
  int nextNote = 0;
  int nAvg = 1; //number of ADC reads to take for average
  strings[0] = adcReadAvg(strE, nAvg);
  strings[1] = adcReadAvg(strA, nAvg);
  strings[2] = adcReadAvg(strD, nAvg);
  strings[3] = adcReadAvg(strG, nAvg);
  strings[4] = adcReadAvg(strB, nAvg);
  strings[5] = adcReadAvg(strEe, nAvg);

  Serial.print(strings[5]);
  Serial.print("\t");

  for(int i=0; i<6; i++){ //convert ADC value to MIDI note for each string
    frets[i] = lookupFret(strings[i]);
    notes[i] = lookupNote(i, frets[i]);
  }

  for(int i=0; i<6;i++){
    Serial.print(frets[i]);
    Serial.print("\t");
  }
  //Serial.print(frets[5]);
  Serial.print("\n");

  //nextNote = findHighestNote(notes[]);


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
  //else if(adcVal)
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

int findHighestNote(int notes[6]){
  int highestNote = 0;
  for(int i=0; i<6; i++){
    if(highestNote<notes[i]) highestNote = notes[i];    
  }
  return highestNote;
}