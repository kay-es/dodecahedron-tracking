/*
TIMER
* Controls the Bean board
* sends signal if accelerometer is still for 2 secs or is moving
*/

/*
 * init EEPROM 
 */
 #include <EEPROM.h>

// vibramotor pins
int ledPin1 = 0;
int ledPin2 = 1;

/* TIMERS */
unsigned long timeSinceStartMS = 0;
unsigned long timeSinceStillMS = 0;
unsigned long timeSinceSideStart = 0;
unsigned long currentMillis = 0;

/*
 * ORIENTATION INFORMATIONS & TRANSIENT STORAGE
 */
int currentSide;
int oldSide;
unsigned int times[10];
int resetCounter = 0;

String csvString;
boolean dataAvailable = false;
 
// initial accel values
int xRotation = 0;
int yRotation = 0;
int zRotation = 0;

// change in accel
int old_x = 0;
int old_y = 0;
int old_z = 0;

//set up our AccelerationReading object
AccelerationReading accel;

void setup() {
  // Optional: Use Bean.setAccelerationRange() to set the sensitivity
  // to something other than the default of ±2g.
  //Connection speed must be same as app.
  Serial.begin(57600);
  accel = Bean.getAcceleration();

  timeSinceStartMS = millis();
  timeSinceStillMS = millis();
  currentMillis = millis();

  //set our initial accel values
  xRotation = (accel.xAxis);
  yRotation = (accel.yAxis),
  zRotation = (accel.zAxis);
  old_x = xRotation;
  old_y = yRotation;
  old_z = zRotation;

  currentSide = getSide(xRotation, yRotation, zRotation); 
  
  if (EEPROM.read(0) != 0xff) {
    for (int i = 0; i < 10; i++) {
        times[i] = EEPROM.read(i);
    }
  }

  parseTimesToCSV(times);
  
  pinMode(ledPin1, OUTPUT);      // sets the digital pin as output
  pinMode(ledPin2, OUTPUT); 
}

// the loop routine runs over and over again forever w/ a 100ms sleep
void loop() {

  accel = Bean.getAcceleration();

  xRotation = (accel.xAxis);
  yRotation = (accel.yAxis),
  zRotation = (accel.zAxis);

  //update the time since we started the program
  //(Resets after 50 days according to arduino docs)
  timeSinceStartMS = millis();
      
  oldSide = currentSide;
  currentSide = getSide(xRotation, yRotation, zRotation);

  if (currentSide != -1) { 
    if (oldSide != currentSide) {
      vibrate(250);
      timeSinceSideStart = currentMillis - (times[currentSide] * 1000);
    }
    else if (currentSide == -99 && dataAvailable) {
      resetCounter++;
      vibrate(250);
      if (resetCounter >= 2) {
        resetCounter = 0;
        dataAvailable = false;
        memset(times, 0, sizeof(times));
        parseTimesToCSV(times);
        Serial.println(getTimesCSVWithCurrentSide());
        for (int i = 0; i < 10; i++) {
          EEPROM.update(i, 0);
        }
      }
    }
    else if (currentSide != -99) {
      times[currentSide] = (currentMillis - timeSinceSideStart) / 1000;
      dataAvailable = true;
      parseTimesToCSV(times);
      Serial.println(getTimesCSVWithCurrentSide());
      for (int i = 0; i < 10; i++) {
        EEPROM.write (i, times[i]);
      }
    }
  }
  else {
    Serial.println(getTimesCSVWithCurrentSide());
  }

  //1 oder 1000? :o
  currentMillis += 1000;
  Bean.sleep(1000);
}


int getSide(int x, int y, int z) {
  if (inRange(x, -215) && inRange(y, 100) && inRange(z, -65)) {
    return 0;
  }
  else if(inRange(x, -107) && inRange(y, 145) && inRange(z, 167)) {
    return 1;
  }
  else if(inRange(x, 157) && inRange(y, 133) && inRange(z, 146)) {
    return 2;
  }
  else if(inRange(x, 210) && inRange(y, 63) && inRange(z, -108)) {
    return 3;
  }
  else if(inRange(x, -15) && inRange(y, 40) && inRange(z, -240)) {
    return 4;
  }
  else if(inRange(x, 113) && inRange(y, -180) && inRange(z, -150)) {
    return 5;
  }
  else if(inRange(x, -150) && inRange(y, -160) && inRange(z, -133)) {
    return 6;
  }
  else if(inRange(x, -211) && inRange(y, -93) && inRange(z, 120)) {
    return 7;
  }
  else if(inRange(x, 17) && inRange(y, -72) && inRange(z, 253)) {
    return 8;
  }
  else if(inRange(x, 218) && inRange(y, -128) && inRange(z, 80)) {
    return 9;
  }
  else if(inRange(x, -15) && inRange(y, -267) && inRange(z, 52)) {
    return -99;
  }
  return -1;
  
}

boolean inRange(int coord, int value) {
  if ((value - 40) <= coord && (value + 40) >= coord) {
    return true;
  }
  else {
    return false;
  }
}

//checks the state of the cube's accelerometer
boolean isStill(int x, int y, int z) {

  //find the change in each orientation value
  int delta_x = x - old_x;
  int delta_y = y - old_y;
  int delta_z = z - old_z;

  //check for movement with a 40 threshold
  if(delta_x > 40 || delta_x < -40 || delta_y > 40 || delta_y < -40 || delta_z > 40 || delta_z < -40) {

    //if we are moving, grab the new values to compare
    old_x = x;
    old_y = y;
    old_z = z;

    return false;
  }

  //if change in each variable isn't above the threshold we aren't moving
  return true;
}

void parseTimesToCSV(unsigned int times[]) {
  csvString = String();
  for (int i = 0; i < 10; i++) {
    csvString.concat(times[i]);
    if (i < 9) {
      csvString.concat(",");
    }
  }
}

String getTimesCSVWithCurrentSide() {
  String appendecCSVString = csvString;
  appendecCSVString.concat(",");
  appendecCSVString.concat(currentSide);
  return appendecCSVString;
}

void vibrate(int ms) {
  digitalWrite(ledPin1, HIGH);   // sets the VIBRATION on
  digitalWrite(ledPin2, HIGH);   // sets the VIBRATION on
  delay(ms);                  // waits for a minor second
  digitalWrite(ledPin1, LOW);    // sets the VIBRATION off
  digitalWrite(ledPin2, LOW);    // sets the VIBRATION off
}

