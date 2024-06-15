#include <Arduino.h>
#include <WiFi.h>

// Configurações Wi-Fi
const char* ssid = "Vuoi un pezzetto di formaggio?";
const char* password = "vabenecosi@2908";

// Pino de leitura do sensor
const int soilMoisturePin = 36; // GPIO36 (VP)

// Valores de calibração (ajuste após calibração)
const int AirValue = 3780; // Valor típico quando o sensor está no ar
const int WaterValue = 440;  // Valor típico quando o sensor está na água

// Intervalo de umidade
int intervals = (AirValue - WaterValue) / 3;
int soilMoistureValue = 0;

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200);

  // Conecta ao Wi-Fi
  Serial.print("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado ao Wi-Fi!");

  // Configura o pino de leitura
  pinMode(soilMoisturePin, INPUT);
  analogReadResolution(12); // Define a resolução do ADC para 12 bits
}

void loop() {
  // Lê o valor do sensor
  soilMoistureValue = analogRead(soilMoisturePin);

  // Converte o valor para porcentagem de umidade
  int moisturePercent = map(soilMoistureValue, WaterValue, AirValue, 100, 0);
  moisturePercent = constrain(moisturePercent, 0, 100); // Garante que o valor esteja entre 0 e 100

  // Determina a condição de umidade
  String condition;
  if (soilMoistureValue <= WaterValue + intervals) {
    condition = "Muito Úmido";
  } else if (soilMoistureValue <= AirValue - intervals) {
    condition = "Úmido";
  } else {
    condition = "Seco";
  }

  // Imprime os valores no console
  Serial.print("Umidade do Solo: ");
  Serial.print(soilMoistureValue);
  Serial.print(" (");
  Serial.print(moisturePercent);
  Serial.print("%) - ");
  Serial.println(condition);

  // Aguarda 1 segundo antes da próxima leitura
  delay(1000);
}
