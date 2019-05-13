
#include "Light.h"
#define NUMLIGHTS 8
Light * myLights[NUMLIGHTS];

// Global Variables //
float light[8];
int solPin[8] = {9, 7, 8, 2, 3, 4, 5, 6};
boolean solOn[8] = {false, false, false, false, false, false, false, false};
unsigned long lastHit[8];
unsigned long lastBeat = 0;

unsigned long decayTime = 0;
int decayInterval = 10;

boolean play = true;
int tempo = 120;
int onTimeBase[8] = {1, 1, 1, 3, 3, 4, 4, 2}; // 0 is far right // 5 is middle,
float onTime[8] = {0, 0, 0, 0, 0, 0, 0, 0};

boolean delayBeat = false;




void setup() {
  for (int i = 0; i < NUMLIGHTS; i++) {
    myLights[i] = new Light(i);
    myLights[i]->init();
  }

  

  // Solenoids
  for (int i = 0; i < sizeof(solPin) / sizeof(int); i++) {
    pinMode(solPin[i], OUTPUT);
  }

  // debug
  Serial.begin(9600);
}


// Main loop

void loop() {
  
  if(millis() > decayTime + decayInterval){
    for(int i = 0; i < NUMLIGHTS; i++){
      light[i] *= .99;
      
    }
  }
  for(int i = 0; i < 8; i++){
    myLights[i]->checkAndArm();
  }
  chanceDance();

  // check if it's time to turn any off
  checkOnTime();

}

/// Algorithmic groovy gravy
// pin A? controls tempo
// pin A? controls swing feel
void chanceDance() {
  tempo = map(analogRead(A1), 0, 1023, 250, 90);
  if (delayBeat) {
    tempo = tempo + analogRead(A2) / 3000.0 * tempo;
  }
  else {
    tempo = tempo - analogRead(A2) / 3000.0 * tempo;
  }


  if ((millis() - lastBeat) > tempo) {
    lastBeat = millis();

    // Check Lights:
    for (int i = 0; i < 8; i++) {
            Serial.print(" L");
            Serial.print(i);
            Serial.print(" ");
            Serial.println(light[i]);
      if (myLights[i]->isArmed() == true) {
        light[i] = 100;
      }

    }

    int numTappers = 0;
    int numStrikers = 0;
    delayBeat = !delayBeat;

    for (int i = 0; i < 8; i++) {
      
      if (random(20, 100) < light[i]) { // based on the current light reading
        myLights[i]->disarm();
        Serial.print("playing: "); Serial.println(i);
        digitalWrite(solPin[i], HIGH); // set it high
        solOn[i] = true; // keep track of what is currently triggered
        lastHit[i] = millis(); // when it fired
      }
      if (i > 2) {
        numStrikers++;
      }
      else {
        numTappers++;
      }
      // TODO: only allow _ polyphony...
//      if(numStrikers > 2){
//        bool onArray[numStrikers];
//        for(int i = 0; i < numStrikers; i++){
//          for(int j = 3; j < NUMLIGHTS; j++){
//            if(solOn[j] == true){
//              onArray[i] = j;
//            }
//          }
//        }
//        
//      }
    }
    for (int i = 0; i < 8; i++) {
      if (i > 2 ) {
        onTime[i] = onTimeBase[i] + random(2)  ;
      }
      else {
        onTime[i] = onTimeBase[i]  + (int)random(3);
      }
    }
  }
}


/////////////////////////////////////////
// checkOnTime()
// check if its time to turn off
/////////////////////////////////////////
void checkOnTime() {
  for (int i = 0; i < 8; i++) {
    // if its been "onTime" milliseconds since a solenoid was fired, turn it off
    if (solOn[i] == true && (millis() - lastHit[i]) > onTime[i])
    {
      solOn[i] = false;
      digitalWrite(solPin[i], LOW);
    }
  }
}
