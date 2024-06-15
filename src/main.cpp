#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Configurações Wi-Fi
const char* ssid = "Vuoi un pezzetto di formaggio?";
const char* password = "vabenecosi@2908";

// Configuração do endpoint do backend
const char* serverName = "http://172.16.1.125:3000/api/sensor";

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

  // Envia os dados para o backend
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    // Dados do sensor a serem enviados
    String httpRequestData = "{\"moisture\":" + String(moisturePercent) + "}";

    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Erro ao enviar o POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Erro na conexão WiFi");
  }

  // Aguarda 5 min segundo antes da próxima leitura
  delay(300000);
}
