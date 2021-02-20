#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
//-------- Customise these values -----------
const char* ssid = "Sridhar007";
const char* password = "Sridhar07";

//-------- Customise the above values --------
#define ORG "nlm7m3"
#define DEVICE_TYPE "IoT"
#define DEVICE_ID "1234"
#define TOKEN "12345678"
//-------- Customise the above values --------

  #define trigPin D5  //pins to be connected  to NODEMCU
 #define echoPin D6
 
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/Data/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
 
WiFiClient wifiClient;
PubSubClient client(server,1883,wifiClient);

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
 
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
  int duration, distance;
  digitalWrite(trigPin, HIGH);
  delay(1000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance =(duration/2) * 0.0343;// time * speed of sound(0.0343cm/microsec)
  //Serial.print(distance);
  //Serial.println(" cm");
  delay(1000);
PublishData(distance);
 if (!client.loop()) {
    mqttConnect();
  }
delay(100);
}
void mqttConnect() {
  if (!client.connected()) {
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

void PublishData(int distance){
 if (!!!client.connected()) {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
  String payload = "{\"d\":{\"distance\":";
  payload += 20;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(topic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
}
