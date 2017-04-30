#define N_TUB 1

const int topPins[N_TUB] = {A1};//, A2};
const int bottomPins[N_TUB] = {A0};//, A3};

int inverted; // 0 == Upright; 1 == Upside-down
unsigned long turnStartTime;

const int motorPins[4] = {8, 9, 10, 11};

// Array of the pins that should currently be monitored
int tubePins[N_TUB];

// Arrays for holding time readings
unsigned long topReadings[N_TUB];
unsigned long bottomReadings[N_TUB];
int tubesDone[N_TUB];

unsigned long stepsPerRev = 48 * 42.5; // reduction factor = 42.5

// Run parameters
int waitTime = 90; // Maximum waiting time (seconds) for a tube
int numTurns = 10; // Number of times to turn the rotor

// Change all current pins to either bottom or top pins
void set_tube_pins(int pins[N_TUB]) {
  for (int i = 0; i < N_TUB; i++) {
    tubePins[i] = pins[i];
  }
}

// Set all readings to 0
void reset_readings() {
  for (int i = 0; i < N_TUB; i++) {
    topReadings[i] = 0;
    bottomReadings[i] = 0;
    tubesDone[i] = false;
  }
}

int reading_done() {
  int done = true;
  for (int i = 0; i < N_TUB; i++) {
    if (tubesDone[i] == false) {
      done = false;
      break;
    }
  }
  return done;
}

void read_sensors() {
  for (int i = 0; i < N_TUB; i++) {
    int reading = analogRead(tubePins[i]);
    if (reading > 570) {
      /*Serial.print("DEB Detected: ");
      Serial.print(i);
      Serial.print(" ");
      Serial.print(reading);
      Serial.print(" ");
      Serial.println(tubePins[i]);*/
      if (tubePins[i] == topPins[i]) {
        if (topReadings[i] == 0) {
          topReadings[i] = millis();
          if (inverted == 0) {
            tubePins[i] = bottomPins[i];
          } else {
            tubesDone[i] = true;
          }
        }
      } else { // tubePins[i] == bottomPins[i]
        if (bottomReadings[i] == 0) {
          bottomReadings[i] = millis();
          if (inverted == 1) {
            tubePins[i] = topPins[i];
          } else {
            tubesDone[i] = true;
          }
        }
      }
    } // endif reading > threshold
  } // endfor tubes
} // endfunc

// Functions for turning the motor while monitoring pins
void turn_rotor_fwd(unsigned int deg) {
  int steps = (deg * stepsPerRev) / 360;
  for (int i = 0; i < steps; i++) {
    for (int j = 0; j < 4; j++) {
      if (i % 4 == j) { digitalWrite(motorPins[j], LOW); } else { digitalWrite(motorPins[j], HIGH); };
    };
    read_sensors();
    delay(7);
  };
}

void turn_rotor_rev(unsigned int deg) {
  unsigned long tmp = (deg * stepsPerRev);
  unsigned int steps = tmp / 360;
  //Serial.print("DEB ");
  //Serial.print(deg);
  //Serial.print(" ");
  //Serial.print(stepsPerRev);
  //Serial.print(" ");
  //Serial.print(tmp);
  //Serial.print(" ");
  //Serial.print(steps);
  //Serial.println(" ");
  for (int i = 0; i < steps; i++) {
    for (int j = 0; j < 4; j++) {
      if (i % 4 == j) { digitalWrite(motorPins[3-j], LOW); } else { digitalWrite(motorPins[3-j], HIGH); };
    };
    read_sensors();
    delay(7);
  };
}

// Turn the rotor to a known position (a whole rev forward and a fixed rotation backward)
void initialize_rotor_position() {
  for (int i = 0; i < stepsPerRev / 2; i++) {
    for (int j = 0; j < 4; j++) {
      if (i % 4 == j) { digitalWrite(motorPins[j], LOW); } else { digitalWrite(motorPins[j], HIGH); };
    };
    delay(7);
  };
  for (int i = 0; i < 550; i++) {
    for (int j = 0; j < 4; j++) {
      if (i % 4 == j) { digitalWrite(motorPins[3-j], LOW); } else { digitalWrite(motorPins[3-j], HIGH); };
    }
    delay(7);
  }
}

void output_results() {
  Serial.print("DEB ");
  Serial.print(topReadings[0]);
  Serial.print(" ");
  Serial.println(bottomReadings[0]);
  Serial.print("RES ");
  for (int i = 0; i < N_TUB; i++) {
    float res;
    if (topReadings[i] != 0 and bottomReadings[i] != 0) {
      if (topReadings[i] > bottomReadings[i]) {
        res = (topReadings[i] - bottomReadings[i]) / 1000.;
      } else {
        res = (bottomReadings[i] - topReadings[i]) / 1000.;
      }
    } else {
      res = 0.0;
    }
    Serial.print(res);
    Serial.print(" ");
  }
  Serial.println("");
}

void debug_write_readings() {
  Serial.print("DEB ");
  for (int i = 0; i < N_TUB; i++) {
    Serial.print(analogRead(topPins[i]));
    Serial.print(" ");
  }
  Serial.println("");
  Serial.print("DEB ");
  for (int i = 0; i < N_TUB; i++) {
    Serial.print(analogRead(bottomPins[i]));
    Serial.print(" ");
  }
  Serial.println("");
}

void setup() {
  
  Serial.begin(9600);
  
  for (int i = 0; i < N_TUB; i++) {
    pinMode(topPins[i], INPUT_PULLUP);
    pinMode(bottomPins[i], INPUT_PULLUP);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(motorPins[i], OUTPUT);
  }
  initialize_rotor_position();
  set_tube_pins(topPins);
  inverted = 1;
  debug_write_readings();
  delay(2000);
}

void loop() {
  reset_readings();
  turnStartTime = millis();
  if (inverted == 1) {
    Serial.println("DEB Turning rev");
    inverted = 0;
    turn_rotor_rev(140);
    Serial.println("DEB Turning rev done");
  } else {
    Serial.println("DEB Turning fwd");
    inverted = 1;
    turn_rotor_fwd(140);
    Serial.println("DEB Turning fwd done");
  }
  while (reading_done() == false and (millis() - turnStartTime) / 1000 < waitTime) {
    read_sensors();
  }
  output_results();
  delay(5000);
}
