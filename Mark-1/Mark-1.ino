#include <ZMPT101B.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define N_AMOSTRAS 1000// Definir porta do sensor...Número de amostras do sensor Hall 
#define rele 26 //Definir porta do rele 
#define SCREEN_WIDTH 128 //Largura da tela OLED
#define SCREEN_HEIGHT 64 //Altura da tela OLED
#define PINO_ACS712 34 //Pino do sensor de corrente 
#define PINO_ZMPT101B 35 //Pino do sensor de tensão 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //Config do display
ZMPT101B voltageSensor(PINO_ZMPT101B, 60.0); // pino 35 frequência 60Hz

float calcularCorrenteRMS(){
  float soma = 0; //Inicializa o acumulador dos quadros das correntes em zero
  //Vai crescer a cada interação com o loop
  for(int i = 0; i < N_AMOSTRAS; i++){ //Repete o bloco N_AMOSTRAS 1000x  e cada interação = uma amostra da onda 
    float leitura = analogRead(PINO_ACS712); //Lê o valor bruto do ADC no pino do ACS712 (0 a 4095)
    float centralizado = leitura - 1324; // Subtrai o offset do ACS712 em repouso 
    //3102 é um offset teórico e pode ser alterado depois do primeiro teste
    float tensao_mV = centralizado * (3300 / 4095.0); //Converte unidades de ADC para milivolts - 3300mV = tensão máxima do ADC do ESP32(3.3V)
    float corrente = tensao_mV / 92.5; //Converte milivolts para amperes - 185mV/A é a sensibilidade do ACS712-05B
    soma += corrente * corrente; //Eleva a corrente ao quadrado e acumula na soma - Quadrado elimina valores negativos da onda AC
  }
  float media = soma / N_AMOSTRAS; // Divide a soma dos quadrados pelo número de amostras e calcula a média dos quadrados 
  float RMS = sqrt(media); //Raiz quadrada da média dos quadrados - Resultado: Corrente RMS em amperes
  return RMS; // Retorna o valor para quem chamou a função 
}

void setup() {
  Serial.begin(115200);
  pinMode(rele, OUTPUT); //Define o modo do relé, output
  Wire.begin(32, 33);
  voltageSensor.setSensitivity(500.0);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){ //Verifica se o display está conectado 
    Serial.println("Display nao encontrado");
    while(1);
  }
  display.clearDisplay();
}

void loop() {
  float tensao = voltageSensor.getRmsVoltage();
  float corrente = calcularCorrenteRMS();
  float potencia = tensao * corrente;
  bool sobrecarga = corrente > 4.5;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Corrente:");
  display.print(corrente);
  display.println("A");
  display.print("Tensao:");
  display.print(tensao);
  display.println("V");
  display.print("Potencia:");
  display.print(potencia);
  display.println("W");
  if(sobrecarga){
    display.println("RELÉ ATUANDO");
    Serial.println("RELÉ ATUANDO");
  }
  else{
    display.println("RELÉ EM STANDBY");
    Serial.println("RELÉ EM STANDBY");
  }
  Serial.print("Corrente:");
  Serial.print(corrente);
  Serial.println("A");
  Serial.print("Tensão:");
  Serial.print(tensao);
  Serial.println("V");
  Serial.print("Potência atual:");
  Serial.print(potencia);
  Serial.println("W");
  display.display();

  if(sobrecarga){
    digitalWrite(rele, LOW); // Estado do relé. Aberto por sobrecarga
  }
  else{
    digitalWrite(rele, HIGH); // Estado do relé. Fechado, carga abaixo do nível de alerta
  }
  delay(500);
}


