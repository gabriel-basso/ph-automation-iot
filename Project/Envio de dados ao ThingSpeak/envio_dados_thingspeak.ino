#include <WiFi.h>
#include "ThingSpeak.h" 

// =================================================================
// ÁREA DE CONFIGURAÇÃO (PREENCHA APENAS O WI-FI)
// =================================================================

// 1. DADOS DO WI-FI (Preencha com cuidado, mantendo as aspas)
const char* ssid = "Casa_Elen";     // <--- COLOCAR SEU WI-FI AQUI
const char* password = "01051954"; // <--- COLOCAR SUA SENHA AQUI

// 2. DADOS DO THINGSPEAK (Já preenchidos com os dados que você enviou)
unsigned long myChannelNumber = 3191626;
const char * myWriteAPIKey = "NXG3KGT8CT9SADVJ";

// 3. CALIBRAÇÃO DO SENSOR (Seus valores medidos)
float voltagePH7 = 0.756; 
float voltagePH4 = 0.941; 

// =================================================================

WiFiClient client;
const int phPin = 36; 
float m, b; 

void setup() {
  Serial.begin(115200);
  
  // Configura hardware
  pinMode(phPin, INPUT);
  analogReadResolution(12);

  // Calcula a curva de pH automaticamente
  m = (4.0 - 7.0) / (voltagePH4 - voltagePH7);
  b = 7.0 - (m * voltagePH7);

  // Inicia Wi-Fi
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client); 
  
  Serial.println("--------------------------------------");
  Serial.println("Iniciando Medidor de pH IoT");
  Serial.print("Slope (m): "); Serial.print(m);
  Serial.print(" | Intercept (b): "); Serial.println(b);
  Serial.println("--------------------------------------");
}

void loop() {
  // 1. VERIFICA CONEXÃO WI-FI
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Conectando ao WiFi: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, password);
      Serial.print(".");
      delay(5000);
    } 
    Serial.println("\nWiFi Conectado!");
  }

  // 2. LEITURA DO SENSOR (Média de 20 amostras)
  unsigned long reading = 0;
  for(int i = 0; i < 20; i++){
    reading += analogRead(phPin);
    delay(10);
  }
  float averageADC = reading / 20.0;
  float currentVoltage = (averageADC / 4095.0) * 3.3;

  // 3. CÁLCULO DO pH
  float phValue = m * currentVoltage + b;

  // 4. MOSTRA NO SERIAL (Diagnóstico)
  Serial.print("Lendo... Voltagem: ");
  Serial.print(currentVoltage, 3);
  Serial.print("V | pH Calculado: ");
  Serial.println(phValue, 2);

  // 5. ENVIA PARA A NUVEM (ThingSpeak)
  // Campo 1: pH
  // Campo 2: Voltagem (bom para saber se o sensor desconectou)
  ThingSpeak.setField(1, phValue);       
  ThingSpeak.setField(2, currentVoltage); 

  Serial.println("Enviando dados para o ThingSpeak...");
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if(x == 200){
    Serial.println(">> SUCESSO: Dados plotados no grafico!");
  }
  else{
    Serial.println(">> ERRO DE ENVIO. Codigo HTTP: " + String(x));
  }

  // AGUARDA 20 SEGUNDOS (Limite do ThingSpeak Gratuito)
  // Se diminuir isso, os dados serão bloqueados.
  Serial.println("Aguardando 20s para o proximo envio...");
  delay(20000); 
}