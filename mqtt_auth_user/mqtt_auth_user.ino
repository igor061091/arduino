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

#define RELAY_OUT 4 
#define IMPULSE_COUNT 3


// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xCD, 0xCA, 0xCE, 0xFE, 0xED };
IPAddress ip(192, 168, 3, 217);
IPAddress server(192, 168, 2, 60);
void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

bool F_timer_reset_is_working=false;             // флаг сигнализирующий что таймер работает
bool F_from_callback_need_publish_reset=false; 
bool F_from_callback_need_publish_solve=false;    
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


void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
   payload[length] = '\0';
  // handle message arrived
    String strTopic = String(topic);
    String strPayload = String((char*)payload);

    if (strTopic == "fromFuture/Furnance/Solve/commands")
    {
        if (strPayload == "ON") {
          digitalWrite(RELAY_OUT, HIGH);
          F_from_callback_need_publish_solve=true;   
          client.publish("fromFuture/Furnance/Solved/state","ON");
        }
        else if (strPayload == "OFF") {
         // digitalWrite(RELAY_OUT, LOW);
        }
    }

    if (strTopic == "fromFuture/Furnance/Reset/commands")
    {
        if (strPayload == "ON") {
          digitalWrite(RELAY_OUT, LOW);
          F_from_callback_need_publish_reset=true; 
          client.publish("fromFuture/Furnance/Solved/state","OFF");
          
        }
          else if (strPayload == "OFF") {
          //digitalWrite(RELAY_OUT, LOW);
        }
    }
}


void setup()
{
  Ethernet.begin(mac, ip);
  Serial.begin(115200);
 
  // Note - the default maximum packet size is 128 bytes. If the
  // combined length of clientId, username and password exceed this,
  // you will need to increase the value of MQTT_MAX_PACKET_SIZE in
  // PubSubClient.h
  if (client.connect("arduino", "testuser", "testpass")) {
    client.publish("fromFuture/Furnance/Solved/state","hello world");
    client.subscribe("fromFuture/Furnance/Solve/commands");
    client.subscribe("fromFuture/Furnance/Reset/commands");
  }
  pinMode(digitalInPin, INPUT);
  pinMode(RELAY_OUT, OUTPUT);
  //attachInterrupt(buttonInt, intHandler, FALLING );
  Serial.begin(115200);
  Serial.println("setup end");
}

void loop()
{
      client.loop();
      timer.tick(); // tick the timer
      dValue = digitalRead(digitalInPin);
      if (dValue == LOW)
        {
          Serial.println("--------- HIGH SOUND ----------");
          if (!F_timer_reset_is_working) {timer.in(5000, timerHandler_ResetImpulse);F_timer_reset_is_working=true;}
          impulse_counter++;
          Serial.println("impulse_counter"); Serial.println(impulse_counter);
          if (impulse_counter>=IMPULSE_COUNT) {digitalWrite(RELAY_OUT, HIGH); client.publish("fromFuture/Furnance/Solved/state","ON");}
          delay(500);
        }
        /*
      if(F_from_callback_need_publish_reset)
        {       
             client.publish("fromFuture/Furnance/Solved/state","OFF");     
             F_from_callback_need_publish_reset=false;
             Serial.println("fromFuture/Furnance/Solved/state OFF");                   
        }
      if(F_from_callback_need_publish_solve)
        {       
             client.publish("fromFuture/Furnance/Solved/state","ON");
             F_from_callback_need_publish_solve=false;  
             Serial.println("fromFuture/Furnance/Solved/state ON");                     
        }
        */
 }
  
