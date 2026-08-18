#include <ZMPT101B.h>

#define N_AMOSTRAS 1000
#define RELE 26
#define PINO_ACS712 34
#define PINO_ZMPT101B 35

ZMPT101B voltageSensor(PINO_ZMPT101B, 60.0);

float calcularCorrenteRMS() {
  float soma = 0;
  for (int i = 0; i < N_AMOSTRAS; i++) {
    float leitura = analogRead(PINO_ACS712);
    float centralizado = leitura - 1324;
    float tensao_mV = centralizado * (3300.0 / 4095.0);
    float corrente = tensao_mV / 92.5;
    soma += corrente * corrente;
  }
  return sqrt(soma / N_AMOSTRAS);
}

void setup() {
  Serial.begin(115200);
  pinMode(RELE, OUTPUT);
  digitalWrite(RELE, HIGH);
  voltageSensor.setSensitivity(621.4); // calibrado com multímetro 128V
}

void loop() {
  Serial.println("RELE ON");
  digitalWrite(RELE, LOW);
  delay(2000);
  
  Serial.println("RELE OFF");
  digitalWrite(RELE, HIGH);
  delay(2000);
}

/*void loop() {
  float tensao = voltageSensor.getRmsVoltage();
  float corrente = calcularCorrenteRMS();
  float potencia = tensao * corrente;
  bool sobrecarga = corrente > 4.5;

  Serial.print("Tensao: ");   Serial.print(tensao);   Serial.println(" V");
  Serial.print("Corrente: "); Serial.print(corrente); Serial.println(" A");
  Serial.print("Potencia: "); Serial.print(potencia); Serial.println(" W");
  Serial.println(sobrecarga ? "RELE ATUANDO" : "RELE EM STANDBY");
  Serial.println("---");

  digitalWrite(RELE, sobrecarga ? LOW : HIGH);
  delay(500);
}*/