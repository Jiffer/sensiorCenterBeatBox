class Light {
    // object varibles
    byte index;

    bool armed = false;

    unsigned long lastAvg = 0;
    int avgSampleRate = 10;

    int threshold = 100;
    
    int history[40];
    int hPtr = 0;
    float movingAverage;

    // Mux stuff
    //Mux control pins
    int s0 = 10;
    int s1 = 11;
    int s2 = 12;
    int s3 = 13;
    //Mux in "SIG" pin
    int SIG_pin = A0;


  public:
    int value;
    // constructor
    Light(byte num) {
      index = num;

    }

    void init() {
      // Mux setup
      pinMode(s0, OUTPUT);
      pinMode(s1, OUTPUT);
      pinMode(s2, OUTPUT);
      pinMode(s3, OUTPUT);

      // fill average with high values
      int arraySize = sizeof(history) / sizeof(int);
      for(int i = 0; i < arraySize; i++){
        history[i] = 1023;
      }
    }
    

    void checkAndArm() {
      value = readMux(index);
      if (millis() > avgSampleRate + lastAvg) {
        lastAvg = millis();
        history[hPtr++] = value;
        int arraySize = sizeof(history) / sizeof(int);
        hPtr %= arraySize;
        int accum = 0;
        for (int i = 0; i < arraySize; i++) {
          accum += history[i];
        }
        movingAverage = accum / arraySize;
      }
      if (!armed) { // then check if it should become armed
        if (value > movingAverage + threshold && millis() > 5000) {
          armed = true;
        }
        else {
          return;
        }
      }
    }

    bool isArmed() {
      if (armed) {
        return true;
      }
      else return false;
    }

    void disarm() {
      armed = false;
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

};
