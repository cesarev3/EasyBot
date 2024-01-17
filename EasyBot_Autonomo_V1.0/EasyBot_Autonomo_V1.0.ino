/*
*******************************************************
 Programa de Controle do EasyBot
 Autor Magneto - Novembro 2016
 ******************************************************
 */

#include <Servo.h> //inclui biblioteca Servo
#include <LiquidCrystal.h> // inclui biblioteca de LCD

LiquidCrystal lcd(25, 27, 29, 31, 33, 35);

// *** Definicao dos pinos para movimento e direcao ***
Servo servoEsquerdo;
Servo servoDireito;
Servo servoTorre;

// Luz do LCD
int ldrValor = 0;
byte ldrOn = 1; // define se a luz do LCD funcionara pelo LDR ou sempre acesa
int ledPWM = 255;
int tempoLuz = 0;
byte ativaLuz = 0;
int tempoLuzBotao = 600;

// Definicao dos pinos
const int botaoPreto = 52;
const int botaoVermelho = 51;
const int botaoVerde = 50;
const int potRight = A11;
const int potLeft = A12;
const int LiPo = A13;
const int LDR = A14;
const int LM35 = A15;
const int trig = 39;
const int echo = 37;
const int trigAbaixo = 38;
const int echoAbaixo = 36;
const int ledVerde = 11;
const int ledVermelho = 12;
const int lcdLed = 4;

// variaveis ultrasom
long duracao; // duracao pulso sensor superior
long distancia; // distancia sensor superior
long duracaoAbaixo; // duracao pulso sensor inferior
long distanciaAbaixo; // distancia sensor inferior
int medida0 = 100;
int medida30 = 100;
int medida45 = 100;
int medidaCentro = 100;
int medida135 = 100;
int medida150 = 100;
int medida180 = 100;
int medidaAbaixo = 0;
byte varreduraCont = 0;
byte desvio = 0;

// variaveis voltagem
float Vout = 0.0;
float Vin = 0.0;
float R1 = 21850.0;
float R2 = 4755.0;
float volt = 0.0;

// *** Definicao das variaveis para movimento e direcao ***
int scaleJoy1 = 0; // escala de torre
int scaleJoy2 = 0; // escala de deslocamento
int scaleJoy3 = 0; // escala de direcao
int scaleIntJoy1 = 0; // escala de torre inteiro
int scaleIntJoy2 = 0; // escala de deslocamento inteiro
int scaleIntJoy3 = 0; // escala de direcao inteiro
int angleLeftJoy1 = 0; // angulo do servo esquerdo para torre
int angleRightJoy1 = 0; // angulo do servo direito para torre
int angleLeftJoy2 = 0; // angulo do servo esquerdo para Up-Down
int angleRightJoy2 = 0; // angulo do servo direito para Up-Down
int angleLeftJoy3 = 0; // angulo do servo esquerdo para Left-Right
int angleRightJoy3 = 0; // angulo do servo direito para Left-Right
int trimJoy1 = 36; // trimagem do joystick 1 conforme programa de teste do TX
int trimJoy2 = 98; // trimagem do joystick 2 conforme programa de teste do TX
int trimJoy3 = 157; // trimagem do joystick 3 conforme programa de teste do TX
byte angleLeft = 82; // servo esquerdo na posicao central
byte angleRight = 80; // servo direito na posicao central
byte angleTower = 93; // servo torre na posicao central
//int trimServo = 0; // trimagem dos servos esquerdo e direito
byte centroServoLeft = 82; // definicao centro servo esquerdo
byte centroServoRight = 80; // definicao centro servo direito
//byte trimRight = 0;
//byte trimLeft = 0;
int valorPotLeft = 0;
int valorPotRight = 0;

// *** Definicao de outras variaveis ***
int incomingByte; // Variavel de entrada de dados pela Serial
int scan = 0; // contagem do movimento de scanner tipo supermaquina
int scanInverso = 0; // contagem do movimento de scanner tipo supermaquina

/**********************************\
 **                               **
 ** Definindo Setup dos programas **
 **                               **
 \*********************************/

void setup() { // Declaracoes

  lcd.begin(16, 2);

  // inicializa Serial
  Serial.begin(9600);
  Serial1.begin(9600);

  // inicialisar pinos
  servoEsquerdo.attach(5);
  servoDireito.attach(6);
  servoTorre.attach(7);
  pinMode(ledVerde,OUTPUT);
  pinMode(ledVermelho,OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(trigAbaixo, OUTPUT);
  pinMode(echoAbaixo, INPUT);
  pinMode (botaoPreto, INPUT);
  pinMode (botaoVermelho, INPUT);
  pinMode (botaoVerde, INPUT);
  pinMode(lcdLed, OUTPUT);

  // posiciona servos
  servoEsquerdo.write(82);
  servoDireito.write(80);
  servoTorre.write(93);

  // Acende backlight e dados do LCD 
  analogWrite(lcdLed, ledPWM); 

  // tela de introducao LCD
  lcd.begin(16, 2);
  lcd.setCursor(0,0);   
  lcd.print(" EasyBot Clock ");   
  lcd.setCursor(0,1);   
  lcd.print("  com Arduino");   
  delay (2500); 
  lcd.clear();
  lcd.setCursor(0,0);   
  lcd.print("   Versao 1.8");
  lcd.setCursor(0,1);   
  lcd.print("   by Magneto");   
  delay (3000); 
  lcd.clear();
  parado();
}


/**********************************\
 **                               **
 ** Definindo Rotinas do programa **
 **                               **
 \*********************************/


// ************** Inicio Rotinas Comuns **************** 

void imprimirServos() { // imprimir angulo dos servos

  lcd.setCursor(0,0);
  lcd.print("SD: ");
  lcd.print(servoDireito.read());
  lcd.print("  SE: ");
  lcd.print(servoEsquerdo.read());
  lcd.print("  ");
  
}

void medirDistancia() { // Leitura do ultrasom

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duracao = pulseIn(echo, HIGH); 
  distancia = duracao / 56;

}

void medirDistanciaAbaixo() { // Leitura do ultrasom de baixo

  digitalWrite(trigAbaixo, LOW);
  delayMicroseconds(2);
  digitalWrite(trigAbaixo, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigAbaixo, LOW);
  duracaoAbaixo = pulseIn(echoAbaixo, HIGH); 
  distanciaAbaixo = duracaoAbaixo / 56;

}

void voltagemBateria () { // Voltagem da bateria
  volt = analogRead(LiPo);

  // calculando voltagem de entrada
  Vout = (volt * 5.0) / 1023.0;
  Vin = Vout / (R2/(R1+R2));
}

void imprimindoDistancia () { // imprimindo valor distancia
  lcd.setCursor(0,0);
  lcd.print("Distancia:      ");
  if(distancia > 99) {
    lcd.setCursor(11,0);
    lcd.print(distancia);
  }
  else {
    lcd.setCursor(11,0);
    lcd.print(" ");
    lcd.setCursor(12,0);
    lcd.print(distancia);
  }
  lcd.setCursor(14,0);
  lcd.print("cm");
  lcd.setCursor(0,1);
  lcd.print("Traseira:      ");
  if(distancia > 99) {
    lcd.setCursor(11,1);
    lcd.print(distancia);
  }
  else {
    lcd.setCursor(11,1);
    lcd.print(" ");
    lcd.setCursor(12,1);
    lcd.print(distancia + 14);
  }
  lcd.setCursor(14,1);
  lcd.print("cm");
}

void imprimindoDistanciaAbaixo () { // imprimindo valor distancia
  lcd.setCursor(0,0);
  lcd.print("Distancia:      ");
  if(distanciaAbaixo > 99) {
    lcd.setCursor(11,0);
    lcd.print(distanciaAbaixo);
  }
  else {
    lcd.setCursor(11,0);
    lcd.print(" ");
    lcd.setCursor(12,0);
    lcd.print(distanciaAbaixo);
  }
  lcd.setCursor(14,0);
  lcd.print("cm");
  lcd.setCursor(0,1);
  lcd.print("Traseira:      ");
  if(distanciaAbaixo > 99) {
    lcd.setCursor(11,1);
    lcd.print(distanciaAbaixo);
  }
  else {
    lcd.setCursor(11,1);
    lcd.print(" ");
    lcd.setCursor(12,1);
    lcd.print(distanciaAbaixo + 14);
  }
  lcd.setCursor(14,1);
  lcd.print("cm");
}

void imprimindoVoltagem () { // imprimindo valor voltagem
  //lcd.setCursor(0,1);
  //lcd.print("Bateria:       ");
  if(Vin <= 9.3) {
    lcd.setCursor(8,1);
    lcd.print("LW ");
  }
  else {
    lcd.setCursor(8,1);
    lcd.print("   ");
  }
  lcd.setCursor(11,1);
  lcd.print(Vin);
  lcd.setCursor(15,1);
  lcd.print("V ");
}

void frenteApenas () { // deslocamento apenas para frente
  servoDireito.write(10);
  servoEsquerdo.write(180);
}

void atrasApenas () { // deslocamento apenas para tras
  servoDireito.write(180);
  servoEsquerdo.write(0);
}

void giroEsquerda () { // girar no eixo para esquerda
  servoDireito.write(0);
  servoEsquerdo.write(0);  
}

void giroDireita () { // girar no eixo para direita
  servoDireito.write(180);
  servoEsquerdo.write(180);  
}

void frenteEsquerda () { // deslocamento para frente e esquerda
  servoDireito.write(0);
  servoEsquerdo.write(centroServoLeft);
}

void frenteDireita () { // deslocamento para frente e direita
  servoDireito.write(centroServoRight);
  servoEsquerdo.write(180);
}

void atrasEsquerda () { // deslocamento para tras e esquerda
  servoDireito.write(180);
  servoEsquerdo.write(centroServoLeft);
}

void atrasDireita () { // deslocamento para tras e direita
  servoDireito.write(centroServoRight);
  servoEsquerdo.write(0);
}

void parado () { // sem deslocamento
  servoDireito.write(centroServoRight);
  servoEsquerdo.write(centroServoLeft);
}

// Controle de movimento autonomo da torre
void torreCentro () { // torre no centro
  servoTorre.write(93);
}

void torre45 () { // torre para esquerda 45 graus
  servoTorre.write(45);
}

void torre30 () { // torre para esquerda 30 graus
  servoTorre.write(30);
}

void torre0 () { // torre para esquerda 90 graus
  servoTorre.write(0);
}

void torre135 () { // torre para direita 45 graus
  servoTorre.write(135);
}

void torre150 () { // torre para direita 45 graus
  servoTorre.write(150);
}

void torre180 () { // torre para direita 90 graus
  servoTorre.write(180);
}

void varreduraInicial() { // varredura inicial
  torreCentro();
  delay(300);
  medirDistancia();
  delay(200);
  medidaCentro = distancia;
  medirDistanciaAbaixo();
  delay(200);
}

void varredura0() { // varredura de distancia 0 graus
  torre0();
  delay(300);
  medirDistancia();
  medida0 = distancia;
}

void varredura30() { // varredura de distancia 30 graus
  torre30();
  delay(300);
  medirDistancia();
  medida30 = distancia;
}

void varredura150() { // varredura de distancia 150 graus
  torre150();
  delay(300);
  medirDistancia();
  medida150 = distancia;
}

void varredura180() { // varredura de distancia 180 graus
  torre180();
  delay(300);
  medirDistancia();
  medida180 = distancia;
}

void varreduraParado() { // varredura de distancia
  torre180();
  delay(300);
  medirDistancia();
  medida180 = distancia;
  delay(200);

  torre0();
  delay(300);
  medirDistancia();
  medida0 = distancia;
  delay(200);

  torreCentro();
  delay(300);
  medirDistancia();
  medidaCentro = distancia;
  delay(200);
  medirDistanciaAbaixo();
  delay(200);
}

void scanner() { // modo Kitty
  if(scanInverso == 0) {
    scan ++;
    if(scan > 5) {
      scanInverso = 1;
    }
  } 
  if(scanInverso == 1) {
    scan --;
    if(scan < 2) {
      scanInverso = 0;
    }
  } 
  lcd.setCursor(7,1);
  lcd.print(" ");
  lcd.setCursor(scan,1);
  lcd.print("I");
  if(scan > 0) {
    lcd.setCursor(scan - 1,1);
    lcd.print("_");
  }
  if(scan < 6) {
    lcd.setCursor(scan + 1,1);
    lcd.print("_");
  }
}

// ************** Final Rotinas Comuns ****************




// ************** Inicio Rotina Modo Autonomo ****************

void modoAutonomo () { // modo Autonomo

  voltagemBateria(); // ler voltagem
  imprimirServos();
  scanner();
  imprimindoVoltagem();

  if(varreduraCont > 29) {
    varreduraCont = 0;
  }

  if(varreduraCont == 0 || varreduraCont == 7 || varreduraCont == 14 || varreduraCont == 21) {
    varreduraInicial();
  }

  if(varreduraCont == 5 || varreduraCont == 12 || varreduraCont == 19 || varreduraCont == 26) {
    varredura30();
  }

  if(varreduraCont == 3 || varreduraCont == 10 || varreduraCont == 17 || varreduraCont == 24) {
    varredura150();
  }

  if(medidaCentro < 20 || distanciaAbaixo < 30) {
    parado();
    imprimirServos();
    atrasApenas();
    imprimirServos();
    delay(1200);
    parado();
    imprimirServos();
    varreduraParado();
    varreduraCont = -1;
    desvio = 1;
  }

  if(desvio == 1 && medida0 > 19 && medida180 > 19 && medida0 > medida180) {
    giroEsquerda();
    imprimirServos();
    delay(650);
    parado();
    imprimirServos();
    medida0 = 100;
    medida180 = 100;
    desvio = 0;
  }

  if(desvio == 1 && medida0 > 19 && medida180 > 19 && medida180 > medida0) {
    giroDireita();
    imprimirServos();
    delay(650);
    parado();
    imprimirServos();
    medida0 = 100;
    medida180 = 100;
    desvio = 0;
  }

  if(desvio == 1 && medida0 > 19 && medida180 < 20) {
    frenteEsquerda();
    imprimirServos();
    delay(1400);
    parado();
    imprimirServos();
    medida0 = 100;
    medida180 = 100;
    desvio = 0;
  }

  if(desvio == 1 && medida0 < 20 && medida180 > 19) {
    frenteDireita();
    imprimirServos();
    delay(1400);
    parado();
    imprimirServos();
    medida0 = 100;
    medida180 = 100;
  }

  if(desvio == 1 && medida0 < 20 && medida180 < 20) {
    atrasApenas();
    imprimirServos();
    delay(1000);
    parado();
    imprimirServos();
    varreduraParado();
  }

  if(desvio == 0 && medidaCentro > 19 && distanciaAbaixo > 29) {
    frenteApenas();
    medirDistanciaAbaixo();

    if(desvio == 0 && medida30 < 40 && distanciaAbaixo > 29) {
      frenteDireita();
      imprimirServos();
      delay(300);
      frenteApenas();
      imprimirServos();
      medida30 = 100;
    }
    if(desvio == 0 && medida150 < 40 && distanciaAbaixo > 29) {
      frenteEsquerda();
      imprimirServos();
      delay(300);
      frenteApenas();
      imprimirServos();
      medida150 = 100;
    }
  }

  varreduraCont ++;
}

// ************** Final Rotina Modo Autonomo ****************




/**********************************\
 **                               **
 **  Definindo Loop do programa   **
 **                               **
 \*********************************/

void loop() {

 
      modoAutonomo();
 
}








/*

 
 // degub na tela via USB
 //  debugnatela();
 // Serial.print("Joy 2 Esquerdo: ");
 // Serial.println(angleLeftJoy2);
 // Serial.print("Joy 2 Direito: ");
 // Serial.println(angleRightJoy2);
 // Serial.print("Joy 1 Torre: ");
 // Serial.println(scaleJoy1);
 // Serial.print(" Valor LDR: ");
 // Serial.println(ldrValor);
 // Serial.print(" PWM LDR: ");
 // Serial.println(ledPWM);
 // delay(300);
 
 
 
/*
 // temporizacao das setas 
 unsigned long currentMillis = millis();
 
 if(currentMillis - previousMillis > interval) {
 // save the last time you blinked the LED 
 previousMillis = currentMillis;   
 
 // if the LED is off turn it on and vice-versa:
 
 if (valorFrente == LOW && valorAtras == 0) {
 valorFrente = HIGH;
 valorAtras = 255;
 }
 else if(valorFrente == HIGH && valorAtras == 255) {
 valorFrente = LOW;
 valorAtras = 0;
 }
 
 if(piscaEsquerdo == 0 && piscaDireito == 0 && piscaAlerta == 0) {
 valorFrente = LOW;
 valorAtras = 0;
 ligaPiscaEsquerdo();
 ligaPiscaDireito();
 }
 
 }
 
/* Disposicao dos botoes e comandos
 
 j1LeftRight = 8 a 67
 j2UpDown = 68 a 127 
 j3LeftRight = 128 a 187
 j4UpDown = 188 a 247
 joyBLeft = 1 
 joyBRight = 2 
 frontTopLeft = 3
 frontBotLeft = 4
 frontTopRight = 5 
 frontBotRight = 6
 upperLeft = 248;
 upperRight = 249;
 upperTop = 250;
 upperDown = 251;
 
 Potenciometro de direcao
 Esquerda: 720/759
 Direita: 345/377
 Centro: 548/576
 
 *****************************
 **  Limites dos Joysticks  **
 **  J1 Left: 8             **
 **  J1 Center: 36          **
 **  J1 Right: 67           **
 **                         **
 **  J2 Up: 68              **
 **  J2 Center: 98          **
 **  J2 Down: 127           **
 **                         **
 **  J3 Left: 128           **
 **  J3 Center: 157         **
 **  J3 Right: 187          **
 **                         **
 **  J4 Up: 188             **
 **  J4 Center: 217         **
 **  J4 Dowm: 247           **
 **                         **
 **  Servo Esquerdo         **
 **  Back: 0 > to < 81      **
 **  Center: 82             **
 **  Foward: 83 > to < 180  **
 **                         **
 **  Servo Direito          **
 **  Back: 81 > to < 180    **
 **  Center: 80             **
 **  Foward: 0 > to < 79    **
 **                         **
 **  Servo Torre            **
 **  Left: 13 > to < 80     **
 **  Left: 86 > to < 165 (old)
 **  Center: 81             **
 **  Right: 82 > to < 165   **
 **  Right: 13 > to < 84 (old)
 **                         **
 *****************************
 
 */

























































