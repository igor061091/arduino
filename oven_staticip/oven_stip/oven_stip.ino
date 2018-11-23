/*
 Basic MQTT example with Authentication

  - connects to an MQTT server, providing username
    and password
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic"
*/
/*
fromFuture/Furnance/Solved/state                    в решенном состоянии возвращает команду ON
fromFuture/Furnance/Solve/commands        принудительно решает загадку командой ON
fromFuture/Furnance/Reset/commands          сбрасывает решенную загадку и возвращает по топику /Solve/state команду OFF
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <timer.h>

#define RELAY_OUT 8 
#define IMPULSE_COUNT 3
#define TIMER_DELAY_RESET 2
#define RELAY_WORK_DELAY  1
#define TIMER_RESET_DEVICE  60

// Update these with values suitable for your network.
byte mac[]    = {  0x5E, 0xFD, 0xCA, 0xCE, 0xFE, 0xED };
IPAddress server(192, 168, 2, 90);             
IPAddress ip(192, 168, 2, 99);
//IPAddress ip(192, 168, 3, 217);
void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

bool F_timer_reset_is_working=false;             // флаг сигнализирующий что таймер работает
const int digitalInPin = 3; // цифровой вход 5
const int digitalPinGnd = 4; // цифровой выход 4
const int digitalPinVcc = 5; // цифровой выход 3
uint8_t impulse_counter;
bool impulse_was_touched_F=0;
int dValue;
long lastMillis;

uint8_t Timer_ResetImpulse=0;
uint8_t   Timer_RelayDelay=0;
uint8_t Timer_Reset_Devise=TIMER_RESET_DEVICE;
void(* resetFunc) (void) = 0;//объявляем функцию reset с адресом 0

void Handler_ISR() 
{
  Serial.print("Handler_ISR");
  if (!impulse_was_touched_F) impulse_was_touched_F=true;
}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
   payload[length] = '\0';
  // handle message arrived
    String strTopic = String(topic);
    String strPayload = String((char*)payload);

    if (strTopic == "dugout/Furnance/Solve/commands")
    {
        if (strPayload == "ON") {
          digitalWrite(RELAY_OUT, HIGH); Timer_RelayDelay=RELAY_WORK_DELAY;
          //timer.in(RELAY_WORK_DELAY, timerHandler_RelayDalay);
          client.publish("dugout/Furnance/Solved/state", "ON");
          Serial.println("dugout/Furnance/Solved/state ON");
        }
        else if (strPayload == "OFF") {
         // digitalWrite(RELAY_OUT, LOW);
        }
    }

    if (strTopic == "dugout/Furnance/Reset/commands")
    {
        if (strPayload == "ON") {
          digitalWrite(RELAY_OUT, LOW);
          client.publish("dugout/Furnance/Solved/state","OFF");
          Serial.println("dugout/Furnance/Solved/state OFF");
          
        }
          else if (strPayload == "OFF") {
          //digitalWrite(RELAY_OUT, LOW);
        }
    }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("FurnanceDevice")) {
      Serial.println("connected");
      Serial.print("My IP address: ");
      for (byte thisByte = 0; thisByte < 4; thisByte++) {
        // print the value of each byte of the IP address:
        Serial.print(Ethernet.localIP()[thisByte], DEC);
        Serial.print(".");
      }
      // Once connected, publish an announcement...
      client.publish("dugout/Furnance/state", "ONLINE");
      // ... and resubscribe
       client.subscribe("dugout/Furnance/Solve/commands");
       client.subscribe("dugout/Furnance/Reset/commands");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      digitalWrite(RELAY_OUT, LOW);
      delay(5000);
      resetFunc();
    }
  }
}
void setup()
{
  Ethernet.begin(mac, ip);
  client.setServer(server, 1883);   
  client.setCallback(callback);
  Serial.begin(115200);
 
  // Note - the default maximum packet size is 128 bytes. If the
  // combined length of clientId, username and password exceed this,
  // you will need to increase the value of MQTT_MAX_PACKET_SIZE in
  // PubSubClient.h
  if (client.connect("FurnanceDevice", "testuser", "testpass")) {
    client.publish("dugout/Furnance/state","ONLINE");
    client.subscribe("dugout/Furnance/Solve/commands");
    client.subscribe("dugout/Furnance/Reset/commands");
  }
  pinMode(digitalInPin, INPUT);
  pinMode(digitalPinGnd, OUTPUT);
  pinMode(digitalPinVcc, OUTPUT);
  pinMode(RELAY_OUT, OUTPUT);
  //attachInterrupt(buttonInt, intHandler, FALLING );
  Serial.begin(115200);

  // Выводим наш адрес:
  Serial.print("IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println();

  digitalWrite(digitalPinVcc, HIGH);   // подача питания на датчик
  digitalWrite(digitalPinGnd, LOW);
  
  interrupts(); 
  attachInterrupt(1, Handler_ISR, FALLING );
  // настройки закончены
  Serial.println("setup end");
}

void loop()
{
   if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (millis() - lastMillis > 1000L){
    lastMillis = millis();
    
    if (!Timer_Reset_Devise--) { Serial.println("SystemReset"); resetFunc(); }
    if (!Timer_ResetImpulse--) {impulse_counter=0; Serial.println("Timer_ResetImpulse"); }
    if (!Timer_RelayDelay--)   {digitalWrite(RELAY_OUT, LOW); Serial.println("Timer_RelayDelay"); }
    
    if (impulse_was_touched_F)     
    {
      Timer_ResetImpulse=TIMER_DELAY_RESET;
      Serial.println("--------- HIGH SOUND ----------");
      //if (!Timer_ResetImpulse--) impulse_counter=0;
      impulse_counter++;
      Serial.println("impulse_counter"); Serial.println(impulse_counter);
      if (impulse_counter>=IMPULSE_COUNT) {digitalWrite(RELAY_OUT, HIGH); impulse_counter=0; Timer_RelayDelay=RELAY_WORK_DELAY;  client.publish("dugout/Furnance/Solved/state","ON"); }
      impulse_was_touched_F=false;
    }
  }  
  
}
