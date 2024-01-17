#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <OneWire.h>
#define DHTTYPE DHT22
#include <Servo.h>
Servo myservo;
const char* ssid = "xiomylesly";
const char* password = "111111111";

const char* mqtt_server = "192.168.0.228";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
const int DHTPin = 0;
const int lamp = 4;

DHT dht(DHTPin, DHTTYPE);

long now = millis();
long lastMeasure = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  String string;
  //byte* payload;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
    //string += ((char)payload[i]);
  }
  //Serial.print(string);
  Serial.println();
  if (topic == "room/lamp") {
    Serial.print("Changing Room lamp to ");
    if (messageTemp == "on") {
      digitalWrite(lamp, HIGH);
      Serial.print("On");
    } else if (messageTemp == "off") {
      digitalWrite(lamp, LOW);
      Serial.print("Off");
    }
  }
  Serial.println();
  if (topic = "servo") {
    Serial.print(" ");
    int resultado = string.toInt();
    int pos = map(resultado, 1, 100, 0, 180);
    Serial.println(pos);
    myservo.write(pos);
    delay(15);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe("room/lamp");
      client.subscribe("servo");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void setup() {
  pinMode(lamp, OUTPUT);
  dht.begin();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop())
    client.connect("ESP8266Client");

  now = millis();
  if (now - lastMeasure > 30000) {
    lastMeasure = now;
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    float hic = dht.computeHeatIndex(t, h, false);
    static char temperatureTemp[7];
    dtostrf(hic, 6, 2, temperatureTemp);
    static char humidityTemp[7];
    dtostrf(h, 6, 2, humidityTemp);
    client.publish("room/temperature", temperatureTemp);
    client.publish("room/humidity", humidityTemp);
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t Heat index: ");
    Serial.print(hic);
    Serial.println(" *C ");
    // Serial.print(hif);
    // Serial.println(" *F");
  }
}