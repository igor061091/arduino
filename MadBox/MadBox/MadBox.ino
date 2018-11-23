
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
Modules modules(1, 0, 19, 18, 17, 16, 15);
Modules &IExt::_modules = modules;

//***********Класс для ног ардуино****************

ArduOut<INVERT> FogPower(6);                                                                                                    
const PROGMEM char FogPowerPREFIX[] = "potter/MadBox/FogPower";
OutMqttShell FogPowerShell(FogPower, FogPowerPREFIX);

//*********Класс для модулей расширения***********

ExtOut<> Niche_Lock(0, 0);
const PROGMEM char Niche_LockPREFIX[] = "potter/MadBox/Niche/Lock";
OutMqttShell Niche_LockShell(Niche_Lock, Niche_LockPREFIX);

ExtOut<> Service_Lock(0, 1);
const PROGMEM char Service_LockPREFIX[] = "potter/MadBox/Service/Lock";
OutMqttShell Service_LockShell(Service_Lock, Service_LockPREFIX);

ExtOut<> Led(0, 2);
const PROGMEM char LedPREFIX[] = "potter/MadBox/Led";
OutMqttShell LedShell(Led, LedPREFIX);

ExtOut<> Fog(0, 3);
const PROGMEM char FogPREFIX[] = "potter/MadBox/Fog";
OutMqttShell FogShell(Fog,FogPREFIX);

ExtOut<> Effect(0, 4);
const PROGMEM char EffectPREFIX[] = "potter/MadBox/Effect";
OutMqttShell EffectShell(Effect, EffectPREFIX);

//******************Сетевые настройки клиента************************
char mqttClientName[] = "MadBox";
uint8_t mac[] = { 0xFE, 0xAD, 0xBD, 0xEF, 0xFE, 0xEE };
IPAddress ip(192, 168, 3, 216);
IPAddress gateway(192, 168, 3, 1);
IPAddress subnet(255, 255, 255, 0);
const PROGMEM char DEVICE_PREFIX[] = "potter/MadBox";
WiznetShell wiznetShell(A0, mac, ip, gateway, subnet, mqttClientName, DEVICE_PREFIX);


void setup()
{
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
}
