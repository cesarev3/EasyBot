/*
*******************************************************
 Programa de recepcao de comandos Via XBee - DualShock
 Controle do EasyBot
 Autor Magneto - Outubro 2016
 *******************************************************
 */

#include <Servo.h> //inclui biblioteca Servo
#include <LiquidCrystal.h> // inclui biblioteca de LCD

LiquidCrystal lcd(25, 27, 29, 31, 33, 35);

// *** Definicao dos pinos para movimento e direcao ***
Servo servoEsquerdo;
Servo servoDireito;
Servo servoTorre;
const int trig = 39;
const int echo = 37;
const int ledVerde = 11;
const int ledVermelho = 12;
const int brilhoLCD = 4;

long duracao;
long distancia;
int samples = 6;
int somaDistancia [5];
int mediaDistancia = 0;

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
byte trimJoy1 = 36; // trimagem do joystick 1 conforme programa de teste do TX
byte trimJoy2 = 98; // trimagem do joystick 2 conforme programa de teste do TX
byte trimJoy3 = 157; // trimagem do joystick 3 conforme programa de teste do TX
int angleLeft = 82; // servo esquerdo na posicao central
int angleRight = 80; // servo direito na posicao central
int angleTower = 85; // servo torre na posicao central

// *** Definicao de outras variaveis ***
int incomingByte; // Variavel de entrada de dados pela Serial
long previousMillis = 0; // will store last time LED was updated
long interval = 100;  // interval at which to blink (milliseconds)


void setup() {

  lcd.begin(16, 2);

  // inicializa Serial
  Serial.begin(9600);
  Serial1.begin(9600);



  // Inicializa pinos
  //  servo1.attach(4);
  servoEsquerdo.attach(5);
  servoDireito.attach(6);
  servoTorre.attach(7);
  pinMode(ledVerde,OUTPUT);
  pinMode(ledVermelho,OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(brilhoLCD, OUTPUT);

  servoEsquerdo.write(82);
  servoDireito.write(80);
  servoTorre.write(85);
}

void loop() {

  analogWrite(brilhoLCD, 255);
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duracao = pulseIn(echo, HIGH);
  distancia = duracao / 58;

  if(distancia > 15) {
    digitalWrite(ledVermelho,HIGH);
    digitalWrite(ledVerde,LOW);
  }
  else if(distancia <= 15) {
    digitalWrite(ledVerde, HIGH);
    digitalWrite(ledVermelho, LOW);
  }

  lcd.setCursor(0,0);
  lcd.print("Distancia:      ");
  lcd.setCursor(11,0);
  lcd.print(distancia);

  // Verificando se tem entrada de dados na Serial
  if (Serial1.available() > 0) {
    incomingByte = Serial1.read(); // Lendo o ultimo dado recebido
    /*
    // Identificar J2UpDown - Joystick da Esquerda
     if(incomingByte >= 68 && incomingByte <= 127) {
     scaleJoy2 = (incomingByte - trimJoy2);
     
     if(scaleJoy2 > 0) { // para tras
     angleLeftJoy2 = map(scaleJoy2, 1, 30, 81, 0);
     angleRightJoy2 = map(scaleJoy2, 1, 30, 81, 180);
     angleLeft = angleLeftJoy2;
     angleRight = angleRightJoy2;
     }
     else if(scaleJoy2 < 0) { // para frente
     scaleJoy2 = scaleJoy2 * -1;
     angleLeftJoy2 = map(scaleJoy2, 1, 30, 83, 180);
     angleRightJoy2 = map(scaleJoy2, 1, 30, 79, 0);
     angleLeft = angleLeftJoy2;
     angleRight = angleRightJoy2;
     }
     else if(scaleJoy2 == 0 && scaleJoy3 == 0) { // parado
     angleLeftJoy2 = 82;
     angleRightJoy2 = 80;
     angleLeft = angleLeftJoy2;
     angleRight = angleRightJoy2;
     }
     
     }
     
     // Identificar J3LeftRight - joystick da Direita
     if(incomingByte >= 128 && incomingByte <= 187) {
     scaleJoy3 = (incomingByte - trimJoy3);
     
     if(scaleJoy3 > 0) { // para direita
     angleLeftJoy3 = map(scaleJoy3, 1, 30, 83, 180);        
     //       angleRightJoy3 = map(scaleJoy3, 1, 30, 81, 180);
     angleLeft = angleLeftJoy3;
     }
     else if(scaleJoy3 < 0) { // para esquerda
     scaleJoy3 = scaleJoy3 * -1;
     //       angleLeftJoy3 = map(scaleJoy3, 1, 30, 83, 180);
     angleRightJoy3 = map(scaleJoy3, 1, 30, 79, 0);
     angleRight = angleRightJoy3;
     }
     else if(scaleJoy3 == 0 && scaleJoy2 == 0) { // parado
     angleLeftJoy3 = 82;
     angleRightJoy3 = 80;
     angleLeft = angleLeftJoy3;
     angleRight = angleRightJoy3;
     }
     
     }
     
     servoEsquerdo.write(angleLeft);
     servoDireito.write(angleRight);
     */

// Identificar J1LeftRight - Joystick da Esquerda
    if(incomingByte >= 8 && incomingByte <= 67) {
      scaleJoy1 = (incomingByte - trimJoy1);
    }
    
    // Identificar J2UpDown - Joystick da Esquerda
    if(incomingByte >= 68 && incomingByte <= 127) {
      scaleJoy2 = (incomingByte - trimJoy2);
    }

    // Identificar J3LeftRight - joystick da Direita
    if(incomingByte >= 128 && incomingByte <= 187) {
      scaleJoy3 = (incomingByte - trimJoy3);
    }

    if(scaleJoy2 > 0 && scaleJoy3 == 0) { // para tras apenas
      scaleIntJoy2 = scaleJoy2;
      angleLeftJoy2 = map(scaleIntJoy2, 1, 30, 81, 0);
      angleRightJoy2 = map(scaleIntJoy2, 1, 30, 81, 180);
      angleLeft = angleLeftJoy2;
      angleRight = angleRightJoy2;
    }
    else if(scaleJoy2 > 0 && scaleJoy3 < 0) { // para tras e para esquerda
      scaleIntJoy2 = scaleJoy2;
      scaleIntJoy3 = scaleJoy3 * -1;
      angleLeftJoy3 = map(scaleIntJoy3, 1, 30, 0, 81);
      angleRightJoy2 = map(scaleIntJoy2, 1, 30, 81, 180);
      angleLeft = angleLeftJoy3;
      angleRight = angleRightJoy2;
    }
    else if(scaleJoy2 > 0 && scaleJoy3 > 0) { // para tras e para direita
      scaleIntJoy2 = scaleJoy2;
      scaleIntJoy3 = scaleJoy3;
      angleLeftJoy2 = map(scaleIntJoy2, 1, 30, 81, 0);
      angleRightJoy3 = map(scaleIntJoy3, 1, 30, 180, 83);
      angleLeft = angleLeftJoy2;
      angleRight = angleRightJoy3;
    }
    else if(scaleJoy2 < 0 && scaleJoy3 == 0) { // para frente apenas
      scaleIntJoy2 = scaleJoy2 * -1;
      angleLeftJoy2 = map(scaleIntJoy2, 1, 30, 83, 180);
      angleRightJoy2 = map(scaleIntJoy2, 1, 30, 79, 0);
      angleLeft = angleLeftJoy2;
      angleRight = angleRightJoy2;
    }
    else if(scaleJoy2 < 0 && scaleJoy3 < 0) { // para frente e para esquerda
      scaleIntJoy2 = scaleJoy2 * -1;
      scaleIntJoy3 = scaleJoy3 * -1;
      angleLeftJoy3 = map(scaleIntJoy3, 1, 30, 180, 83);
      angleRightJoy2 = map(scaleIntJoy2, 1, 30, 79, 0);
      angleLeft = angleLeftJoy3;
      angleRight = angleRightJoy2;
    }
    else if(scaleJoy2 < 0 && scaleJoy3 > 0) { // para frente e para direita
      scaleIntJoy2 = scaleJoy2 * -1;
      scaleIntJoy3 = scaleJoy3;
      angleLeftJoy2 = map(scaleIntJoy2, 1, 30, 83, 180);
      angleRightJoy3 = map(scaleIntJoy3, 1, 30, 0, 79);
      angleLeft = angleLeftJoy2;
      angleRight = angleRightJoy3;
    }
    else if(scaleJoy2 == 0 && scaleJoy3 < 0) { // parado e para esquerda
      scaleIntJoy3 = scaleJoy3 * -1;
      angleLeftJoy2 = 82;
      angleRightJoy3 = map(scaleIntJoy3, 1, 30, 79, 0);
      angleRight = angleRightJoy3;
      angleLeft = angleLeftJoy2;
    }
    else if(scaleJoy2 == 0 && scaleJoy3 > 0) { // parado e para direita
      angleLeftJoy3 = map(scaleIntJoy3, 1, 30, 83, 180);
      angleRightJoy2 = 80;
      angleLeft = angleLeftJoy3;
    }
    else if(scaleJoy2 == 0 && scaleJoy3 == 0) { // parado
      angleLeftJoy2 = 82;
      angleRightJoy2 = 80;
      angleLeft = angleLeftJoy2;
      angleRight = angleRightJoy2;
    }
    
    if(scaleJoy1 < 0) { // torre para esquerda
      scaleIntJoy1 = scaleJoy1 * -1;
      angleLeftJoy1 = map(scaleIntJoy1, 1, 30, 86, 165);
      angleTower = angleLeftJoy1;
    }
    else if(scaleJoy1 > 0) { // torre para direita
      scaleIntJoy1 = scaleJoy1;
      angleRightJoy1 = map(scaleIntJoy1, 1, 30, 84, 13);
      angleTower = angleRightJoy1;
    }
    else if(scaleJoy1 == 0) { // torre para centro
      angleTower = 85;
    }

  }

  servoEsquerdo.write(angleLeft);
  servoDireito.write(angleRight);
  servoTorre.write(angleTower);

  // Serial.print("Joy 2 Esquerdo: ");
  // Serial.println(angleLeftJoy2);
  // Serial.print("Joy 2 Direito: ");
  // Serial.println(angleRightJoy2);
  // Serial.print("Joy 1 Torre: ");
  // Serial.println(scaleJoy1);
  //delay(300);
}  
/*
 
 // ligar farol Alto
 if(incomingByte == 3) 
 servoTorre.write(0);
 
 // Desligar Farol Alto
 if(incomingByte == 4) 
 servoTorre.write(180);
 
 // Ligar lanterna e farol baixo
 if(incomingByte == 248) {
 servoTorre.write(130);
 }
 // Desligar lanterna, farol baixo, pisca alerta e alarme
 if(incomingByte == 251) {
 servoTorre.write(90);
 digitalWrite(led,LOW);
 }
 // Pisca Alerta
 if(incomingByte == 250) 
 servoTorre.write(45);
 digitalWrite(led,HIGH);
 
 }
 
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
 **  Left: 86 > to < 165    **
 **  Center: 85             **
 **  Right: 13 > to < 84    **
 *****************************
 
 */




































