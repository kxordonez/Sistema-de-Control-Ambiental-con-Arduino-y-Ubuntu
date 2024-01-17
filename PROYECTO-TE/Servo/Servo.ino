#include <ESP8266WiFi.h>
#include <PubSubClient.h>

///https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <OneWire.h>

#include <Servo.h> 
Servo myservo; 

// Update these with values suitable for your network.

const char* ssid = "xiomylesly";
const char* password = "111111111";
const char* mqtt_server = "192.168.0.228";    ///broker MQTT
 
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {

  Serial.begin(115200);
   myservo.attach(2);
  setup_wifi();
  client.setServer(mqtt_server, 1880);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
   String string;

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
   // Serial.print((char)payload[i]);
     string+=((char)payload[i]);  
  }
  
    Serial.print(string);

if (topic ="servo")   
    Serial.print(" ");
   int resultado = string.toInt();   
   int pos = map(resultado, 1, 100, 0, 180);
   Serial.println(pos);
    myservo.write(pos);
    delay(15); 
   
 }

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {

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
}
void loop() {
  if (!client.connected()) {
    reconnect();

  }
  client.loop();


 delay(100);
  
}