/*
TIMER
*/

/*s
 * init EEPROM 
 */
 #include <EEPROM.h>

/* vibramotor pins */
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
  
  Serial.begin(57600);
  accel = Bean.getAcceleration();

  timeSinceStartMS = millis();
  timeSinceStillMS = millis();
  currentMillis = millis();

  //set initial accel values
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

// the loop routine w/ a 100ms sleep
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

  // add offset to currentMillis because of sleep for one second
  currentMillis += 1000;
  Bean.sleep(1000);
}


int getSide(int x, int y, int z) {
  if (inRange(x, 186) && inRange(y, -140) && inRange(z, -120)) {
    return 0;
  }
  else if(inRange(x, -75) && inRange(y, -144) && inRange(z, -198)) {
    return 1;
  }
  else if(inRange(x, -220) && inRange(y, -120) && inRange(z, 20)) {
    return 2;
  }
  else if(inRange(x, -75) && inRange(y, -107) && inRange(z, 234)) {
    return 3;
  }
  else if(inRange(x, 183) && inRange(y, -123) && inRange(z, 150)) {
    return 4;
  }
  else if(inRange(x, 235) && inRange(y, 90) && inRange(z, -10)) {
    return 5;
  }
  else if(inRange(x, 75) && inRange(y, 75) && inRange(z, -220)) {
    return 6;
  }
  else if(inRange(x, -176) && inRange(y, 90) && inRange(z, -139)) {
    return 7;
  }
  else if(inRange(x, -180) && inRange(y, 119) && inRange(z, 130)) {
    return 8;
  }
  else if(inRange(x, 78) && inRange(y, 114) && inRange(z, 215)) {
    return 9;
  }
  // Head
  else if(inRange(x, -5) && inRange(y, -265) && inRange(z, 38)) {
    return -99;
  }
  // Bottom
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

    //if  moving, grab the new values to compare
    old_x = x;
    old_y = y;
    old_z = z;
    
    return false;
  }

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

