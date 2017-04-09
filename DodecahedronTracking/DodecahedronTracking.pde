import processing.serial.*;
import java.text.NumberFormat;
import java.util.Locale;

Serial myPort;
PImage bg;

String serialPort = "/tmp/cu.Lightblue-Bean";

String descriptors[] = {"Arbeiten", "Daily Scrum", "Sprint Planning", "Spring Review", "Sprint Retrospektive", "PB Refinement", "Kaffee", "Toilette", "Mittagspause", "Anderes"};
int descriptorSize;

final float CENT_PER_SECOND = 2.48333;

String status;

void setup() {
  
  size(720, 400);
  frameRate(60);
  bg = loadImage("background_timer.png");
  myPort = new Serial(this, "/tmp/cu.Lightblue-Bean", 57600);
 
 background(bg); 
  
      textSize (12);
      fill(0);
      
      for(int i = 0; i < 10; i++) {
        text (i + 1, 138, 97 + i * 27.2);
      }  
      
      descriptorSize = descriptors.length;
      for(int i = 0; i < descriptorSize; i++) {
        String descriptor = descriptors[i];
        text (descriptor, 191, 97 + i * 27.2);
      }  
      
      for(int i = 0; i < 10; i++) {
        text (getParsedTime(0), 355, 97 + i * 27.2);
        text (getParsedMoney(0), 520, 97 + i * 27.2);
      }
}

void draw() { 
 
  while (myPort.available() > 0) {
    
    String value = myPort.readString();  
    
    if (!value.equals("") && value.length() > 20) {
      String[] sides = value.split(",");
      if (sides.length > 11) {
        return;
      }
      background(bg);
      
      String sideString = sides[10].trim();
      int activeSideId = Integer.parseInt(sideString);
      
      textSize(12);
      for(int i = 0; i < 10; i++) {
        fill(0);
        if (activeSideId == i) {
          fill(0, 128, 255);
        }
        text (i + 1, 138, 97 + i * 27.2);
      }  
      
      textSize(12);
      for(int i = 0; i < 10; i++) {
        fill(0);
        String descriptor = descriptors[i];
        if (activeSideId == i) {
          fill(0, 128, 255);
        }
        text (descriptor, 191, 97 + i * 27.2);
      }  
      
      for(int i = 0; i < 10; i++) {
        fill(0);
        String time = sides[i].trim();
        if (activeSideId == i) {
          fill(0, 128, 255);
        }
        int totalSeconds = Integer.parseInt(time);
        String timeFormat = getParsedTime(totalSeconds);
        text (timeFormat, 355, 97 + i * 27.2);
        String moneyFormat = getParsedMoney(totalSeconds);
        text (moneyFormat, 520, 97 + i * 27.2);
      }
      delay(300);
    }
  } 
}

String getParsedTime(int totalSecs) {
  int hours = totalSecs / 3600;
  int minutes = (totalSecs % 3600) / 60;
  int seconds = totalSecs % 60;

  return String.format("%02d:%02d:%02d", hours, minutes, seconds);
}

String getParsedMoney(int totalSecs) {
  float totalCents = totalSecs * CENT_PER_SECOND;
  NumberFormat nf = NumberFormat.getCurrencyInstance(Locale.GERMANY);
  
  return String.format(nf.format(totalCents/100.0));
}
  