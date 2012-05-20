#include "SD.h"
#include <Wire.h>
#include "RTClib.h"

#define redLEDpin 5
#define greenLEDpin 4

#define pirPin 3
#define chipSelect 10

RTC_DS1307 RTC;
File logfile;
char filename[] = "LOGGER00.CSV";
int last_pir_value = LOW;

void setup() {
  Serial.begin(9600);

  pinMode(pirPin, INPUT);
  digitalWrite(pirPin, LOW);

  delay(1000);

  pinMode(greenLEDpin, OUTPUT);
  //pinMode(redLEDpin, OUTPUT);

  pinMode(chipSelect, OUTPUT);

  SD.begin(chipSelect);

  logfile = SD.open(filename, FILE_WRITE);
  if (!logfile) {
    Serial.print("LOG failed");
  }

  Wire.begin();
  if (!RTC.begin()) {
    logfile.println("RTC failed");
    Serial.print("RTC failed");
  }

  //RTC.adjust(DateTime(__DATE__, __TIME__));


  logfile.close();
}

void loop() {

  DateTime now;
  now = RTC.now();

  int current_pir_value = digitalRead(pirPin);

  if(digitalRead(pirPin) == HIGH) {
    digitalWrite(greenLEDpin, HIGH);
  }
  else {
    digitalWrite(greenLEDpin, LOW);
  }

  if(current_pir_value != last_pir_value) {

    if(digitalRead(pirPin) == HIGH) {
      Serial.println("HIGH");

      logfile = SD.open(filename, FILE_WRITE);

      logfile.print("HIGH ");
      logfile.print(now.unixtime());
      logfile.println("");

      logfile.close();
    }
    else {
      Serial.print("LOW");

      logfile = SD.open(filename, FILE_WRITE);

      logfile.print("LOW ");
      logfile.print(now.unixtime());
      logfile.println("");

      logfile.close();
    }
  }
  last_pir_value = current_pir_value;
}
