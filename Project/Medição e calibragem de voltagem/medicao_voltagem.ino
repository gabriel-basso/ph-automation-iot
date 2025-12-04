/*
 * CÓDIGO DE CALIBRAÇÃO - ESP32 + pH
 * Objetivo: Ler a voltagem para ajustar o potenciômetro e calibrar
 */

const int phPin = 36; // GPIO 36 (VP)

void setup() {
  Serial.begin(115200);
  pinMode(phPin, INPUT);
  analogReadResolution(12); // Resolução de 12 bits
}

void loop() {
  // Faz 50 leituras rápidas para estabilizar o valor
  long reading = 0;
  for(int i = 0; i < 50; i++){
    reading += analogRead(phPin);
    delay(2);
  }
  float averageADC = reading / 50.0;
  
  // Converte para voltagem (considerando ESP32 3.3V)
  float voltage = (averageADC / 4095.0) * 3.3;

  Serial.print("Leitura ADC: ");
  Serial.print(averageADC);
  Serial.print(" | Voltagem Chegando no ESP32: ");
  Serial.print(voltage);
  Serial.println(" V");

  delay(500);
}