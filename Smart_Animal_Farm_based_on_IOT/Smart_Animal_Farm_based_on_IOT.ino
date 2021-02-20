#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

int pos = 0;    // variable to store the servo position

#define DHTPIN D2     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);


//ULTRASONIC SENSOR (HC-SR04) DISTANCE MEASUREMENT//
#define trigPin D3
#define echoPin D4

int duration, distance;
   
//-------- Customise these values -----------
const char* ssid = "Smartbridge";
const char* password = "iotgyan@sb";

//-------- Customise the above values --------
#define ORG "nlm7m3"
#define DEVICE_TYPE "IoT"
#define DEVICE_ID "1234"
#define TOKEN "12345678"
//-------- Customise the above values --------
 
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/Data/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
 
WiFiClient wifiClient;
PubSubClient client(server, 1883,wifiClient);

void setup() {
    Serial.println("DHTxx test!");

  dht.begin();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(D1, OUTPUT); 
  pinMode(D7, OUTPUT);
  pinMode(D0, OUTPUT);
  Serial.print("Ultrasonic Sensor Starting!!!");
  Serial.println("");
 Serial.begin(115200);
 Serial.println();
 Serial.print("Connecting to ");
 Serial.print(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 } 
 Serial.println("");
 
 Serial.print("WiFi connected, IP address: ");
 Serial.println(WiFi.localIP());
}
 
void loop() {

  PublishData();
  delay(1000);
}



void PublishData(){
 if (!!!client.connected()) {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
 digitalWrite(D1,HIGH);
 digitalWrite(D1,LOW);
   // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");

    digitalWrite(trigPin, HIGH);
  delay(1000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance =(duration/2) * 0.0343;// time * speed of sound(0.0343cm/microsec)
  Serial.print("Distance is");
  Serial.println(distance);

  int Fire=digitalRead(D6);
  Serial.print("Fire :");
  Serial.println(Fire);

  int Gas=analogRead(A0);
  Serial.print("Harmful :");
  Serial.println(Gas);
  
  if(distance<=10){
  digitalWrite(D7,HIGH); 
  }
  else{
    digitalWrite(D7,LOW);
    }
    if(Fire==1){
  digitalWrite(D0,HIGH); 
  }
  else{
    digitalWrite(D0,LOW);
    }
  String payload = "{\"d\":{\"distance\":";
  payload += distance;
  payload+="," "\"Temperature\":";
  payload += t;
  payload+="," "\"Humidity\":";
  payload += h;
  payload+="," "\"Harmful\":";
  payload += Gas;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(topic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
}
