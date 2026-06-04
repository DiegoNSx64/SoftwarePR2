#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "UPV-PSK";
const char* password = "giirob-pr2-2023";
const char* mqtt_server = "mqtt.dsic.upv.es";
const int mqtt_port = 1883;
const char* mqtt_user = "giirob";
const char* mqtt_pass = "UPV2024";

#define PIN_BOTON        4     
#define PIN_TRIG         12
#define PIN_ECHO         13

const float DISTANCIA_UMBRAL = 10.0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_BOTON, INPUT_PULLUP);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void setup_wifi() {
  delay(10);
  Serial.println("\nConectando a WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print(".");
  }
  Serial.println("\n¡WiFi Conectado!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");

    if (client.connect("ESP32_Seguridad_S2", mqtt_user, mqtt_pass)) {
      Serial.println(" ¡Conectado con éxito al Broker de la UPV!");
      
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

  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  
 
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  

  long duracion = pulseIn(PIN_ECHO, HIGH, 30000);
  

  float distanciaCm = duracion * 0.0343 / 2.0;

-
  int presencia = 0;
  if (distanciaCm > 0 && distanciaCm < DISTANCIA_UMBRAL) {
    presencia = 1; 
  }
  int boton = (digitalRead(PIN_BOTON) == LOW) ? 1 : 0;


  StaticJsonDocument<256> doc;
  doc["placa_procesada"] = presencia; 
  doc["parada_emergencia"] = boton;

  char buffer[256];
  serializeJson(doc, buffer);
  

  client.publish("incubadora/1/101", buffer);
  

  Serial.print("Datos enviados: ");
  Serial.println(buffer);
  
  delay(2000);
}
