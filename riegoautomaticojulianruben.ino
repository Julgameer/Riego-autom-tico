#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <ThingSpeak.h>
// ────────── PINES Y CONSTANTES ──────────
#define RELE 27
#define SENSORH0_SECO 2700
#define SENSORH1_SECO 3050
#define SENSOR0 34
#define SENSOR1 35
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ────────── WIFI Y THINGSPEAK ──────────
const char* ssid = "Redmi12";
const char* password = "00000000";
WiFiClient client;
unsigned long channelID = 3305776;
const char* writeAPIKey = "2BV5O26DUVERRBT4";
unsigned long lastUpdate = 0;
const long interval = 20000;  // 20 segundos

// ────────── SETUP ──────────
void setup() {
  Serial.begin(115200);
  // Configuración de pines
  pinMode(RELE, OUTPUT);
  digitalWrite(RELE, HIGH);
  // Inicializar I2C para OLED
  Wire.begin(21, 22);
  // Inicializar pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("ERROR: pantalla no detectada");
    while (true)
      ;
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("Iniciando...");
  display.display();

  // Conectar WiFi
  WiFi.begin(ssid, password);
  display.setTextSize(1);
  display.println("Conectando WiFi...");
  display.display();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Conectado");
  display.println("WiFi OK");
  display.display();
  // Iniciar ThingSpeak
  ThingSpeak.begin(client);
  delay(2000);
}

// ────────── LOOP ──────────
void loop() {
  // Leer sensores
  int h0 = analogRead(SENSOR0);
  int h1 = analogRead(SENSOR1);
  Serial.print("H0: ");
  Serial.println(h0);
  Serial.print("H1: ");
  Serial.println(h1);
  // Control del relé
  if (h0 > SENSORH0_SECO && h1 > SENSORH1_SECO) {
    digitalWrite(RELE, LOW);  // Encender bomba
  }
  if (h0 < SENSORH0_SECO && h1 < SENSORH1_SECO) {
    digitalWrite(RELE, HIGH);  // Apagar bomba
  }
  // Actualizar pantalla OLED
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("H0:");
  display.println(h0);
  display.print("H1:");
  display.println(h1);
  display.setTextSize(1);
  display.setCursor(0, 50);
  display.print("Tiempo: ");
  display.print(millis() / 1000);
  display.println(" s");
  display.display();
  // Enviar datos a ThingSpeak cada 20 segundos
  if (millis() - lastUpdate > interval) {
    ThingSpeak.setField(1, h0);
    ThingSpeak.setField(2, h1);
    int x = ThingSpeak.writeFields(channelID, writeAPIKey);
    if (x == 200) {
      Serial.println("Datos enviados a ThingSpeak");
    } else {
      Serial.println("Error al enviar datos");
    }
    lastUpdate = millis();
  }
  delay(500);
}
