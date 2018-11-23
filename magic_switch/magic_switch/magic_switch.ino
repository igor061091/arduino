#include "ESP8266WiFi.h"
#include "ESP8266WiFiScan.h"
#include <Adafruit_NeoPixel.h>
#include <PubSubClient.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

// Adafruit define
#define PIN            D1
#define NUMPIXELS      16
//------------------------

#define MAX_RANGE 10 //условие уровня сигнала RSSI на котором переключаемся. 
#define LED_PIN LED_BUILTIN
#define DEBOUNCE_TIME 1500L
#define RIDDLE_TIME 500L

#define GOAL_LED_1 1
#define GOAL_LED_2 4
#define GOAL_LED_3 7
#define GOAL_LED_4 9
#define STAR_1 12
#define STAR_2 13
#define STAR_3 14
#define STAR_4 15

typedef enum {
  begin_1=1,
  ultimate_goal_2,
  start_expectation_3,
  technical_delay_4,
  minigame_5,
  minigame_error_6,
  minigame_solution_7,
} RiddleState_T;

uint8 ssid[] = "ESP_ap"; //имя AP "палочки"
long lastTouchMillis;
long lastRiddleMillis;
long lastSecondMillis;
long TimerSecond;
long TimerSecondPrevTick;

const char* ssid_ = "Request";
const char* password = "Playstorm4545";
IPAddress mqtt_server(192, 168, 2, 60);

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

bool WasTached_F=0;
bool RiddleBusy_F=0;
uint8_t TachedCounter=0;
RiddleState_T RiddleState=begin_1;
uint8_t NumPixel=0;

bool GoalLed1Touched_F=0;
bool GoalLed2Touched_F=0;
bool GoalLed3Touched_F=0;
bool GoalLed4Touched_F=0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid_);

  WiFi.begin(ssid_, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(LED_PIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(LED_PIN, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
 {  
//  Serial.begin(115200);
//  Serial.println("Hello!");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); //тушим светодиод

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.scanNetworks(false, false, 1, ssid);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pixels.begin(); // This initializes the NeoPixel library.  // Adafruit_NeoPixel
  Serial.begin(115200);
  Serial.println("setup end");
}

void switch_led (){
  static bool powerOn = false;
  if (!powerOn) {
    powerOn = true;
    digitalWrite(LED_PIN, LOW); //зажигаем светодиод
  }
  else
  {
    powerOn = false;
    digitalWrite(LED_PIN, HIGH); //тушим светодиод
  }
}

void BeginTask()
{
     //if(TimerSecond==TimerSecondPrevTick) return;  
     //TimerSecondPrevTick=TimerSecond;
     
     Serial.println("BeginTask");
     pixels.setPixelColor(STAR_1, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.setPixelColor(STAR_2, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.setPixelColor(STAR_3, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.setPixelColor(STAR_4, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.show(); // This sends the updated pixel color to the hardware.
     
    // RiddleState=ultimate_goal_2; 
}

void UltimateGoalTask()
{
     static uint8_t counter=0;
     Serial.println("UltimateGoalTask");

     if  (counter==0)
     {
       pixels.clear(); // This sends the updated pixel color to the hardware. 
       pixels.setPixelColor(GOAL_LED_1, pixels.Color(0,0,150)); // Moderately bright green color.
       pixels.show(); // This sends the updated pixel color to the hardware. 
     }
     if  (counter==1)
     {
       pixels.setPixelColor(GOAL_LED_2, pixels.Color(0,0,150)); // Moderately bright green color.
       pixels.show(); // This sends the updated pixel color to the hardware. 
       
     }
     if  (counter==2)
     {
       pixels.setPixelColor(GOAL_LED_3, pixels.Color(0,0,150)); // Moderately bright green color.
       pixels.show(); // This sends the updated pixel color to the hardware. 
       
     }
     if  (counter==3)
     {
       pixels.setPixelColor(GOAL_LED_4, pixels.Color(0,0,150)); // Moderately bright green color.
       pixels.show(); // This sends the updated pixel color to the hardware.
       
       RiddleState=start_expectation_3;  
       
     }
     /*pixels.setPixelColor(STAR, pixels.Color(0,0,150)); // Moderately bright green color.
     pixels.show(); // This sends the updated pixel color to the hardware.
     delay(RIDDLE_TIME); */
     counter++;
}

void  StartExpectationTask()
{
     Serial.println("StartExpectationTask");
     pixels.clear(); // This sends the updated pixel color to the hardware. 
     pixels.setPixelColor(STAR_1, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.setPixelColor(STAR_2, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.setPixelColor(STAR_3, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.setPixelColor(STAR_4, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.show(); // This sends the updated pixel color to the hardware.
     //RiddleState=technical_delay_4;  
}

void TechnicalDelayTask()
{
  static uint8_t counter=0;
  Serial.println("TechnicalDelayTask");
  if (counter>=5)
  {
  pixels.clear(); // This sends the updated pixel color to the hardware. 
  RiddleState=minigame_5; 
  }
  counter++;
}

void MinigameTask()
{
     Serial.println("MinigameTask");
     pixels.clear(); // This sends the updated pixel color to the hardware. 
     if (GoalLed1Touched_F) pixels.setPixelColor(GOAL_LED_1, pixels.Color(0,0,150)); // Moderately bright green color.
     if (GoalLed2Touched_F) pixels.setPixelColor(GOAL_LED_2, pixels.Color(0,0,150)); // Moderately bright green color.
     if (GoalLed3Touched_F) pixels.setPixelColor(GOAL_LED_3, pixels.Color(0,0,150)); // Moderately bright green color.
     if (GoalLed4Touched_F) pixels.setPixelColor(GOAL_LED_4, pixels.Color(0,0,150)); // Moderately bright green color.
     pixels.setPixelColor(STAR_1, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.setPixelColor(STAR_2, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.setPixelColor(STAR_3, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.setPixelColor(STAR_4, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.setPixelColor(NumPixel, pixels.Color(0,0,150)); // Moderately bright green color.
     pixels.show(); // This sends the updated pixel color to the hardware. 
}

void MinigameErrorTask()
{
     Serial.println("MinigameErrorTask");
     pixels.clear(); // This sends the updated pixel color to the hardware. 
     pixels.setPixelColor(STAR_1, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.setPixelColor(STAR_2, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.setPixelColor(STAR_3, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.setPixelColor(STAR_4, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.setPixelColor(NumPixel, pixels.Color(150,0,0)); // Moderately bright green color.
     pixels.show(); // This sends the updated pixel color to the hardware.
     RiddleState=minigame_5; 
}

void MinigameSolutionTask()
{
     Serial.println("MinigameSolutionTask");
     if (TimerSecond & 0x01)
         {
           pixels.clear();  pixels.show(); 
          }
          else
          {
            pixels.setPixelColor(GOAL_LED_1, pixels.Color(0,0,150)); // Moderately bright green color.
            pixels.setPixelColor(GOAL_LED_2, pixels.Color(0,0,150)); // Moderately bright green color.
            pixels.setPixelColor(GOAL_LED_3, pixels.Color(0,0,150)); // Moderately bright green color.
            pixels.setPixelColor(GOAL_LED_4, pixels.Color(0,0,150)); // Moderately bright green color.; // Moderately bright green color.
             pixels.show(); // This sends the updated pixel color to the hardware.
          } 
}



void loop()
{ 
   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  int n = WiFi.scanComplete();
  if(n >= 0)
  {
    int rssiVal = abs(WiFi.RSSI(0)); //RSSI берем по модулю, так удобнее
    WiFi.scanDelete();
    WiFi.scanNetworks(false,false, 1, ssid);
    if (rssiVal!=0){     
      //Serial.println(rssiVal);               
      if ((rssiVal>0)&&(rssiVal<=MAX_RANGE)){
        if (millis() - lastTouchMillis > DEBOUNCE_TIME){
          lastTouchMillis = millis();
          WasTached_F=true;
          Serial.println("WasTached_F"); 
          TachedCounter++;
          switch_led ();
          }
          
          //switch_led ();
      }   
    }

/*
    for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.

    delay(1000); // Delay for a period of time (in milliseconds).

  }*/

      if (millis() - lastSecondMillis > 1000L){      
         lastSecondMillis = millis();
         TimerSecond++;
      }
    
      if (millis() - lastRiddleMillis > RIDDLE_TIME){      
         lastRiddleMillis = millis();

      if (RiddleState==begin_1)
      {     
             if (WasTached_F) RiddleState=ultimate_goal_2; 
      }

      if (RiddleState==start_expectation_3)
      {     
             if (WasTached_F) RiddleState=technical_delay_4; 
      }
      
      if (RiddleState==minigame_5)
      {
          if (NumPixel<11){
             if (WasTached_F && NumPixel==GOAL_LED_1) GoalLed1Touched_F=true; 
             else if (WasTached_F && NumPixel==GOAL_LED_2) GoalLed2Touched_F=true; 
                  else if (WasTached_F && NumPixel==GOAL_LED_3) GoalLed3Touched_F=true; 
                      else if (WasTached_F && NumPixel==GOAL_LED_4) GoalLed4Touched_F=true; 
                           else if (WasTached_F) {
                              RiddleState=minigame_error_6;
                              GoalLed1Touched_F=false;
                              GoalLed2Touched_F=false; 
                              GoalLed3Touched_F=false; 
                              GoalLed4Touched_F=false;
                           }
             if (GoalLed1Touched_F && GoalLed2Touched_F && GoalLed3Touched_F && GoalLed4Touched_F) RiddleState=minigame_solution_7; 
             NumPixel++; 
          }
          else NumPixel=0;
      }
        
        switch (RiddleState)
        {
         case begin_1: BeginTask(); break;
         case ultimate_goal_2: UltimateGoalTask(); break;
         case start_expectation_3: StartExpectationTask(); break;
         case technical_delay_4: TechnicalDelayTask(); break;
         case minigame_5: MinigameTask(); break;
         case minigame_error_6:  MinigameErrorTask(); break;
         case minigame_solution_7: MinigameSolutionTask(); break;
        }
       
        WasTached_F=false;
        Serial.println("NumPixel");  
        Serial.println(NumPixel);       
      }   


       
  }
}
