
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#include <MqttShell.h>
#include <Interfaces.h>
#include <InterfacesShell.h>

#include <ArduinoIO.h>
#include <IO_boards.h>

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
Modules modules(1, 1, 19, 18, 17, 16, 15);
Modules &IExt::_modules = modules;

//***********Класс для ног ардуино****************
/*
ArduIn<INVERT> Example1(2);                                                                                                    
const PROGMEM char Example1PREFIX[] = "arduino/Test/Example1";
InMqttShell Example1Shell(Example1, Example1PREFIX);

ArduOut<> Example2(3);                                                                                                    
const PROGMEM char Example2PREFIX[] = "arduino/Test/Example2";
OutMqttShell Example2Shell(Example2, Example2PREFIX);
*/
//*********Класс для модулей расширения***********

// ins

ExtIn<> Reader0(0, 0);                                                                                 
const PROGMEM char Reader0PREFIX[] = "potter/Quidditch/Reader0";
InMqttShell Reader0Shell(Reader0,Reader0PREFIX);

ExtIn<> Reader1(0, 1);                                                                                 
const PROGMEM char Reader1PREFIX[] = "potter/Quidditch/Reader1";
InMqttShell Reader1Shell(Reader1, Reader1PREFIX);

ExtIn<> Reader2(0, 2);                                                                                 
const PROGMEM char Reader2PREFIX[] = "potter/Quidditch/Reader2";
InMqttShell Reader2Shell(Reader2, Reader2PREFIX);

ExtIn<> Reader3(0, 3);                                                                                 
const PROGMEM char Reader3PREFIX[] = "potter/Quidditch/Reader3";
InMqttShell Reader3Shell(Reader3, Reader3PREFIX);

ExtIn<> Reader4(0, 4);                                                                                 
const PROGMEM char Reader4PREFIX[] = "potter/Quidditch/Reader4";
InMqttShell Reader4Shell(Reader4, Reader4PREFIX);

ExtIn<> Reader5(0, 5);                                                                                 
const PROGMEM char Reader5PREFIX[] = "potter/Quidditch/Reader5";
InMqttShell Reader5Shell(Reader5, Reader5PREFIX);

ExtIn<> Reader6(0, 6);                                                                                 
const PROGMEM char Reader6PREFIX[] = "potter/Quidditch/Reader6";
InMqttShell Reader6Shell(Reader6, Reader6PREFIX);

// outs

ExtOut<> Lock1(0, 0);
const PROGMEM char Lock1PREFIX[] = "potter/Quidditch/Lock1";
OutMqttShell Lock1Shell(Lock1, Lock1PREFIX);

ExtOut<> Lock2(0, 1);
const PROGMEM char Lock2PREFIX[] = "potter/Quidditch/Lock2";
OutMqttShell Lock2Shell(Lock2, Lock2PREFIX);

ExtOut<> Box_Lock(0, 2);
const PROGMEM char Box_LockPREFIX[] = "potter/Quidditch/Box_Lock";
OutMqttShell Box_LockShell(Box_Lock, Box_LockPREFIX);

ExtOut<> Led_R(0, 3);
const PROGMEM char Led_RPREFIX[] = "potter/Quidditch/Led_R";
OutMqttShell Led_RShell(Led_R, Led_RPREFIX);

ExtOut<> Led_G(0, 4);
const PROGMEM char Led_GPREFIX[] = "potter/Quidditch/Led_G";
OutMqttShell Led_GShell(Led_G, Led_GPREFIX);

ExtOut<> Led_B(0, 5);
const PROGMEM char Led_BPREFIX[] = "potter/Quidditch/Led_B";
OutMqttShell Led_BShell(Led_B, Led_BPREFIX);

ExtOut<> Save(0, 6);
const PROGMEM char SavePREFIX[] = "potter/Quidditch/Save";
PulseOutMqttShell pulseSaveShell(Led_B, SavePREFIX, 500);

//******************Сетевые настройки клиента************************
char mqttClientName[] = "Quidditch";
uint8_t mac[] = { 0xFE, 0xAD, 0xBD, 0xEF, 0xFE, 0xEE };
IPAddress ip(192, 168, 3, 215);
IPAddress gateway(192, 168, 3, 1);
IPAddress subnet(255, 255, 255, 0);
const PROGMEM char DEVICE_PREFIX[] = "potter/Quidditch";
WiznetShell wiznetShell(A0, mac, ip, gateway, subnet, mqttClientName, DEVICE_PREFIX);


void setup()
{
  pubSubClient.setServer(mqttServer, 1883);
  pubSubClient.setCallback(onTopicUpdate);

  pubSubClient.subscribe("potter/Quidditch/Lock1");
  pubSubClient.subscribe("potter/Quidditch/Lock2");
  pubSubClient.subscribe("potter/Quidditch/Box/Lock");
  pubSubClient.subscribe("potter/Quidditch/Led_R");
  pubSubClient.subscribe("potter/Quidditch/Led_G");
  pubSubClient.subscribe("potter/Quidditch/Led_B");
  pubSubClient.subscribe("potter/Quidditch/Save");
}

void onTopicUpdate(char * topic, byte * payload, unsigned int length)
{
  char message[MAX_COMMAND_SIZE + 1];
  memcpy(message, payload, (length > MAX_COMMAND_SIZE) ? MAX_COMMAND_SIZE : length);
  message[(length > MAX_COMMAND_SIZE) ? MAX_COMMAND_SIZE : length] = '\0';
  /*
    if (topic == "potter/Quidditch/Lock1")
      {
          if (payload == "ON") {
            Lock1.setState(true); 
            pubSubClient.publish("potter/Quidditch/Lock1/state","ON");
            Serial.println("potter/Altar/Solved/state ON");
          }
          else if (payload == "OFF") {
            Lock1.setState(false); 
            pubSubClient.publish("potter/Quidditch/Lock1/state","OFF");
          }
      }*/
      
  MqttShell::doOnTopicUpdate(topic, message);
}

void loop()
{
 if (!pubSubClient.loop()) MqttShell::doOnReconnect();
 modules.refresh();
 MqttShell::doLoop(); 
 //Led_B.setState(true);
 //Led_G.setState(true);
 //Lock1.setState(true);
}
