
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

byte mqttServer[] = { 192, 168, 2, 90 };
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
ArduIn<INVERT> sensor(2);                                                                                                    
const PROGMEM char sensorPREFIX[] = "potter/SunStatue/sensor";
InMqttShell sensorShell(sensor, sensorPREFIX);

ArduOut<> Led1(5);                                                                                                    
const PROGMEM char Led1PREFIX[] = "potter/SunStatue/Led1";
OutMqttShell Led1Shell(Led1, Led1PREFIX);

ArduOut<> Led2(6);                                                                                                    
const PROGMEM char Led2PREFIX[] = "potter/SunStatue/Led2";
OutMqttShell Led2Shell(Led2, Led2PREFIX);

ArduOut<> Lock(7);                                                                                                    
const PROGMEM char LockPREFIX[] = "potter/SunStatue/Lock";
OutMqttShell LockShell(Lock, LockPREFIX);


//******************Сетевые настройки клиента************************
char mqttClientName[] = "SunStatue";
uint8_t mac[] = { 0xFE, 0xAD, 0xBD, 0xEF, 0xFE, 0xEE };
IPAddress ip(192, 168, 3, 217);
IPAddress gateway(192, 168, 3, 1);
IPAddress subnet(255, 255, 255, 0);
const PROGMEM char DEVICE_PREFIX[] = "potter/SunStatue";
WiznetShell wiznetShell(A0, mac, ip, gateway, subnet, mqttClientName, DEVICE_PREFIX);


void setup()
{
  pinMode(4, OUTPUT);
  pubSubClient.setServer(mqttServer, 1883);
  pubSubClient.setCallback(onTopicUpdate);
}

void onTopicUpdate(char * topic, byte * payload, unsigned int length)
{
  char message[MAX_COMMAND_SIZE + 1];
  memcpy(message, payload, (length > MAX_COMMAND_SIZE) ? MAX_COMMAND_SIZE : length);
  message[(length > MAX_COMMAND_SIZE) ? MAX_COMMAND_SIZE : length] = '\0';
  MqttShell::doOnTopicUpdate(topic, message);
}

void loop()
{
  if (!pubSubClient.loop()) MqttShell::doOnReconnect();
  modules.refresh();
  MqttShell::doLoop();
 // Example2.setState(true); 
 // Example2.setState(true); 
}
