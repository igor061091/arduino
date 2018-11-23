/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/
#include <timer.h>

#define RELAY_OUT 4 
#define IMPULSE_COUNT 3

bool F_timer_reset_is_working=false;
const int digitalInPin = 2; // цифровой вход D2
uint8_t impulse_counter;
int dValue;
int buttonInt = 0;
auto timer = timer_create_default(); // create a timer with default settings
Timer<> default_timer; // save as above

bool timerHandler_ResetImpulse(void *) {
  Serial.println("timerHandler_ResetImpulse");
  impulse_counter=0;
  Serial.println(impulse_counter);
  F_timer_reset_is_working=false;
  return true; // repeat? true
}

bool timerHandler_RelayStop(void *) {
  Serial.println("timerHandler_RelayStop");
  digitalWrite(RELAY_OUT, LOW); 
  return true; // repeat? true
}
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(digitalInPin, INPUT);
  pinMode(RELAY_OUT, OUTPUT);
  //attachInterrupt(buttonInt, intHandler, FALLING );
  Serial.begin(115200);
  Serial.println("setup end");
}

void intHandler() {
 Serial.println("int push");
// timer.in(5000, timerHandler);
 delay(10000);
 Serial.println("int push delay"); 
 //digitalWrite(RELAY_OUT, HIGH);   // turn the LED on (HIGH is the voltage level) 
}

// the loop function runs over and over again forever
void loop() {
  timer.tick(); // tick the timer
  dValue = digitalRead(digitalInPin);
  if (dValue == LOW)
    {
      Serial.println("--------- HIGH SOUND ----------");
      if (!F_timer_reset_is_working) {timer.in(5000, timerHandler_ResetImpulse);F_timer_reset_is_working=true;}
      impulse_counter++;
      Serial.println("impulse_counter"); Serial.println(impulse_counter);
      if (impulse_counter>=IMPULSE_COUNT) {digitalWrite(RELAY_OUT, HIGH); timer.in(10000, timerHandler_RelayStop);}
      delay(500);
    }
  /*
  digitalWrite(RELAY_OUT, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(RELAY_OUT, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);     */                  // wait for a second
}
