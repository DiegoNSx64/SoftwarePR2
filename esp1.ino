#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "UPV-PSK";
const char* password = "giirob-pr2-2023";
const char* mqtt_server = "mqtt.dsic.upv.es";
const int mqtt_port = 1883;
const char* mqtt_user = "giirob";
const char* mqtt_pass = "UPV2024";

#define PIN_TEMP_ANALOG 4
#define PIN_VIBRACION   5   
#define PIN_CO2         34 
#define PIN_O2          35  

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  
  // Inicializar la red WiFi de la escuela
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void setup_wifi() {
  delay(10);
  Serial.println("\nConectando ESP1 a WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print(".");
  }
  Serial.println("\n¡WiFi Conectado en ESP1!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("ESP1 intentando conexión MQTT...");
    if (client.connect("ESP32_Ambiente_S1", mqtt_user, mqtt_pass)) {
      Serial.println(" ¡Conectada con éxito al Broker!");
    } else { 
      Serial.print("Falló con estado ");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos...");
      delay(5000); 
    }
  }
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();  

  float t = analogRead(PIN_TEMP_ANALOG) * (100.0 / 4095.0);
  float v = analogRead(PIN_VIBRACION) * (5.0 / 4095.0);
  int valorCO2 = analogRead(PIN_CO2); 
  float valorO2 = analogRead(PIN_O2) * (25.0 / 4095.0);

  StaticJsonDocument<256> doc;
  doc["temperatura"] = t;
  doc["vibracion"] = v;
  doc["co2"] = valorCO2;
  doc["o2"] = valorO2;

  char buffer[256];
  serializeJson(doc, buffer);
  
  client.publish("incubadora/1/101", buffer);
  
  delay(2000);
}
