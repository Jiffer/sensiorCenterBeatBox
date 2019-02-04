// Global Variables //
float light[8];
int solPin[8] = {9, 7, 8, 2, 3, 4, 5, 6};
boolean solOn[8] = {false, false, false, false, false, false, false, false};
unsigned long lastHit[8];
unsigned long lastBeat = 0;

boolean play = true;
int tempo = 120;
int onTimeBase[8] = {5, 5, 5, 5, 6, 6, 6, 4}; // 0 is far right // 5 is middle,
float onTime[8] = {0, 0, 0, 0, 0, 0, 0, 0};

boolean delayBeat = false;

//Mux control pins
int s0 = 10;
int s1 = 11;
int s2 = 12;
int s3 = 13;
//Mux in "SIG" pin
int SIG_pin = A0;


void setup() {

  // Mux setup
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  // Solenoids
  for (int i = 0; i < sizeof(solPin) / sizeof(int); i++) {
    pinMode(solPin[i], OUTPUT);
  }

  // debug
  Serial.begin(9600);
}


// Main loop

void loop() {


  if (play) {
    chanceDance();

    // check if it's time to turn any off
    checkOnTime();
  }
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
      if (i == 4) {
        light[i] =  (int)constrain(map(readMux(i), 500, 950, -20, 100), 0, 100);
      }else{
        light[i] =  (int)constrain(map(readMux(i), 400, 850, -20, 100), 0, 100);
      }
//      Serial.print("Mux: ");
//      Serial.print(readMux(i));
//      Serial.print(" L");
//      Serial.print(i);
//      Serial.print(" ");
//      Serial.println(light[i]);
      
    }


    int numTappers = 0;
    int numStrikers = 0;
    delayBeat = !delayBeat;
    for (int i = 0; i < 8; i++) {
      if (random(100) < light[i]) { // based on the current light reading
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
    }
    for (int i = 0; i < 8; i++) {
      if (i >2 ) {
        onTime[i] = onTimeBase[i]  ;
      }
      else {
        onTime[i] = onTimeBase[i] + numTappers + (int)random(6);
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


/////////////////////////////////////////
// MUX function
/////////////////////////////////////////
int readMux(int channel) {
  int controlPin[] = {s0, s1, s2, s3};

  int muxChannel[16][4] = {
    {0, 0, 0, 0}, //channel 0
    {1, 0, 0, 0}, //channel 1
    {0, 1, 0, 0}, //channel 2
    {1, 1, 0, 0}, //channel 3
    {0, 0, 1, 0}, //channel 4
    {1, 0, 1, 0}, //channel 5
    {0, 1, 1, 0}, //channel 6
    {1, 1, 1, 0}, //channel 7
    {0, 0, 0, 1}, //channel 8
    {1, 0, 0, 1}, //channel 9
    {0, 1, 0, 1}, //channel 10
    {1, 1, 0, 1}, //channel 11
    {0, 0, 1, 1}, //channel 12
    {1, 0, 1, 1}, //channel 13
    {0, 1, 1, 1}, //channel 14
    {1, 1, 1, 1} //channel 15
  };

  //loop through the 4 sig
  for (int i = 0; i < 4; i ++) {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  //read the value at the SIG pin
  int val = analogRead(SIG_pin);

  //return the value
  return val;
}
