#define N_TUB 3

const int topPins[N_TUB] = {A0, A2, A4};
const int bottomPins[N_TUB] = {A1, A3, A5};

int inverted; // 0 == Upright; 1 == Upside-down

const int motorPins[4] = {8, 9, 10, 11};

// Array of the pins that should currently be monitored
int tubePins[N_TUB];

// Arrays for holding time readings
unsigned long topReadings[N_TUB];
unsigned long bottomReadings[N_TUB];

int stepsPerRev = 48 * 42.5; // reduction factor = 42.5

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
  }
}

void read_sensors() {
  for (int i = 0; i < N_TUB; i++) {
    int reading = analogRead(tubePins[i]);
    if (reading > 590) {
      if (tubePins[i] == topPins[i]) {
        if (topReadings[i] == 0) {
          topReadings[i] = millis();
          if (inverted == 0) {
            tubePins[i] = bottomPins[i];
          }
        }
      } else { // tubePins[i] == bottomPins[i]
        if (bottomReadings[i] == 0) {
          bottomReadings[i] == millis();
          if (inverted == 1) {
            tubePins[i] = topPins[i];
          }
        }
      }
    } // endif reading > threshold
  } // endfor
} // endfunc

// Functions for turning the motor while monitoring pins
void turn_motor_fwd(unsigned int deg) {
  int steps = (deg * stepsPerRev) / 360;
  for (int i = 0; i < steps; i++) {
    for (int j = 0; j < 4; j++) {
      if (i % 4 == j) { digitalWrite(motorPins[j], LOW); } else { digitalWrite(motorPins[j], HIGH); };
    };
    read_sensors();
    delay(6);
  };
}

void turn_motor_rev(unsigned int deg) {
  int steps = (deg * stepsPerRev) / 360;
  for (int i = 0; i < steps; i++) {
    for (int j = 0; j < 4; j++) {
      if (i % 4 == j) { digitalWrite(motorPins[3-j], LOW); } else { digitalWrite(motorPins[3-j], HIGH); };
    };
    read_sensors();
    delay(6);
  };
}

// Turn the rotor to a known position (a whole rev forward and a fixed rotation backward)
void initialize_rotor_position() {
  for (int i = 0; i < stepsPerRev; i++) {
    for (int j = 0; j < 4; j++) {
      if (i % 4 == j) { digitalWrite(motorPins[j], LOW); } else { digitalWrite(motorPins[j], HIGH); };
    };
    delay(7);
  };
  for (int i = 0; i < 300; i++) {
    for (int j = 0; j < 4; j++) {
      if (i % 4 == j) { digitalWrite(motorPins[3-j], LOW); } else { digitalWrite(motorPins[3-j], HIGH); };
    }
    delay(7);
  }
}

void setup() {
  initialize_rotor_position();
  set_tube_pins(bottomPins);

}

void loop() {
  // put your main code here, to run repeatedly:

}
