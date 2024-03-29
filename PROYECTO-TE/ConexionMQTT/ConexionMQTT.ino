#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#define DHT_PIN 0
#define DHT_TIPO DHT22

DHT dht(DHT_PIN, DHT_TIPO);
WiFiClient espClient;
PubSubClient client(espClient);

char msg[16];

void setup() {
  dht.begin();
  setup_wifi();
  client.setServer("192.168.0.228", 1883);
  pinMode(0, OUTPUT);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  //----------------------------------
  delay(2000);
  int temperatura = dht.readTemperature();
  int humedad = dht.readHumidity();
  //----------------------------------

  snprintf(msg, 16, "%d,%d", temperatura, humedad);

  client.publish("canal", msg);
  delay(1000);
}

void reconnect() {

  if (client.connect("ESP32")) {
    Serial.println(".....Conexion exitosa");
  }
}

void setup_wifi() {

  Serial.begin(9600);
  WiFi.begin("xiomylesly", "111111111");  //se habilitará el modo estación

  // Mientras el ESP32 no se conecte al AP:
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP()); //Imprimo el IP del esp32
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String val = String((char*)payload);
}