/*
*******************************************************
 Programa de Controle do EasyBot
 Autor Magneto - Novembro 2016
 ******************************************************
 */

#include <Servo.h> //inclui biblioteca Servo
#include <LiquidCrystal.h> // inclui biblioteca de LCD
#include "Wire.h" // inclui biblioteca RTC

// definindo o endereco I2C para o RTC
#define DS1307_ADDRESS 0x68

LiquidCrystal lcd(25, 27, 29, 31, 33, 35);

// definindo a variavel para zerar o contador do RTC
byte zero = 0x00; //workaround for issue #527

// *** Definicao dos pinos para movimento e direcao ***
Servo servoEsquerdo;
Servo servoDireito;
Servo servoTorre;

// definindo variaveis numericas inteiras relogio
// hora e calendario
int ajusteSegundo = 0;
int ajusteMinuto = -1;
int ajusteHora = -1;
int ajusteDia= 0;
int ajusteMes = 0;
int ajusteAno = 2014;

// termometro
float adcLido = 0;
float temperaturaC = 0;
int temperaturaLida = 1;
float temperaturas [31];

// Menu Principal
int listaMenu = 1;
int entraMenu = 1;
int rodaMenu = 0;
int foraMenu = 0;
int parada = 0;

// ajustes e menu relogio
int delayAjuste = 200; // tempo que o botao saida deve ficar pressionado para entrar no menu ajuste de tempo
int contadorSemana = 0; // controle do dia de semana
int tempoAjuste = 0; 
int movimento = 0;
int entrada = 0;
int tempoMenu = 1;
int controle = 0;
int funcao = 0;
int luzAjuste = 0;
int luzMenu = 0;

// Luz do LCD
int ldrValor = 0;
int ldrOn = 1; // define se a luz do LCD funcionara pelo LDR ou sempre acesa
int ledPWM = 255;
int tempoLuz = 0;
int ativaLuz = 0;
int tempoLuzBotao = 600;

// Definicao dos pinos
const int botaoPreto = 52;
const int botaoVermelho = 51;
const int botaoVerde = 50;
const int LDR = A14;
const int LiPo = A13;
const int LM35 = A15;
const int trig = 39;
const int echo = 37;
const int ledVerde = 11;
const int ledVermelho = 12;
const int lcdLed = 4;

// variaveis ultrasom
long duracao;
long distancia;
long distancia1;
long distancia2;
int distanciaLida = 1;
float distancias [6];
int distancia2Lida = 1;
float distancias2 [32];
long medida0 = 0;
long medida45 = 0;
long medidaCentro = 0;
long medida135 = 0;
long medida180 = 0;

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
byte trimJoy1 = 36; // trimagem do joystick 1 conforme programa de teste do TX
byte trimJoy2 = 98; // trimagem do joystick 2 conforme programa de teste do TX
byte trimJoy3 = 157; // trimagem do joystick 3 conforme programa de teste do TX
int angleLeft = 82; // servo esquerdo na posicao central
int angleRight = 80; // servo direito na posicao central
int angleTower = 93; // servo torre na posicao central

// *** Definicao de outras variaveis ***
int incomingByte; // Variavel de entrada de dados pela Serial
long previousMillis = 0; // will store last time LED was updated
long interval = 100;  // interval at which to blink (milliseconds)

// Definindo texto de caracteres
char semana0[]= "Dom";
char semana1[]= "Seg";
char semana2[]= "Ter";
char semana3[]= "Qua";
char semana4[]= "Qui";
char semana5[]= "Sex";
char semana6[]= "Sab";

// Assinalando a variavel tempo como 32 bits
unsigned long tempo; 



/**********************************\
 **                               **
 ** Definindo Setup dos programas **
 **                               **
 \*********************************/

void setup() { // Declaracoes

  // declarando a biblioteca Wire 
  Wire.begin();

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
  pinMode (botaoPreto, INPUT);
  pinMode (botaoVermelho, INPUT);
  pinMode (botaoVerde, INPUT);
  pinMode(lcdLed, OUTPUT);

  // posiciona servos
  servoEsquerdo.write(82);
  servoDireito.write(80);
  servoTorre.write(93);

  // definindo conteudo da variavel tempo  
  tempo=0; 

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
  lcd.print("   Versao 1.2");
  lcd.setCursor(0,1);   
  lcd.print("   by Magneto");   
  delay (3000); 
  lcd.clear();

}


/**********************************\
 **                               **
 ** Definindo Rotinas do programa **
 **                               **
 \*********************************/


// ************** Inicio Rotinas Relogio **************** 

byte decToBcd(byte val) { // Convert normal decimal numbers to binary coded decimal
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val) { // Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}

void setDateTime() { // Transferindo as informacoes das variaveis para o RTC

  byte segundo = ajusteSegundo;
  byte minuto = ajusteMinuto;
  byte hora = ajusteHora;
  byte weekDay = contadorSemana;
  byte dia = ajusteDia;
  byte mes = ajusteMes;
  byte ano  = ajusteAno;

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero); //stop Oscillator

  Wire.write(decToBcd(segundo));
  Wire.write(decToBcd(minuto));
  Wire.write(decToBcd(hora));
  Wire.write(decToBcd(weekDay));
  Wire.write(decToBcd(dia));
  Wire.write(decToBcd(mes));
  Wire.write(decToBcd(ano));

  Wire.write(zero); //start 

  Wire.endTransmission();

}

void termometro() { // Leitura do termometro

    if(temperaturaLida > 29) temperaturaLida = 0;

  adcLido = analogRead(LM35);
  temperaturas[temperaturaLida] = (500 * adcLido) / 1023; 

  temperaturas[30]  = ( temperaturas[0]   + temperaturas[1]  + temperaturas[2]
    + temperaturas[3]  + temperaturas[4]  + temperaturas[5]  + temperaturas[6]
    + temperaturas[7]  + temperaturas[8]  + temperaturas[9]  + temperaturas[10]
    + temperaturas[11] + temperaturas[12] + temperaturas[13] + temperaturas[14]
    + temperaturas[15] + temperaturas[16] + temperaturas[17] + temperaturas[18]
    + temperaturas[19] + temperaturas[20] + temperaturas[21] + temperaturas[22]
    + temperaturas[23] + temperaturas[24] + temperaturas[25] + temperaturas[26]
    + temperaturas[27] + temperaturas[28] + temperaturas[29]) / 30;

  temperaturaC = temperaturas[30];
}

void luzdisplay() { //Controle de luminosidade do display do relogio

  ldrValor = analogRead(LDR);

  if(ldrValor > 600 && ldrOn == 1) {
    ledPWM = 255; // Totalmente aceso
  }

  else if(ldrValor < 70 && ldrOn == 1) {
    ledPWM = 5; // pouca luz para nao atrapalhar no escuro
  }

  else if(ldrValor > 69 && ldrValor < 600 && ldrOn == 1) {
    ledPWM = map(ldrValor, 70, 599, 6, 255); // remapeando a escala de luminosidade de acordo com LDR
  }

}

void acendedisplay () { // Rotina de acendimento do Display (anytime)

  if(tempoLuz < tempoLuzBotao) { 
    ledPWM = 255;
    tempoLuz++; 
  }
}

// *** Verificar a necessidade desta rotina ***
void sailuzAjuste () {  // Rotina de saida menu funcao

  luzMenu++;
  delay(200);

  if(luzAjuste == 1) {
    lcd.clear(); 
  }

  if(luzMenu > 1) {
    luzMenu = 0;
    luzAjuste = 0; 
  }
}

void entrahora () { // Rotina de Ajuste de horas

  lcd.setCursor(0,0);   
  lcd.print("Hora: ");

  if(movimento == HIGH) {   
    ajusteHora++;
    if(ajusteHora > 23) {
      ajusteHora = 0; 
    }

    lcd.setCursor(6,0);
    lcd.print(ajusteHora);
    lcd.print("       ");
    delay(200); 
  } 
}

void entraminuto () { // Rotina de Ajuste de minutos

  lcd.setCursor(0,0);   
  lcd.print("Minuto: ");  

  if(movimento == HIGH) { 
    ajusteMinuto++; 
    if(ajusteMinuto > 59) {
      ajusteMinuto = 0; 
    }

    lcd.setCursor(8,0);
    lcd.print(ajusteMinuto);
    lcd.print("       ");
    delay(200); 
  }
}

void entradia () { // Rotina de Ajuste dia do mes

  lcd.setCursor(0,0);   
  lcd.print("Dia: ");  

  if(movimento == HIGH) {
    ajusteDia++;
    if(ajusteDia > 31) {
      ajusteDia = 1; 
    }

    lcd.setCursor(5,0);
    lcd.print(ajusteDia);
    lcd.print("       ");
    delay(200); 
  }
}

void entrames () { // Rotina de Ajuste do mes

    lcd.setCursor(0,0);   
  lcd.print("Mes: ");   

  if(movimento == HIGH) {
    ajusteMes++;
    if(ajusteMes > 12) {
      ajusteMes = 1; 
    }

    lcd.setCursor(5,0);
    lcd.print(ajusteMes);
    lcd.print("       ");
    delay(200); 
  }
}

void entraano () { // Rotina de Ajuste do ano

    lcd.setCursor(0,0);   
  lcd.print("Ano: 20");   

  if(movimento == HIGH) {
    ajusteAno++;
    if(ajusteAno > 20) {
      ajusteAno = 15; 
    }

    lcd.setCursor(7,0);
    lcd.print(ajusteAno);
    lcd.print("       ");
    delay(200); 
  }
}

void entrasemana () {  // Rotina de Ajuste do dia da semana

    lcd.setCursor(0,0);   
  lcd.print("Dia Semana: ");   

  if(movimento == HIGH) {
    contadorSemana++;
    if(contadorSemana > 6) {
      contadorSemana = 0; 
    }

    lcd.setCursor(12,0);

    if(contadorSemana == 0) {
      lcd.print(semana0);
      lcd.print("       ");
      delay(200); 
    }

    if(contadorSemana == 1) {
      lcd.print(semana1);
      lcd.print("       ");
      delay(200); 
    }

    if(contadorSemana == 2) {
      lcd.print(semana2);
      lcd.print("       ");
      delay(200); 
    }

    if(contadorSemana == 3) {
      lcd.print(semana3);
      lcd.print("       ");
      delay(200); 
    }

    if(contadorSemana == 4) {
      lcd.print(semana4);
      lcd.print("       ");
      delay(200); 
    }

    if(contadorSemana == 5) {
      lcd.print(semana5);
      lcd.print("       ");
      delay(200); 
    }

    if(contadorSemana == 6) {
      lcd.print(semana6);
      lcd.print("       ");
      delay(200); 
    }
  }   

}

void saitempoAjuste () {  // Rotina de saida menu ajuste

    tempoMenu++;
  delay(300);

  if(tempoAjuste == 1) {
    lcd.clear(); 
  }

  if(tempoMenu > 6) {
    tempoMenu = 0;
    tempoAjuste = 0;
    setDateTime(); 
  }
}

void imprimindodados() { // Impressao de informacao no Display

  // Reset the register pointer
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);

  // carregando as variaveis de tempo
  int segundo = bcdToDec(Wire.read());
  int minuto = bcdToDec(Wire.read());
  int hora = bcdToDec(Wire.read() & 0b111111); //24 horas
  int weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
  int dia = bcdToDec(Wire.read());
  int mes = bcdToDec(Wire.read());
  int ano = bcdToDec(Wire.read()); 

  if(dia < 10) {
    lcd.setCursor(0,1);    
    lcd.print("0");
    lcd.print(dia);   
    lcd.print("-"); 
  }
  else {  
    lcd.setCursor(0,1);    
    lcd.print(dia);   
    lcd.print("-"); 
  }

  lcd.setCursor(3,1);
  if(mes == 1) lcd.print("Jan");
  if(mes == 2) lcd.print("Fev");
  if(mes == 3) lcd.print("Mar");
  if(mes == 4) lcd.print("Abr");
  if(mes == 5) lcd.print("Mai");
  if(mes == 6) lcd.print("Jun");
  if(mes == 7) lcd.print("Jul");
  if(mes == 8) lcd.print("Ago");
  if(mes == 9) lcd.print("Set");
  if(mes == 10) lcd.print("Out");
  if(mes == 11) lcd.print("Nov");
  if(mes == 12) lcd.print("Dez");
  lcd.setCursor(6,1);
  lcd.print("-");  

  lcd.setCursor(7,1);
  lcd.print("20");
  lcd.print(ano);
  lcd.print("  ");

  lcd.setCursor(13,1);
  if(weekDay == 0 || weekDay == 7) {    
    lcd.print(semana0); 
  }  
  if(weekDay == 1) {    
    lcd.print(semana1); 
  }  
  if(weekDay == 2) {    
    lcd.print(semana2); 
  }  
  if(weekDay == 3) {    
    lcd.print(semana3); 
  }  
  if(weekDay == 4) {    
    lcd.print(semana4); 
  }  
  if(weekDay == 5) {    
    lcd.print(semana5); 
  }  
  if(weekDay == 6) {    
    lcd.print(semana6); 
  }  

  if(hora < 10) {
    lcd.setCursor(0,0); 
    lcd.print("0");  
    lcd.print(hora);   
    lcd.print(":"); 
  }
  else {  
    lcd.setCursor(0,0);    
    lcd.print(hora);   
    lcd.print(":"); 
  }
  if(minuto < 10) {
    lcd.setCursor(3,0);
    lcd.print("0");  
    lcd.print(minuto);   
    lcd.print(":"); 
  }
  else {  
    lcd.setCursor(3,0);    
    lcd.print(minuto);   
    lcd.print(":"); 
  }
  if(segundo < 10) {
    lcd.setCursor(6,0);   
    lcd.print("0"); 
    lcd.print(segundo);
    lcd.print("   "); 
  } 
  else {  
    lcd.setCursor(6,0);    
    lcd.print(segundo);
    lcd.print("   "); 
  }  

  lcd.setCursor(11,0);   
  lcd.print(temperaturaC); 
  lcd.setCursor(15,0);
  lcd.print("C"); 
}

// ************** Final Rotinas Relogio **************** 





// ************** Inicio Rotinas Comuns **************** 

void medirDistancia() { // Leitura do ultrasom

  //for(int i = 0; i < 6; i++) {

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duracao = pulseIn(echo, HIGH); 
  //distancias[i] = duracao / 56;
  distancia = duracao / 56;
 // }

  //distancia  = (distancias[1]   + distancias[2]  + distancias[3] + distancias[4]  + distancias[5]) / 5;
}

void medirDistancia2() { // Leitura do ultrasom

  if(distancia2Lida > 29) distancia2Lida = 0;

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duracao = pulseIn(echo, HIGH);
  distancias2[distancia2Lida] = (duracao / 58) + 5;

  distancias2[31]  = ( distancias2[0]   + distancias2[1]  + distancias2[2]
    + distancias2[3]  + distancias2[4]  + distancias2[5]  + distancias2[6]
    + distancias2[7]  + distancias2[8]  + distancias2[9]  + distancias2[10]
    + distancias2[11] + distancias2[12] + distancias2[13] + distancias2[14]
    + distancias2[15] + distancias2[16] + distancias2[17] + distancias2[18]
    + distancias2[19] + distancias2[20] + distancias2[21] + distancias2[22]
    + distancias2[23] + distancias2[24] + distancias2[25] + distancias2[26]
    + distancias2[27] + distancias2[28] + distancias2[29]) / 30;

  distancia2 = distancias2[31];
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
  lcd.setCursor(11,0);
  lcd.print(distancia);
  lcd.setCursor(14,0);
  lcd.print("cm");
  lcd.setCursor(0,1);
  lcd.print("Traseira:      ");
  lcd.setCursor(11,1);
  lcd.print(distancia + 14);
  lcd.setCursor(14,1);
  lcd.print("cm");
}

void imprimindoVoltagem () { // imprimindo valor voltagem
  lcd.setCursor(0,1);
  lcd.print("Bateria:       ");
  lcd.setCursor(9,1);
  lcd.print(Vin);
  lcd.setCursor(15,1);
  lcd.print("V ");
}

void frenteApenas () { // deslocamento apenas para frente
  servoEsquerdo.write(180);
  servoDireito.write(0);
}

void atrasApenas () { // deslocamento apenas para tras
  servoEsquerdo.write(0);
  servoDireito.write(180);
}

void frenteEsquerda () { // deslocamento para frente e esquerda
  servoEsquerdo.write(82);
  servoDireito.write(0);
}

void frenteDireita () { // deslocamento para frente e direita
  servoEsquerdo.write(180);
  servoDireito.write(80);
}

void atrasEsquerda () { // deslocamento para tras e esquerda
  servoEsquerdo.write(82);
  servoDireito.write(180);
}

void atrasDireita () { // deslocamento para tras e direita
  servoEsquerdo.write(0);
  servoDireito.write(80);
}

void parado () { // sem deslocamento
  servoEsquerdo.write(82);
  servoDireito.write(80);
}

// Controle de movimento autonomo da torre
void torreCentro () { // torre no centro
  servoTorre.write(93);
}

void torre45 () { // torre para esquerda 45 graus
  servoTorre.write(45);
}

void torre0 () { // torre para esquerda 90 graus
  servoTorre.write(0);
}

void torre135 () { // torre para direita 45 graus
  servoTorre.write(135);
}

void torre180 () { // torre para direita 90 graus
  servoTorre.write(180);
}

void varredura() { // varredura de distancia
  torre0();
  delay(200);
  medirDistancia();
  medida0 = distancia;
  delay(200);
  torre45();
  delay(200);
  medirDistancia();
  medida45 = distancia;
  delay(200);
  torreCentro();
  delay(200);
  medirDistancia();
  medidaCentro = distancia;
  delay(200);
  torre135();
  delay(200);
  medirDistancia();
  medida135 = distancia;
  delay(200);
  torre180();
  delay(200);
  medirDistancia();
  medida180 = distancia;
  delay(200);
}

// ************** Final Rotinas Comuns ****************





// ************** Inicio Rotina Menu Principal **************** 

void menuPrincipal () { // Escolhas Menu Principal
  lcd.setCursor(0,0);
  lcd.print("### Programa ###");

  if(movimento == HIGH) { 
    listaMenu++; 
  }
  if(listaMenu > 4) {
    listaMenu = 0; 
  }
  if(listaMenu == 1) {
    lcd.setCursor(0,1);
    lcd.print(" Radio  Controle ");
    rodaMenu = 1;
    delay(400);
  }

  else if(listaMenu == 2) {
    lcd.setCursor(0,1);
    lcd.print(" Modo  Autonomo  ");
    rodaMenu = 2;
    delay(400);
  }

  else if(listaMenu == 3) {
    lcd.setCursor(0,1);
    lcd.print("Relogio de Mesa ");
    rodaMenu = 3;
    delay(400);
  }

  else if(listaMenu == 4) {
    lcd.setCursor(0,1);
    lcd.print("Trena acima 50cm");
    rodaMenu = 4;
    delay(400);
  }
  movimento = digitalRead(botaoPreto);
  entrada = digitalRead(botaoVerde);
  parada = digitalRead(botaoVermelho);
}

// ************** Final Rotina Menu Principal **************** 




// ************** Inicio Rotina Modo Radio Controle ****************

void modoRadioControle () { // Modo Radio Controle
  parada = digitalRead(botaoVermelho);


  voltagemBateria(); // ler voltagem

  lcd.setCursor(0,0);
  lcd.print("EasyBot R/C mode");

  if (Serial1.available() > 0) { // Verificando se tem entrada de dados na Serial
    incomingByte = Serial1.read(); // Lendo o ultimo dado recebido

    if(incomingByte >= 8 && incomingByte <= 67) { // Identificar J1LeftRight - Joystick da Esquerda
      scaleJoy1 = (incomingByte - trimJoy1);
    }

    if(incomingByte >= 68 && incomingByte <= 127) { // Identificar J2UpDown - Joystick da Esquerda
      scaleJoy2 = (incomingByte - trimJoy2);
    }

    if(incomingByte >= 128 && incomingByte <= 187) { // Identificar J3LeftRight - joystick da Direita
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
      angleLeftJoy1 = map(scaleIntJoy1, 1, 30, 0, 92);
      angleTower = angleLeftJoy1;
    }
    else if(scaleJoy1 > 0) { // torre para direita
      scaleIntJoy1 = scaleJoy1;
      angleRightJoy1 = map(scaleIntJoy1, 1, 30, 170, 94);
      angleTower = angleRightJoy1;
    }
    else if(scaleJoy1 == 0) { // torre para centro
      angleTower = 93;
    }

    if(incomingByte == 251) {
      foraMenu = 1;
    }

  }

  servoEsquerdo.write(angleLeft);
  servoDireito.write(angleRight);
  servoTorre.write(angleTower);

  // ler se tem que parar o programa
  if(parada == HIGH) {
    foraMenu == 1;
  }
}  

// ************** Final Rotina Modo Radio Controle ****************




// ************** Inicio Rotina Modo Relogio ****************

void modoRelogio () { // modo relogio
  movimento = digitalRead(botaoPreto);
  entrada = digitalRead(botaoVerde);
  parada = digitalRead(botaoVermelho);

  // Reset the register pointer
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);

  // carregando as variaveis com informacao do RTC
  int segundo = bcdToDec(Wire.read());
  int minuto = bcdToDec(Wire.read());
  int hora = bcdToDec(Wire.read() & 0b111111); //24 hour time
  int weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
  int dia = bcdToDec(Wire.read());
  int mes = bcdToDec(Wire.read());
  int ano = bcdToDec(Wire.read()); 
  // Indo para rotina do termometro
  termometro();

  // Indo para rotina de iluminacao do display
  if(ativaLuz == 0) luzdisplay();
  if(hora >= 22 && ativaLuz == 0) ledPWM = 0;
  if(hora >= 0 && hora < 7 && ativaLuz == 0) ledPWM = 0;
  analogWrite(lcdLed, ledPWM); 

  //Configura a hora e calendario   
  if (tempoAjuste == 1 && tempoMenu > 0) {
    if (entrada == HIGH) {
      saitempoAjuste(); 
    }
    switch (tempoMenu) {
      case (1):
      entrahora();
      break;
      case (2):
      entraminuto();
      break;
      case (3):
      entradia();
      break;
      case (4):
      entrames();
      break;
      case (5):
      entraano();
      break;
      case (6):
      entrasemana();
      break; 
    }
  }

  //Configura ajuste Luz Display  
  // if (luzAjuste == 1 && luzMenu > 0) {
  //   analogWrite(lcdLed, 255);
  //   if (entrada == HIGH) {
  //    sailuzAjuste(); 
  //  }
  //  switch (luzMenu) {
  //   case (1):
  //   entraluzAjuste();
  //   break;
  // }
  //}

  // Chave de selecao de calculo ou ajuste
  if(tempoAjuste == 0 && luzAjuste == 0) {

    // Imprimindo no Display
    imprimindodados();
    //  imprimindoDistancia();
    //  imprimindoVoltagem();

    // Habilitando entrada no ajuste de tempo
    if(entrada == HIGH) {
      controle++; 
    }

    if(controle > delayAjuste) {
      lcd.clear();
      delay(1000);
      tempoAjuste = 1;
      tempoMenu = 1;
      controle = 0; 
    }

    // Acende a luz do display
    if(movimento == HIGH || entrada == HIGH) {
      tempoLuz = 1;
      ativaLuz = 1; 
    }

    if(ativaLuz == 1) {
      acendedisplay(); 
    }

    // Desliga temporizador da Luz
    if (ativaLuz == 1 && tempoLuz >= tempoLuzBotao) {
      ativaLuz = 0;
      tempoLuz = 0; 
    }
  }

  // incrementa o valor do indice da matriz em 1
  temperaturaLida++;

  // ler se tem que parar o programa
  if(parada == HIGH) {
    foraMenu == 1;
  } 

}

// ************** Final Rotina Modo Relogio ****************



// ************** Inicio Rotina Modo Trena ****************

void modoTrena () { // modo Trena
  parada = digitalRead(botaoVermelho);
  lcd.setCursor(0,0);
  medirDistancia();
  lcd.print("Trena Eletronica");
  delay(4000);
  imprimindoDistancia();

  // ler se tem que parar o programa
  if(parada == HIGH) {
    foraMenu == 1;
  }
}

// ************** Final Rotina Modo Trena ****************




// ************** Inicio Rotina Modo Autonomo ****************


void modoAutonomo () { // modo Autonomo
  parada = digitalRead(botaoVermelho);
  lcd.setCursor(0,0);
  lcd.print(" Autonomous !!! ");
  lcd.setCursor(0,1);
  lcd.print("                 ");
  parado();
 /*
 torre0();
 delay(500);
 medirDistancia();
 imprimindoDistancia();
 delay(3000);
 torre45();
  delay(500);
  medirDistancia();
 imprimindoDistancia();
 delay(3000);
  torreCentro();
   delay(500);
   medirDistancia();
 imprimindoDistancia();
 delay(3000);
  torre135();
   delay(500);
   medirDistancia();
 imprimindoDistancia();
 delay(3000);
  torre180();
   delay(500);
   medirDistancia();
 imprimindoDistancia();
 delay(3000);
 torreCentro();
  delay(500);
  medirDistancia();
 imprimindoDistancia();
 delay(10000);
 */
  torreCentro();
  delay(300);
  medirDistancia();
  medidaCentro = distancia;

  if(medidaCentro > 80){
    frenteApenas();
  }
  
varredura();

  if(medidaCentro < 15) {
    parado();
    if(medida0 > 15 && medida180 > 15) {
      if(medida0 > medida180) {
        atrasDireita();
        delay(1500);
        parado();
        torreCentro();
        frenteApenas();
      }
      else if(medida180 > medida0) {
        atrasEsquerda();
        delay(1500);
        parado();
        torreCentro();
        frenteApenas();
      }
    }
    if(medida0 > 15 && medida180 < 15) {
      atrasDireita();
      delay(1500);
      parado();
      frenteApenas();
    }
    else if(medida180 > 15) {
      atrasEsquerda();
      delay(1500);
      parado();
      torreCentro();
      frenteApenas();
    }
  }

  if(medidaCentro > 15 && medidaCentro < 60) {
    varredura();
    if(medida45 > 15 && medida135 > 15) {
      if(medida45 > medida135) {
        frenteEsquerda();
        delay(800);
        torreCentro();
        frenteApenas();
      }
      else if(medida135 > medida45) {
        frenteDireita();
        delay(800);
        torreCentro();
        frenteApenas();
      }
    }
    if(medida45 > 15 && medida135 < 15) {
      frenteEsquerda();
      delay(800);
      torreCentro();
      frenteApenas();
    }
    else if(medida135 > 15) {
      frenteEsquerda();
      delay(800);
      torreCentro();
      frenteApenas();
    }
  }

if(medida45 < 15 && medidaCentro < 15 && medida135 < 15) {
  parado();
  atrasApenas();
  delay(1500);
  parado();
}

    // ler se tem que parar o programa
    if(parada == HIGH) {
      foraMenu == 1;
    }
}

// ************** Final Rotina Modo Autonomo ****************


/**********************************\
 **                               **
 **  Definindo Loop do programa   **
 **                               **
 \*********************************/

void loop() {

  // lendo posicao dos botoes
  movimento = digitalRead(botaoPreto);
  entrada = digitalRead(botaoVerde);
  parada = digitalRead(botaoVermelho);

  // entrada no programa desejado
  if(entraMenu == 1) {
    while(entrada == LOW) {
      menuPrincipal();
    } 
  }
  entraMenu = 0;

  // rodar o programa desejado
  if(rodaMenu == 1) { // modo radio controle
    while(parada == LOW) {
      modoRadioControle();
    }
  }
  else if(rodaMenu == 2) { // modo autonomo
    while(parada == LOW) {
      modoAutonomo();
    }
  }
  else if(rodaMenu == 3) { // modo relogio de mesa
    while(parada == LOW) {
      modoRelogio();
    }
  }
  else if(rodaMenu == 4) { // modo Trena
    while(parada == LOW) {
      lcd.setCursor(0,1);
      lcd.print("Trena Eletron !!!");
      modoTrena();
    }
  }


  lcd.clear();

  rodaMenu = 0;
  entraMenu = 1;
    torreCentro();
    parado();

  for(int i = 0; i < 10; i++) {
    lcd.setCursor(0,0);
    lcd.print(" >>> Parada <<< ");
    lcd.setCursor(0,1);
    lcd.print(" > Solicitada < ");
    delay(100);
    lcd.setCursor(0,0);
    lcd.print("> >> Parada << <");
    lcd.setCursor(0,1);
    lcd.print(">  Solicitada  <");
    delay(100);
    lcd.setCursor(0,0);
    lcd.print(">> > Parada < <<");
    lcd.setCursor(0,1);
    lcd.print(">> Solicitada <<");
    delay(100);
    lcd.setCursor(0,0);
    lcd.print(">>>  Parada  <<<");
    lcd.setCursor(0,1);
    lcd.print(">  Solicitada  <");
    delay(100);
    lcd.setCursor(0,0);
    lcd.print(">>>> Parada <<<<");
    lcd.setCursor(0,1);
    lcd.print(">> Solicitada <<");
    delay(100);
  }

  lcd.setCursor(0,0);
  lcd.print("  Reiniciando   ");
  lcd.setCursor(0,1);
  lcd.print(" Menu Principal ");
  delay(3000);
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













































