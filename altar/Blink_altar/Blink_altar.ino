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
#define RELAY_OUT_LOCK 6 
#define RELAY_OUT_LIGHT 7
#define VCC_SENSOR_OUT 8

//#define LIGHT_ON  digitalWrite(RELAY_OUT_LIGHT, LOW)
//#define LIGHT_OFF digitalWrite(RELAY_OUT_LIGHT, HIGH)

const int DigitalSensorPinMoon = 9;
bool DigitalSensorStateMoon = 0;
bool DigitalSensorSwitchedMoon = 0;

const int DigitalSensorPinSun = 5;
bool DigitalSensorStateSun = 0;
bool DigitalSensorSwitchedSun = 0;

uint8_t MoonBuffer;
uint8_t SunBuffer;
uint8_t SimCounter;

const uint8_t AnswerMoon=0b10000001;
const uint8_t AnswerSun=~AnswerMoon;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(RELAY_OUT_LOCK, OUTPUT); digitalWrite(RELAY_OUT_LOCK, HIGH);
  pinMode(RELAY_OUT_LIGHT, OUTPUT); digitalWrite(RELAY_OUT_LIGHT, HIGH);
  pinMode(VCC_SENSOR_OUT, OUTPUT); digitalWrite(VCC_SENSOR_OUT, HIGH);      // питание на сенсор
  pinMode(DigitalSensorPinMoon, INPUT);
  Serial.begin(115200); 
  Serial.print("AnswerMoon:"); Serial.println(AnswerMoon,BIN); 
  Serial.print("AnswerSun:");  Serial.println(AnswerSun,BIN); 
  Serial.println("setup end"); 
}

#define LIGHT_ON  digitalWrite(RELAY_OUT_LIGHT, LOW)
#define LIGHT_OFF digitalWrite(RELAY_OUT_LIGHT, HIGH)

#define LOCK_ON  digitalWrite(RELAY_OUT_LOCK, LOW)
#define LOCK_OFF digitalWrite(RELAY_OUT_LOCK, HIGH)

// the loop function runs over and over again forever
void loop() {
  /* Обработка сенсора Moon */
  DigitalSensorStateMoon=digitalRead(DigitalSensorPinMoon);
  if (DigitalSensorStateMoon== HIGH && !DigitalSensorSwitchedMoon){
     Serial.print("Moon button switch:"); 
     DigitalSensorSwitchedMoon= true;
     MoonBuffer|=1<<SimCounter++; 
     Serial.println(MoonBuffer, BIN);
     Serial.println(SimCounter, DEC); 
     delay(2000);  
  } else DigitalSensorSwitchedMoon= false;
 /* ------------------------------------------ */
 /* Обработка сенсора Sun */
 DigitalSensorStateSun=digitalRead(DigitalSensorPinSun);
  if (DigitalSensorStateSun== HIGH && !DigitalSensorSwitchedSun){
     Serial.print("Sun button switch:"); 
     DigitalSensorSwitchedSun= true; 
     SunBuffer|=1<<SimCounter++; 
     Serial.println(SunBuffer, BIN);
     Serial.println(SimCounter, DEC); 
     delay(2000);  
  } else DigitalSensorSwitchedSun= false;
 /* ------------------------------------------ */

 /* Вывод результата  */
 if (SimCounter>=8)
 {
   if ((SunBuffer == AnswerSun) && (MoonBuffer == AnswerMoon)) 
   {
      LOCK_ON;
      Serial.println("Answer right"); 
      SimCounter=0;
      MoonBuffer=0; SunBuffer=0;
   }
   else
   {
     LOCK_OFF;
     Serial.println("Answer failed"); 
     SimCounter=0;
     MoonBuffer=0; SunBuffer=0;
   }
 }

 /*------------------------------------*/

 
/*
  LIGHT_ON;   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  LIGHT_OFF;    // turn the LED off by making the voltage LOW
  delay(1000);
 */
  /*
  digitalWrite(RELAY_OUT_LOCK, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(RELAY_OUT_LOCK, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);  */
  // wait for a second
}
