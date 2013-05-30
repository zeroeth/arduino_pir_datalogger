/* ARDUINO MOTION LOGGER
   ---------------------
   This code has two modes, one for logging, and one for setting the clock
   To set the time:
     - uncomment `set_rtc_time_from_compile();`
     - upload and boot once until you see a steady flash on the green led
     - avoid resetting until you re-comment out the code as it will set the clock to the time you compiled.
*/

#include "SD.h"
#include <Wire.h>
#include "RTClib.h"
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

#define greenLEDpin 4

#define pirPin 3
#define interruptNumber 1 //for pin 3
#define chipSelect 10

RTC_DS1307 RTC;
File logfile;
char filename[] = "LOGGER00.CSV";
int last_pir_value = LOW;
int current_pir_value;
DateTime now;
bool should_sleep = false;
char logger_buffer[512] = {0};


void setup() {

  pin_io_modes();

  initialize_sd_and_open_log();

  initialize_rtc_clock();

  // ENABLE to set time once
  // set_rtc_time_from_compile();

  sensor_started_alert();

  setup_sleep();

  go_to_sleep();
}



/* SETUP ********************************/

void pin_io_modes() {
  pinMode(pirPin, INPUT);
  digitalWrite(pirPin, LOW);

  delay(1000);

  pinMode(greenLEDpin, OUTPUT);

  digitalWrite(greenLEDpin, LOW);

  pinMode(chipSelect, OUTPUT);
}


void initialize_sd_and_open_log() {
  SD.begin(chipSelect);

  logfile = SD.open(filename, FILE_WRITE);
  if(!logfile) {
    sdcard_error_alert();
  }
}


void initialize_rtc_clock() {
  Wire.begin();
  if (!RTC.begin()) {
    rtc_error_alert();
  }
}


void set_rtc_time_from_compile() {
  RTC.adjust(DateTime(__DATE__, __TIME__));

  /*
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for Leonardo only
    }

    Serial.println("RTC Sync:");

    Serial.println(__DATE__);
    Serial.println(__TIME__);
    Serial.println(__FILE__);

    Serial.println(DateTime(__DATE__, __TIME__).unixtime());

    delay(5000); // rtc warmup
    now = RTC.now();
    Serial.println(now.unixtime());
    Serial.println("---------");
  */

  time_set_alert();
}



/* MAIN LOOP ****************************/

void loop() {
  current_pir_value = digitalRead(pirPin);

  if(current_pir_value != last_pir_value) {

    now = RTC.now();

    if(current_pir_value == HIGH) {
      // digitalWrite(greenLEDpin, HIGH); // save power

      sprintf(logger_buffer, "HIGH %lu", (unsigned long)now.unixtime());
    }
    else {
      // digitalWrite(greenLEDpin, LOW); // save power

      sprintf(logger_buffer, "LOW %lu", (unsigned long)now.unixtime());

      should_sleep = true;
    }

    logfile = SD.open(filename, FILE_WRITE);
    logfile.println(logger_buffer);
    logfile.close();
  }

  last_pir_value = current_pir_value;

  if(should_sleep) {
    go_to_sleep();
  }
}




/* ALERTS *******************************/
// continuous slow with 2 quick blinks = SD card error
// continuous 1 second with 4 quick blinks = rtc error
// continuous 100 ms blink = time set mode
// 2 seconds of quick blink = ready to go

void time_set_alert() {
  digitalWrite (greenLEDpin, HIGH);

  delay(500);

  for(;;)
  {
    digitalWrite (greenLEDpin, LOW);

    delay(100);

    digitalWrite (greenLEDpin, HIGH);

    delay(100);
  }
}


void rtc_error_alert() {
  digitalWrite (greenLEDpin, HIGH);

  delay(500);

  for(;;)
  {
    digitalWrite (greenLEDpin, LOW);

    delay(100);

    digitalWrite (greenLEDpin, HIGH);

    delay(100);

    digitalWrite (greenLEDpin, LOW);

    delay(100);

    digitalWrite (greenLEDpin, HIGH);

    delay(100);

    digitalWrite (greenLEDpin, LOW);

    delay(100);

    digitalWrite (greenLEDpin, HIGH);

    delay(100);

    digitalWrite (greenLEDpin, LOW);

    delay(100);

    digitalWrite (greenLEDpin, HIGH);

    delay(100);

    digitalWrite (greenLEDpin, LOW);

    delay(1000);

    digitalWrite (greenLEDpin, HIGH);

    delay(1000);
  }
}


void sdcard_error_alert() {
  digitalWrite (greenLEDpin, HIGH);

  delay(500);

  for(;;)
  {
    digitalWrite (greenLEDpin, LOW);

    delay(50);

    digitalWrite (greenLEDpin, HIGH);

    delay(50);

    digitalWrite (greenLEDpin, LOW);

    delay(2000);

    digitalWrite (greenLEDpin, HIGH);

    delay(2000);
  }
}


void sensor_started_alert() {
  for (int alert_count = 0; alert_count < 10; alert_count++)
  {
    digitalWrite (greenLEDpin, HIGH);

    delay(50);

    digitalWrite (greenLEDpin, LOW);

    delay(50);
  }

}



/* INTERRUPT ROUTINE FOR PIR SENSOR *****/

void setup_sleep() {
}

void go_to_sleep() {
  // FIXME this doesnt kick in till first log
  should_sleep = false;

  attachInterrupt(interruptNumber, pinInterrupt, HIGH);
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();

  sleep_mode();
  sleep_disable();
}


void pinInterrupt(void) {
  detachInterrupt(interruptNumber);
}
