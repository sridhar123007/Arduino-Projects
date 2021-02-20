#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET D5
/* Object named display, of the class Adafruit_SSD1306 */
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

Servo myservo;
#include "DHT.h"  
#define DHTPIN D3   
#define DHTTYPE DHT11   
DHT dht (DHTPIN, DHTTYPE);

String command;
String data="";


void callback(char* topic, byte* payload, unsigned int payloadLength);

//-------- Customise these values -----------
const char* ssid = "Smartbridge";
const char* password = "iotgyan@sb";

//-------- Customise the above values --------
#define ORG "nlm7m3"
#define DEVICE_TYPE "IoT"
#define DEVICE_ID "1234"
#define TOKEN "12345678"

//  PIN DECLARATIONS 

#define trigPin D6
#define echoPin D7

float h,t;
int duration,distance,moisture;

//-------- Customise the above values --------
const char publishTopic[] = "iot-2/evt/Data/fmt/json";
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/cmd/home/fmt/String";// cmd  REPRESENT command type AND COMMAND IS TEST OF FORMAT STRING
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;


WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

int publishInterval = 5000; // 30 seconds
long lastPublishMillis;
void publishData();

void drawStr(uint8_t x, uint8_t y, char* str){
  display.setCursor(x, y);  /* Set x,y coordinates */
  display.println(str);
}

void setup() {
  
  Serial.begin(115200);
  Serial.println();
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
  myservo.attach(D5);
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); /* Initialize display with address 0x3C */
  display.clearDisplay();  /* Clear display */
  display.setTextSize(1);  /* Select font size of text. Increases with size of argument. */
  display.setTextColor(WHITE);  /* Color of text*/
  Serial.println(""); 
  wifiConnect();
  mqttConnect();
}

void loop() {


 if (millis() - lastPublishMillis > publishInterval)
  {
    publishData();
    lastPublishMillis = millis();
  }
  
  if (!client.loop()) {
    mqttConnect();
  }
}

void wifiConnect() {
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFi connected, IP address: "); 
  Serial.println(WiFi.localIP());
}

void mqttConnect() {
  if (!client.connected()) 
  {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    
    initManagedDevice();
    Serial.println();
  }
}

void initManagedDevice() {
  if (client.subscribe(topic)) {
   
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
  
  Serial.print("callback invoked for topic: ");
  Serial.println(topic);

  for (int i = 0; i < payloadLength; i++) {
    
    command+= (char)payload[i];
  }
  
  Serial.print("data: "+ command);
    if(command == "MOTORON"&& moisture<50)
  {
    myservo.write(180);
    Serial.print("Motor ON");
  }
  else if(command == "MOTOROFF"&& moisture>90)
  {
     myservo.write(0);
    Serial.print("Motor OFF");
  }
  command= "";
}


void publishData() 
{
  delay(1000);
h = dht.readHumidity();
t = dht.readTemperature();
if (isnan(h) || isnan(t))
{
Serial.println("Failed to read from DHT sensor!");
return;
}
Serial.print("Humidity: ");
Serial.print(h);
Serial.print(" %\t");
Serial.print("Temperature: ");
Serial.print(t);
Serial.println(" *C ");
Serial.print("temp");
    moisture = analogRead(A0);
    moisture = map(moisture,0,1023,0,100);
  digitalWrite(trigPin, HIGH);
  delay(1000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance =(duration/2) * 0.0343;// time * speed of sound(0.0343cm/microsec)
  Serial.print(distance);
  Serial.println(" cm");
  delay(1000);
  Serial.println("moisture is :");
  Serial.println(moisture);
  delay(1000);

    if(moisture<50)
  {
  display.clearDisplay();
  drawStr(20, 50, "Am sad");
  display.display();
  myservo.write(90);
  
  }
  else if(moisture>90)
  {
  display.clearDisplay();
  drawStr(20, 50, "Am Happy");
  display.display();
   myservo.write(0);
  }
  if(distance<5)
  {
  Serial.println("Welcome");
  display.clearDisplay();
  drawStr(20, 50, "Welcome");
  display.display();
  }
  else if(distance>10 && distance<20)
  {
  Serial.println("BYE..");
  display.clearDisplay();
  drawStr(20, 50, "BYE...");
  display.display();
  }
  

  
  String payload = "{\"d\":{\"temperature\":";
  payload += t;
  payload += ",""\"humidity\":";
  payload +=  h;
  payload += ",""\"distance\":";
  payload += distance;
  payload +=",""\"Moisture\":";
  payload += moisture;
  payload += "}}";


  Serial.print("\n");
  Serial.print("Sending payload: "); Serial.println(payload);

  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish OK");
  } else {
    Serial.println("Publish FAILED");
  }
}
