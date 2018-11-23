
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <timer.h>
#include <MqttShell.h>
#include <Interfaces.h>
#include <InterfacesShell.h>

#include <ArduinoIO.h>
#include <IO_boards.h>

#define RELAY_OUT_LOCK 6 
#define RELAY_OUT_LIGHT 7
#define VCC_SENSOR_OUT 5

#define TIMER_DELAY_RESET 1000

#define LIGHT_ON  digitalWrite(RELAY_OUT_LIGHT, LOW)
#define LIGHT_OFF digitalWrite(RELAY_OUT_LIGHT, HIGH)

#define LOCK_ON  digitalWrite(RELAY_OUT_LOCK, LOW)
#define LOCK_OFF digitalWrite(RELAY_OUT_LOCK, HIGH)


const int DigitalLedPin = 5;

const int DigitalSensorPinMoon = 9;
bool DigitalSensorStateMoon = 0;
bool DigitalSensorSwitchedMoon = 0;

const int DigitalSensorPinSun = 5;
bool DigitalSensorStateSun = 0;
bool DigitalSensorSwitchedSun = 0;

uint8_t MoonBuffer;
uint8_t SunBuffer;
uint8_t SimCounter;

const uint8_t AnswerMoon=0x00;  //0b10100011;
const uint8_t AnswerSun=~AnswerMoon;

bool F_Final_Result=false;

auto timer = timer_create_default(); // create a timer with default settings
Timer<> default_timer; // save as above
bool F_timer_sensor_is_working=false;             // флаг сигнализирующий что таймер работает
//void OutModuleIndicate(ExtOut *Out, uint8_t Buffer; );
//************************For MMQTT server***************************
EthernetClient ethernetClient;

byte mqttServer[] = { 192, 168, 2, 60 };
void onTopicUpdate(char * topic, byte * payload, unsigned int length);
const uint8_t MAX_COMMAND_SIZE = 10;
PubSubClient pubSubClient(mqttServer, 1883, onTopicUpdate, ethernetClient);

PubSubClient &MqttShell::_pubSubClient = pubSubClient;

using namespace IO;
//*******************************************************************
// По порядку:
// Количество выходных модулей.
// Количество входных модулей.
// Пин защелкивания выходных модулей.
// Пин данных выходных модулей.
// Пин защелкивания входных модулей.
// Пин данных ыходных модулей.
// Сигнал тактирования модулей.

//*******************************************************************
Modules modules(2, 0, 19, 18, 17, 16, 15);
Modules &IExt::_modules = modules;

//***********Класс для ног ардуино****************
ExtOut<> MoonOut[8]={ExtOut<>(0, 0),ExtOut<>(0, 1),ExtOut<>(0, 2),ExtOut<>(0, 3),ExtOut<>(0, 4),ExtOut<>(0, 5),ExtOut<>(0, 6),ExtOut<>(0, 7)};
//const PROGMEM char Example4PREFIX[] = "arduino/Test/Example4";
//OutMqttShell Example4Shell(Example4, Example4PREFIX);

ExtOut<> SunOut[8]={ExtOut<>(1, 0),ExtOut<>(1, 1),ExtOut<>(1, 2),ExtOut<>(1, 3),ExtOut<>(1, 4),ExtOut<>(1, 5),ExtOut<>(1, 6),ExtOut<>(1, 7)};
//const PROGMEM char Example5PREFIX[] = "arduino/Test/Example5";
//OutMqttShell Example5Shell(Example5, Example5PREFIX);

//******************Сетевые настройки клиента************************
char mqttClientName[] = "potter";
uint8_t mac[] = { 0xFE, 0xCD, 0xCD, 0xEF, 0xFE, 0xEE };
IPAddress ip(192, 168, 3, 214);
IPAddress gateway(192, 168, 3, 1);
IPAddress subnet(255, 255, 255, 0);
const PROGMEM char DEVICE_PREFIX[] = "potter/Altar/";
WiznetShell wiznetShell(A0, mac, ip, gateway, subnet, mqttClientName, DEVICE_PREFIX);
void(* resetFunc) (void) = 0;//объявляем функцию reset с адресом 0


void setup()
{
  pinMode(RELAY_OUT_LOCK, OUTPUT); digitalWrite(RELAY_OUT_LOCK, HIGH);
  pinMode(RELAY_OUT_LIGHT, OUTPUT); digitalWrite(RELAY_OUT_LIGHT, HIGH);
  pinMode(VCC_SENSOR_OUT, OUTPUT); digitalWrite(VCC_SENSOR_OUT, HIGH);      // питание на сенсор
  pinMode(DigitalSensorPinMoon, INPUT);
  pinMode(DigitalLedPin, OUTPUT);                                // тестовый светодиод
  Serial.begin(115200); 
  Serial.print("AnswerMoon:"); Serial.println(AnswerMoon,BIN); 
  Serial.print("AnswerSun:");  Serial.println(AnswerSun,BIN); 
  Serial.println("setup end"); 
  //timer.every(100, timerHandler_FinalResultIndicate);
  attachInterrupt(0, SensorReadSun, FALLING );
  attachInterrupt(1, SensorReadMoon, FALLING );

  pubSubClient.setServer(mqttServer, 1883);
  pubSubClient.setCallback(onTopicUpdate);

  Ethernet.begin(mac, ip);
  if (pubSubClient.connect("potter", "testuser", "testpass")) {
    pubSubClient.publish("potter/Altar/state","hello world");
    pubSubClient.subscribe("potter/Altar/Solve/commands");
    pubSubClient.subscribe("potter/Altar/Activate/commands");
  }

}

void onTopicUpdate(char * topic, byte * payload, unsigned int length)
{
    payload[length] = '\0';
    // handle message arrived
    String strTopic = String(topic);
    String strPayload = String((char*)payload);

    if (strTopic == "potter/Altar/Solve/commands")
    {
        if (strPayload == "ON") {
          LOCK_ON; 
          pubSubClient.publish("potter/Altar/Solved/state","ON");
          Serial.println("potter/Altar/Solved/state ON");
        }
        else if (strPayload == "OFF") {
         // digitalWrite(RELAY_OUT, LOW);
        }
    }

    if (strTopic == "potter/Altar/Activate/commands")
    {
        if (strPayload == "ON") {
          LIGHT_ON;  
          Serial.println("potter/Altar/Activate/commands ON");
          
        }
          else if (strPayload == "OFF") {
          LIGHT_OFF; 
          Serial.println("potter/Altar/Activate/commands OFF - reset");
          resetFunc();
        }
    }



  /*
  char message[MAX_COMMAND_SIZE + 1];
  memcpy(message, payload, (length > MAX_COMMAND_SIZE) ? MAX_COMMAND_SIZE : length);
  message[(length > MAX_COMMAND_SIZE) ? MAX_COMMAND_SIZE : length] = '\0';
  MqttShell::doOnTopicUpdate(topic, message);*/
}

void OutModuleIndicate(ExtOut<> *Out, uint8_t Buff );

bool timerHandler_ResetButton(void *) {
  Serial.print("timerHandler_ResetButton");
  DigitalSensorSwitchedMoon=false;
  DigitalSensorSwitchedSun=false;
  F_timer_sensor_is_working=false;
  Serial.println(DigitalSensorSwitchedMoon);
  Serial.println(DigitalSensorSwitchedSun);
  return true; // repeat? true
}

void timerHandler_FinalResultIndicate(void *)
{
  // digitalWrite(DigitalLedPin, !digitalRead(DigitalLedPin)); // toggle the LED
  
  
  static uint8_t final_result_cycle=0;
  if (final_result_cycle==0) 
     {
         OutModuleIndicate(MoonOut, 0x00);
         OutModuleIndicate(SunOut, 0x00);
     }

  if (final_result_cycle==1) 
     {
         OutModuleIndicate(MoonOut, MoonBuffer);
         OutModuleIndicate(SunOut, SunBuffer);
     }

  if (final_result_cycle>1) 
     {  
         OutModuleIndicate(SunOut, 0x00);      
         OutModuleIndicate(MoonOut, 0x01<<final_result_cycle-2);
     }

   if (final_result_cycle>10) 
     {     
         OutModuleIndicate(MoonOut, 0x00);    
         OutModuleIndicate(SunOut, 0x80>>final_result_cycle-11);
     } 
   if (final_result_cycle<25) final_result_cycle++; 
   else 
   {
     OutModuleIndicate(MoonOut, MoonBuffer);
     OutModuleIndicate(SunOut, SunBuffer);
     Serial.print("final_result_cycle end"); 
     Serial.println(MoonBuffer); 
     Serial.println(SunBuffer); 
   }
}

void SensorReadMoon()
{
  if (!DigitalSensorSwitchedMoon){
           DigitalSensorSwitchedMoon= true;
     } 
}

void SensorReadSun()
{
  if (!DigitalSensorSwitchedSun){
          DigitalSensorSwitchedSun= true; 
     } 
}

void OutModuleIndicate(ExtOut<> *Out, uint8_t Buff )
{
   uint8_t i=0;
   for (i=0; i<8; i++)
   {
    if (Buff & 0x01)
    Out[i].setState(true);
    else
    Out[i].setState(false);
    Buff>>=1;
   } 
}
/*
#define LIGHT_ON  digitalWrite(RELAY_OUT_LIGHT, LOW)
#define LIGHT_OFF digitalWrite(RELAY_OUT_LIGHT, HIGH)

#define LOCK_ON  digitalWrite(RELAY_OUT_LOCK, LOW)
#define LOCK_OFF digitalWrite(RELAY_OUT_LOCK, HIGH)*/

void BlinkFinalResult(void)
{
   timer.every(100, timerHandler_FinalResultIndicate);
   Serial.println("timer final indication start");
}



void loop()
{
  //if (!pubSubClient.loop()) MqttShell::doOnReconnect();
  pubSubClient.loop();
  timer.tick(); // tick the timer
  modules.refresh();
 // MqttShell::doLoop();
  
/* Обработка сенсора Moon */
  //DigitalSensorStateMoon=digitalRead(DigitalSensorPinMoon);
  if (DigitalSensorSwitchedMoon==true && !F_timer_sensor_is_working){
           timer.in(TIMER_DELAY_RESET, timerHandler_ResetButton);  F_timer_sensor_is_working=true; Serial.println("timer start"); 
           Serial.print("Moon button switch:"); 
           MoonBuffer=MoonBuffer<<1;  SunBuffer=SunBuffer<<1; 
           MoonBuffer|=1; SimCounter++; 
           Serial.println(MoonBuffer, BIN); Serial.println(SunBuffer, BIN);
           Serial.println(SimCounter, DEC); 
           OutModuleIndicate(MoonOut, MoonBuffer);
           OutModuleIndicate(SunOut, SunBuffer);
           pubSubClient.publish("potter/Altar/Solved/state","ON");
           Serial.println("potter/Altar/Solved/state ON"); 
  } else DigitalSensorSwitchedMoon= false;
 /* ------------------------------------------ */
 /* Обработка сенсора Sun */
 //DigitalSensorStateSun=digitalRead(DigitalSensorPinSun);
  if (DigitalSensorSwitchedSun== true && !F_timer_sensor_is_working){
          timer.in(TIMER_DELAY_RESET, timerHandler_ResetButton); F_timer_sensor_is_working=true;  Serial.println("timer start");         
          Serial.print("Sun button switch:");
          MoonBuffer=MoonBuffer<<1;  SunBuffer=SunBuffer<<1; 
          SunBuffer|=1; SimCounter++; 
          Serial.println(MoonBuffer, BIN); Serial.println(SunBuffer, BIN);
          Serial.println(SimCounter, DEC);
          OutModuleIndicate(MoonOut, MoonBuffer);
          OutModuleIndicate(SunOut, SunBuffer); 
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
      //MoonBuffer=0; SunBuffer=0;
      if (!F_Final_Result) {BlinkFinalResult(); F_Final_Result=true;}
      pubSubClient.publish("potter/Altar/Solved/state","ON");
      Serial.println("potter/Altar/Solved/state ON");
      //OutModuleIndicate(MoonOut, MoonBuffer);
      //OutModuleIndicate(MoonOut, MoonBuffer);
   }
   else
   {
     LOCK_OFF;
     Serial.println("Answer failed"); 
     SimCounter=0;
     MoonBuffer=0; SunBuffer=0;
     OutModuleIndicate(MoonOut, MoonBuffer);
     OutModuleIndicate(SunOut, SunBuffer);
     //resetFunc(); //вызываем reset
   }
 }

 /*------------------------------------*/
 
}
