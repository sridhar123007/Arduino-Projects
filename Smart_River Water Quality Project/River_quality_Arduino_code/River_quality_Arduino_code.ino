#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(8,9);
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
#define SensorPin A0             //pH meter Analog output to Arduino1 Analog Input 0
#define Offset 0.00            //deviation compensate
unsigned long int avgValue;     //Store the average value of the sensor feedback


float t,turbidity,phValue1;
void setup()
{
 
  Serial.begin(115200);  
   sensors.begin();
  Serial.println("Ready");    //Test the serial monitor
  mySerial.begin(115200);
}
void loop()
{
  sensors.requestTemperatures(); 
  
  Serial.print("Celsius temperature: ");
  // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
   
  t=sensors.getTempCByIndex(0);
  Serial.print(t);
  delay(1000);
  float phValue=(float)analogRead(SensorPin)*5.0/1024; //convert the analog into millivolt
  phValue1=3.5*phValue+Offset;                      //convert the millivolt into pH value
  Serial.print("    pH:");  
  Serial.println(phValue1,2);
   delay(1000);

  int sensorValue = analogRead(A1);  //turbidity sensor
  turbidity = sensorValue * (5.0 / 1024.0);
  Serial.print("Sensor Output (V):");
  Serial.println(turbidity);
  delay(1000);
  sendAndroidValues();
}



void sendAndroidValues()
{
  //puts # before the values so our app knows what to do with the data
 // mySerial.print('#');
  //Serial.print('#');
  //for loop cycles through 2 sensors and sends values via serial
  String payload = "#{\"d\":{\"temperatureC\":";
  payload += t;
  payload+="," "\"pH_Value\":";
  payload += phValue1;
  payload+="," "\"turbidity\":";
  payload += turbidity;
  payload += "}}~";
 Serial.print("Sending payload: ");
 Serial.println(payload);
 mySerial.print(payload);
 // mySerial.println('~'); //used as an end of transmission character - used in app for string length
 // Serial.println('~');
  delay(2000);        //added a delay to eliminate missed transmissions
}

