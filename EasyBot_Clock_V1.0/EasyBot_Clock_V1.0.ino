/***************************************\
 **                                     **
 **   Relogio completo com termometro   **
 **             usando RTC              **
 **      Primeira grande montagem       **
 **                                     **
 \***************************************/


// carregando biblioteca para display e RTC
#include <LiquidCrystal.h>
#include "Wire.h"

// definindo o endereco I2C para o RTC
#define DS1307_ADDRESS 0x68

// especifica as portas para uso no display
LiquidCrystal lcd(25, 27, 29, 31, 33, 35);

// definindo a variavel para zerar o contador do RTC
byte zero = 0x00; //workaround for issue #527

// definindo variaveis numericas inteiras
// hora e calendario
int ajustesegundo = 0;
int ajusteminuto = -1;
int ajustehora = -1;
int ajustedia= 0;
int ajustemes = 0;
int ajusteano = 2014;

// termometro
float adclido = 0;
float temperaturac = 0;
int lertemperatura = 1;
float temperaturas [32];

// ajustes e menu
int delayajuste = 200; // tempo que o botao saida deve ficar pressionado para entrar no menu ajuste de tempo
int contadorsemana = 0; // controle do dia de semana
int tempoajuste = 0; 
int movimento = 0;
int saida = 0;
int tempomenu = 1;
int controle = 0;
int funcao = 0;
int luzajuste = 0;
int luzmenu = 0;

// Luz do LCD
int ldrvalor = 0;
int ldron = 1; // define se a luz do LCD funcionara pelo LDR ou sempre acesa
int ledpwm = 255;
int tempoluz = 0;
int ativaluz = 0;
int tempoluzbotao = 600;

// definindo constantes inteiras para portas Arduino
const int lcdled = 45;
const int botao1 = 52;
const int botao2 = 50;
const int LDR = A14;
const int LM35 = A15;

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


/********************************\
 **                              **
 **   Iniciando as Declaracoes   **
 **                              **
 \********************************/

// Declaracoes
void setup() {

  // declarando a biblioteca Wire 
  Wire.begin();

  // definindo modo dos pinos  
  //pinMode (botao0, INPUT);
  pinMode (botao1, INPUT);
  pinMode (botao2, INPUT);
  pinMode (lcdled, OUTPUT);

  // definindo conteudo da variavel tempo  
  tempo=0;   

  // definindo comunicacao serial
  Serial.begin(9600);

  // Acende backlight e dados do LCD 
  analogWrite(lcdled, ledpwm); 

  // tela de introducao LCD
  lcd.begin(16, 2);
  lcd.setCursor(0,0);   
  lcd.print(" EasyBot Clock ");   
  lcd.setCursor(0,1);   
  lcd.print("  com Arduino");   
  delay (2500); 
  lcd.clear();
  lcd.setCursor(0,0);   
  lcd.print("   Versao 1.0");
  lcd.setCursor(0,1);   
  lcd.print("   by Magneto");   
  delay (3000); 
  lcd.clear();
}


/********************************\
 **                              **
 **    Iniciando o programa      **
 **                              **
 \********************************/

// Programa
void loop() {

  // lendo posicao dos botoes
  movimento = digitalRead(botao1);
  saida = digitalRead(botao2);

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
  if(ativaluz == 0) luzdisplay();
  if(hora >= 22 && ativaluz == 0) ledpwm = 0;
  if(hora >= 0 && hora < 7 && ativaluz == 0) ledpwm = 0;
  analogWrite(lcdled, ledpwm);

  //Configura a hora e calendario   
  if (tempoajuste == 1 && tempomenu > 0) {
    if (saida == HIGH) {
      saitempoajuste(); 
    }
    switch (tempomenu) {
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
  if (luzajuste == 1 && luzmenu > 0) {
    analogWrite(lcdled, 255);
    if (saida == HIGH) {
      sailuzajuste(); 
    }
    switch (luzmenu) {
      case (1):
      entraluzajuste();
      break;
    }
  }

  // Chave de selecao de calculo ou ajuste
  if(tempoajuste == 0 && luzajuste == 0) {

    // Imprimindo no Display
    imprimindodados();

    // Habilitando entrada no ajuste de tempo
    if(saida == HIGH) {
      controle++; 
    }

    if(controle > delayajuste) {
      lcd.clear();
      delay(1000);
      tempoajuste = 1;
      tempomenu = 1;
      controle = 0; 
    }

    // Acende a luz do display
    if(movimento == HIGH || saida == HIGH) {
      tempoluz = 1;
      ativaluz = 1; 
    }

    if(ativaluz == 1) {
      acendedisplay(); 
    }

    // Ajuste Entrada na funcao luz
    if (funcao == HIGH && movimento == HIGH) {   
      lcd.clear();
      delay(300);
      luzajuste = 1; 
      luzmenu = 1;
    }

    // Desliga temporizador da Luz
    if (ativaluz == 1 && tempoluz >= tempoluzbotao) {
      ativaluz = 0;
      tempoluz = 0; 
    }
  }

  // incrementa o valor do indice da matriz em 1
  lertemperatura++;

  // degub na tela via USB
  debugnatela();

}

/**********************************\
 **                               **
 ** Definindo Rotinas do programa **
 **                               **
 \*********************************/

byte decToBcd(byte val){
  // Convert normal decimal numbers to binary coded decimal
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)  {
  // Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}


void termometro() { // Leitura do termometro

    if(lertemperatura > 29) lertemperatura = 0;

  adclido = analogRead(LM35);
  temperaturas[lertemperatura] = (500 * adclido) / 1023; // equiparar multimetro com prototipo
  //  temperaturas[lertemperatura] = (adclido * ((1.1/1023)*100)) - 0.5; // equiparar multimetro com projeto caixa

  temperaturas[31]  = ( temperaturas[0]   + temperaturas[1]  + temperaturas[2]
    + temperaturas[3]  + temperaturas[4]  + temperaturas[5]  + temperaturas[6]
    + temperaturas[7]  + temperaturas[8]  + temperaturas[9]  + temperaturas[10]
    + temperaturas[11] + temperaturas[12] + temperaturas[13] + temperaturas[14]
    + temperaturas[15] + temperaturas[16] + temperaturas[17] + temperaturas[18]
    + temperaturas[19] + temperaturas[20] + temperaturas[21] + temperaturas[22]
    + temperaturas[23] + temperaturas[24] + temperaturas[25] + temperaturas[26]
    + temperaturas[27] + temperaturas[28] + temperaturas[29]) / 30;

  temperaturac = temperaturas[31];
}

void luzdisplay() { //luz do display

    ldrvalor = analogRead(LDR);

  if(ldrvalor > 500 && ldron == 1) {
    ledpwm = 255; // Totalmente aceso
  }

  if(ldrvalor < 30 && ldron == 1) {
    ledpwm = 5; // pouca luz para nao atrapalhar no escuro
  }

  if(ldrvalor > 29 && ldrvalor < 501 && ldron == 1) {
    //ledpwm = ldrvalor / 2; // controle de luminisidade >> Futuro fazer degraus
    ledpwm = map(ldrvalor,0,800,0,255);
  }

  if(ldron == 0) {
    ledpwm = 255; // Totalmente aceso
  }
}

void acendedisplay () { // Rotina de acendimento do Display (anytime)

  if(tempoluz < tempoluzbotao) { 
    ledpwm = 255;
    tempoluz++; 
  }
}

void entraluzajuste () { // Rotina de selecao do controle de Luz: auto ou manual

  lcd.setCursor(0,0);   
  lcd.print("Modo LDR: ");

  if(ldron == 1) {
    lcd.print("On ");
  }
  else {
    lcd.print("Off");
  }

  if(movimento == HIGH) {   
    ldron++;
    if(ldron > 1) {
      ldron = 0; 
    }

    lcd.setCursor(10,0);
    if(ldron == 1) {
      lcd.print("On "); 
      lcd.print(" ");
      delay(200);
    } 
    if(ldron == 0) {
      lcd.print("Off"); 
      lcd.print(" ");
      delay(200);
    } 
  } 
}

void sailuzajuste () {  // Rotina de saida menu funcao

  luzmenu++;
  delay(200);

  if(luzajuste == 1) {
    lcd.clear(); 
  }

  if(luzmenu > 1) {
    luzmenu = 0;
    luzajuste = 0; 
  }
}

void entrahora () { // Rotina de Ajuste de horas

  lcd.setCursor(0,0);   
  lcd.print("Hora: ");

  if(movimento == HIGH) {   
    ajustehora++;
    if(ajustehora > 23) {
      ajustehora = 0; 
    }

    lcd.setCursor(6,0);
    lcd.print(ajustehora);
    lcd.print("       ");
    delay(200); 
  } 
}

void entraminuto () { // Rotina de Ajuste de minutos

  lcd.setCursor(0,0);   
  lcd.print("Minuto: ");  

  if(movimento == HIGH) { 
    ajusteminuto++; 
    if(ajusteminuto > 59) {
      ajusteminuto = 0; 
    }

    lcd.setCursor(8,0);
    lcd.print(ajusteminuto);
    lcd.print("       ");
    delay(200); 
  }
}


void entradia () { // Rotina de Ajuste dia do mes

  lcd.setCursor(0,0);   
  lcd.print("Dia: ");  

  if(movimento == HIGH) {
    ajustedia++;
    if(ajustedia > 31) {
      ajustedia = 1; 
    }

    lcd.setCursor(5,0);
    lcd.print(ajustedia);
    lcd.print("       ");
    delay(200); 
  }
}

void entrames () { // Rotina de Ajuste do mes

    lcd.setCursor(0,0);   
  lcd.print("Mes: ");   

  if(movimento == HIGH) {
    ajustemes++;
    if(ajustemes > 12) {
      ajustemes = 1; 
    }

    lcd.setCursor(5,0);
    lcd.print(ajustemes);
    lcd.print("       ");
    delay(200); 
  }
}

void entraano () { // Rotina de Ajuste do ano

    lcd.setCursor(0,0);   
  lcd.print("Ano: 20");   

  if(movimento == HIGH) {
    ajusteano++;
    if(ajusteano > 20) {
      ajusteano = 15; 
    }

    lcd.setCursor(7,0);
    lcd.print(ajusteano);
    lcd.print("       ");
    delay(200); 
  }
}

void entrasemana () {  // Rotina de Ajuste do dia da semana

    lcd.setCursor(0,0);   
  lcd.print("Dia Semana: ");   

  if(movimento == HIGH) {
    contadorsemana++;
    if(contadorsemana > 6) {
      contadorsemana = 0; 
    }

    lcd.setCursor(12,0);

    if(contadorsemana == 0) {
      lcd.print(semana0);
      lcd.print("       ");
      delay(200); 
    }

    if(contadorsemana == 1) {
      lcd.print(semana1);
      lcd.print("       ");
      delay(200); 
    }

    if(contadorsemana == 2) {
      lcd.print(semana2);
      lcd.print("       ");
      delay(200); 
    }

    if(contadorsemana == 3) {
      lcd.print(semana3);
      lcd.print("       ");
      delay(200); 
    }

    if(contadorsemana == 4) {
      lcd.print(semana4);
      lcd.print("       ");
      delay(200); 
    }

    if(contadorsemana == 5) {
      lcd.print(semana5);
      lcd.print("       ");
      delay(200); 
    }

    if(contadorsemana == 6) {
      lcd.print(semana6);
      lcd.print("       ");
      delay(200); 
    }
  }   

}

void saitempoajuste () {  // Rotina de saida menu ajuste

    tempomenu++;
  delay(200);

  if(tempoajuste == 1) {
    lcd.clear(); 
  }

  if(tempomenu > 6) {
    tempomenu = 0;
    tempoajuste = 0;
    setDateTime(); 
  }
}

void setDateTime() {

  byte segundo = ajustesegundo;
  byte minuto = ajusteminuto;
  byte hora = ajustehora;
  byte weekDay = contadorsemana;
  byte dia = ajustedia;
  byte mes = ajustemes;
  byte ano  = ajusteano;

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

void imprimindodados() { 

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
  lcd.print(temperaturac); 
  lcd.setCursor(15,0);
  lcd.print("C"); 
}

void debugnatela() {

  //Serial.print("Valor Menu:");
  //Serial.print(tempomenu);
  //Serial.print(" Contador Semana: ");
  //Serial.print(contadorsemana);
  //Serial.print(" Dia da Semana: ");
  //Serial.print(weekDay);
  //Serial.print(" Ajuste: ");
  //Serial.print(tempoajuste);
  //Serial.print(" Controle: ");
  //Serial.print(controle);
  //Serial.print(" PWM LDR: ");
  //Serial.print(ledpwm);
  //Serial.print(" Valor LDR: ");
  //Serial.print(ldrvalor);
  //Serial.print(" Luz: ");
  //Serial.print(ativaluz);
  //Serial.print(" Tempo Luz: ");
  //Serial.print(tempoluz);
  //Serial.print(" energia temporario 1: ");
  //Serial.print(energiatemp1);
  //Serial.print(" energia temporario 2: ");
  //Serial.print(energiatemp2);
  //Serial.print(" energia: ");
  //Serial.print(energia);
  //Serial.print(" indice: ");
  //Serial.print(indice);
  //Serial.print(" soma temperatura: ");
  //Serial.print(somatemperatura);
  //Serial.print(" temperatura1: ");
  //Serial.print(matriztemperatura[1]);
  //Serial.print(" temperatura2: ");
  //Serial.print(matriztemperatura[2]);
  //Serial.print(" temperatura3: ");
  //Serial.print(matriztemperatura[3]);
  //Serial.println("  ");
  //delay(50);
}

























