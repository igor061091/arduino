
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

//*********Класс для модулей расширения***********

ExtIn<> Sensor0(0, 0);                                                                                 
const PROGMEM char Sensor0PREFIX[] = "potter/Corridor/Floor/Sensor0";
InMqttShell Sensor0Shell(Sensor0,Sensor0PREFIX);

ExtIn<> Sensor1(0, 1);                                                                                 
const PROGMEM char Sensor1PREFIX[] = "potter/Corridor/Floor/Sensor1";
InMqttShell Sensor1Shell(Sensor1,Sensor1PREFIX);

ExtIn<> Sensor2(0, 2);                                                                                 
const PROGMEM char Sensor2PREFIX[] = "potter/Corridor/Floor/Sensor2";
InMqttShell Sensor2Shell(Sensor2,Sensor2PREFIX);

ExtIn<> Sensor3(0, 3);                                                                                 
const PROGMEM char Sensor3PREFIX[] = "potter/Corridor/Floor/Sensor3";
InMqttShell Sensor3Shell(Sensor3,Sensor3PREFIX);

ExtIn<> Sensor4(0, 4);                                                                                 
const PROGMEM char Sensor4PREFIX[] = "potter/Corridor/Floor/Sensor4";
InMqttShell Sensor4Shell(Sensor4,Sensor4PREFIX);

ExtIn<> Sensor5(0, 5);                                                                                 
const PROGMEM char Sensor5PREFIX[] = "potter/Corridor/Floor/Sensor5";
InMqttShell Sensor5Shell(Sensor5,Sensor5PREFIX);

ExtIn<> MainDoorSensor(0, 6);                                                                                 
const PROGMEM char MainDoorSensorPREFIX[] = "potter/Corridor/MainDoor/Sensor";
InMqttShell MainDoorSensorShell(MainDoorSensor,MainDoorSensorPREFIX);

ExtIn<> CorridorDoorSensor(0, 7);                                                                                 
const PROGMEM char CorridorDoorSensorPREFIX[] = "potter/Corridor/CorridorDoor/Sensor";
InMqttShell CorridorDoorSensorShell(CorridorDoorSensor,CorridorDoorSensorPREFIX);

// outs

ExtOut<> MainDoorLock(0, 0);
const PROGMEM char MainDoorLockPREFIX[] = "potter/Corridor/MainDoor/Lock";
OutMqttShell MainDoorLockShell(MainDoorLock, MainDoorLockPREFIX);

ExtOut<> CorridorDoorLock(0, 1);
const PROGMEM char CorridorDoorLockPREFIX[] = "potter/Corridor/CorridorDoor/Lock";
OutMqttShell CorridorDoorLockShell(CorridorDoorLock, CorridorDoorLockPREFIX);

ExtOut<> CorridorWall1Light(0, 2);
const PROGMEM char CorridorWall1LightPREFIX[] = "potter/Corridor/CorridorWall1/Light";
OutMqttShell CorridorWall1LightShell(CorridorWall1Light, CorridorWall1LightPREFIX);

ExtOut<> CorridorWall2Light(0, 3);
const PROGMEM char CorridorWall2LightPREFIX[] = "potter/Corridor/CorridorWall2/Light";
OutMqttShell CorridorWall2LightShell(CorridorWall2Light, CorridorWall2LightPREFIX);

ExtOut<> UFLight(0, 4);
const PROGMEM char UFLightPREFIX[] = "potter/Corridor/UFLight";
OutMqttShell UFLightShell(UFLight, UFLightPREFIX);

ExtOut<> FireLight(0, 5);
const PROGMEM char FireLightPREFIX[] = "potter/Corridor/FireLight";
OutMqttShell FireLightShell(FireLight, FireLightPREFIX);

ExtOut<> RunningLight(0, 6);
const PROGMEM char RunningLightPREFIX[] = "potter/Corridor/RunningLight";
OutMqttShell RunningLightShell(RunningLight, RunningLightPREFIX);

//******************Сетевые настройки клиента************************
char mqttClientName[] = "corridorDevice";
uint8_t mac[] = { 0xFE, 0xAD, 0xBD, 0xEF, 0xFE, 0xEE };
IPAddress ip(192, 168, 3, 218);
IPAddress gateway(192, 168, 3, 1);
IPAddress subnet(255, 255, 255, 0);
const PROGMEM char DEVICE_PREFIX[] = "potter/Corridor";
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
 // Example4.setState(true); 
 // Example2.setState(true); 
}
