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
int current_pir_value;
DateTime now;

void setup() {

  pinMode(pirPin, INPUT);
  digitalWrite(pirPin, LOW);

  delay(1000);

  pinMode(greenLEDpin, OUTPUT);
  pinMode(  redLEDpin, OUTPUT);

  digitalWrite(greenLEDpin, LOW);
  digitalWrite(  redLEDpin, LOW);

  pinMode(chipSelect, OUTPUT);

  SD.begin(chipSelect);

  logfile = SD.open(filename, FILE_WRITE);
  if(!logfile) {
    error_alert();
  }

  Wire.begin();
  if (!RTC.begin()) {
    error_alert();
    logfile.println("RTC failed");
  }

  //RTC.adjust(DateTime(__DATE__, __TIME__));

  logfile.close();

  go_to_sleep();
}

void loop() {
  current_pir_value = digitalRead(pirPin);

  if(current_pir_value != last_pir_value) {

    now = RTC.now();

    logfile = SD.open(filename, FILE_WRITE);

    if(current_pir_value == HIGH) {
      digitalWrite(greenLEDpin, HIGH);

      logfile.print("HIGH ");
    }
    else {
      digitalWrite(greenLEDpin, LOW);

      logfile.print("LOW ");
    }

    logfile.print(now.unixtime());
    logfile.println("");
    logfile.close();
  }

  last_pir_value = current_pir_value;
}

void error_alert() {
  digitalWrite(redLEDpin, HIGH);
}

void go_to_sleep() {
}
