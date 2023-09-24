#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "HariWifi";
const char* password = "hari*teju";

// MQTT broker credentials (set to NULL if not required)
const char* MQTT_username = NULL; 
const char* MQTT_password = NULL;

// Change the variable to your MQTT broker IP address
const char* mqtt_server = "broker.hivemq.com";

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient;
PubSubClient client(espClient);
StaticJsonDocument<200> doc;

const int PIN_RED   = 5;
const int PIN_GREEN = 4;
const int PIN_BLUE  = 0;


unsigned long previousMillis = 0;

// This functions connects your ESP8266 to your router
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
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void setColor(int R, int G, int B) {
  analogWrite(PIN_RED,   R);
  analogWrite(PIN_GREEN, G);
  analogWrite(PIN_BLUE,  B);
}

// This function is executed when some device publishes a message to a topic that your ESP8266 is subscribed to.
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  deserializeJson(doc, messageTemp);
  int r = doc["r"];
  int g = doc["g"];
  int b = doc["b"];
  setColor(r, g, b);


  // if statements to control more GPIOs with MQTT

  // // If a message is received on the topic, change the color of the RGB LED.
  // if(topic=="iotfrontier/color"){
  //   Serial.print("Changing Color");

  //   deserializeJson(doc, messageTemp);

  //   int r = doc["r"];
  //   int g = doc["g"];
  //   int b = doc["b"];
  //   setColor(r, g, b);
  // }
  // Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("NodeMCUClient", MQTT_username, MQTT_password)) {
      Serial.println("connected");  
      client.subscribe("iotfrontier/color");  
      Serial.println("Subscribed");  
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client", MQTT_username, MQTT_password);

  unsigned long currentMillis = millis();
  // Publishes new temperature and humidity every 30 seconds
  if (currentMillis - previousMillis > 30000) {
    previousMillis = currentMillis;
    Serial.println("in loop");
  }
} 