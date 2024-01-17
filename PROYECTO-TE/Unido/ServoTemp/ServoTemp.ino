#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include "DHT.h"
#include <Servo.h> 
Servo myservo; 
#define DHTPIN 0     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient; 
PubSubClient client(espClient);
char msg[50];
int value = 0;


void setup() {
  Serial.begin(115200);
  myservo.attach(2);
  Serial.println(F("DHTxx test!"));
  dht.begin();
  setup_wifi();
  client.setServer("192.168.0.228",1883);
  pinMode(0,OUTPUT);

  client.setCallback(callback);
}



void setup_wifi(){

  Serial.begin(115200);
  WiFi.begin("xiomylesly","111111111"); //se habilitará el modo estación

  // Mientras el ESP32 no se conecte al AP:
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  
  Serial.println(WiFi.localIP()); //Imprimo el IP del esp32
}


void reconnect(){

  if(client.connect("ESP32")){
    
    Serial.println("connected");
      client.subscribe("servo");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
}


void callback(char* topic, byte* payload, unsigned int length){
  String string;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  //payload[length]='\0';
  for (int i = 0; i < length; i++) {
   // Serial.print((char)payload[i]);
     string+=((char)payload[i]);  
  }
  Serial.print(string);

if (topic ="servo")  { 
    Serial.print(" ");
   int resultado = string.toInt();   
   int pos = map(resultado, 1, 100, 0, 180);
   Serial.println(pos);
    myservo.write(pos);
    delay(15); 
   
 }

 if (topic ="canal")  { 
    Serial.print("CANAL ");
  
   
 }

}

void loop() {
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if(!client.connected()){

    reconnect();
  }

   client.loop();

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  snprintf(msg,16,"%d,%d",t,h);
  
  client.publish("canal",msg);

  Serial.print(F("Humidity: "));
  Serial.print(h); ///////////////////////
  Serial.print(F("%  Temperature: "));
  Serial.print(t);//////////////////////
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
  delay(1000);
}


