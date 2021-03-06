/////////////////////// BIBLIOTECAS //////////////////////////////////////
#include <LiquidCrystal.h>

#include <EEPROM.h>

////////////////////// PORTAS ENTRADA/SAIDA /////////////////////////////
const int Led[] = {2, 3, 4, 5, 6, 7}; // barras do bar-graph

const int Ld = 22; // LED CARRO DIREITO
const int Le = 23; // LED CARRO ESQUERDO
const int Bd = 14;  // BOTAO DIREITO
const int Be = 15;  // BOTAO ESQUERDO
const int Od = 26;  // OBSTACULO DIREITO
const int Oe = 27;  // OBSTACULO ESQUERDO

LiquidCrystal lcd(12, 11, 35, 34, 33, 32);  // Portas do display lcd

const int PinoPotenciometro = A0; // Porta do potenciometro

const int speakerPin = 13;  // Porta do buzzer

const int motor = 9;  // Porta do motor

const int imprVel = 36; // Botão imprime velocidade (retorna valores da EEPROM)


////////////////////// VARIAVEIS ////////////////////////////////

int L1[] = {0, 0}; // Estados dos leds da primeira linha
int L2[] = {0, 0}; // Estados dos leds da segunda linha
int L3[] = {0, 0}; // Estados dos leds da terceira linha
int L4[] = {0, 0}; // Estados dos leds para obstaculos na linha do jogador

int Oesq = 0; //  Variavel: verifica se existe um obstaculo a esquerda na linha do jogador
int Odir = 0; //  Variavel: verifica se existe um obstaculo a direita na linha do jogador
int Cesq = 0; //  Variavel: verifica se o jogador está a esquerda em sua linha
int Cdir = 1; //  Variavel: verifica se o jogador está a direita em sua linha
int Besq = 0; // Estado do botão esquerdo
int Bdir = 0; //  Estaado do botão direito

int INI = 1;  // Variavel: verifica se é o inicio do jogo ou não

int pont = 0; // Pontuação

int vel = 0;  // multiplicador dos pontos baseado na velocidade

int novo = 0; // Variavel que controla a frequencia em que os obstaculos são criados
int tempo = 0;  // Variavel auxiliar para controle do delay entre as atualizações de posição dos obstaculos
int del = 3000; // Delay entre atualização da posição dos obstaculos

int endereco = 0; // Variavel que representa um endereco da EEPROM (varia de 0 a 49)

int ValorPot = 0; //  Valor atual do potenciometro

int pwm = 0; // Valor da porta pwm do motor

int pct = 0;  // porcentagem da velocidade em que o motor está funcionando

int pr = 0; // estado do botao imprime velocidade

//////////////////// SETUP ///////////////////////////////
void setup() {
  for (int x = 0; x < 6; x++) {
    pinMode(Led[x], OUTPUT);
  }

  lcd.begin(16, 2);

  pinMode(Le, OUTPUT);
  pinMode(Ld, OUTPUT);
  pinMode(Oe, OUTPUT);
  pinMode(Od, OUTPUT);

  pinMode(Be, INPUT);
  pinMode(Bd, INPUT);

  Serial.begin(9600);

}
/////////////////// LOOP ////////////////////
void loop() {

  if (tempo >= del) { // SE O AUXILIAR TEMPO FOR MAIOR QUE O DELAY ESTIPULADO PELO JOGADOR: ATUALIZA POSIÇÕES

    //atualiza posições dos obstaculos
    L4[0] = L3[0];
    L4[1] = L3[1];
    L3[0] = L2[0];
    L3[1] = L2[1];
    L2[0] = L1[0];
    L2[1] = L1[1];

    endereco++; // ATUALIZA POSIÇÃO DA MEMORIA EM QUE A PORCENTAGEM DA VELOCIDADE SERA GRAVADA

    novo++; // INCREMENTA VARIAVEL DE CONTROLE NOVO: PARA CADA 3 ATUALIZAÇÕES DE POSIÇÃO UM NOVO OBSTACULO É GERADO 
    if (novo == 3) novo = 0;
    if (novo == 1 || novo == 2) {
      L1[0] = 0;
      L1[1] = 0;
    }
    else if (novo == 0) {
      L1[0] = rand() % 2; // POSICIONAMENTO ALEATORIO DO OBSTACULO
      if (L1[0] == 0) L1[1] = 1;  // SOMENTE UMA COLUNA PODE TER UM OBSTACULO POR VEZ
      else if (L1[0] == 1) L1[1] = 0;
      pont = pont + (vel + 1) * 10; // ATUALIZAÇÃO NA PONTUAÇÃO BASEADA NA VELOCIDADE

      lcd.clear(); // ATUALIZA DILPLAY LCD
      lcd.setCursor(0, 0);
      lcd.print("Pontos:");
      lcd.setCursor(0, 1);
      lcd.print(pont);
    }
    tempo = 0; //RETORNA VARIAVEL DE CONTROLE TEMPO PARA ZERO
  }

  if (L1[0] == LOW)digitalWrite(Led[0], LOW); // LIGA E DESLIGA PARTES DO BAR-GRAPH DE ACORDO COM AS POSIÇÕES ATUALIZADAS
  else digitalWrite(Led[0], HIGH);
  if (L1[1] == LOW)digitalWrite(Led[1], LOW);
  else digitalWrite(Led[1], HIGH);
  if (L2[0] == LOW)digitalWrite(Led[2], LOW);
  else digitalWrite(Led[2], HIGH);
  if (L2[1] == LOW)digitalWrite(Led[3], LOW);
  else digitalWrite(Led[3], HIGH);
  if (L3[0] == LOW)digitalWrite(Led[4], LOW);
  else digitalWrite(Led[4], HIGH);
  if (L3[1] == LOW)digitalWrite(Led[5], LOW);
  else digitalWrite(Led[5], HIGH);

  Oesq = L4[0]; //QUARTA LINHA = LINHA DO JOGADOR
  Odir = L4[1];

  ////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////// ESTADOS QUE CONTROLAM O FUNCIONAMENTO DO JOGO //////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////// INICIO DO JOGO ///////////////////////////////
  if (INI) { // Altera as variaveis para o estado de inicio de jogo
    Oesq = 0;
    Odir = 0;
    Cesq = 1;
    Cdir = 0;

    L1[0] = 0;
    L2[0] = 0;
    L3[0] = 0;
    L4[0] = 0;
    L1[1] = 0;
    L2[1] = 0;
    L3[1] = 0;
    L4[1] = 0;
    novo = 0;

    pont = 0;
    tempo = 0;

    digitalWrite(Od, LOW);
    digitalWrite(Oe, LOW);
    digitalWrite(Ld, LOW);
    digitalWrite(Le, HIGH);
    
    //******************* EFEITO SONORO PARA O INICIO DO JOGO ************************
    tone(speakerPin, 599);
    delay(300);
    noTone(speakerPin);
    tone(speakerPin, 244);
    delay(300);
    noTone(speakerPin);
    tone(speakerPin, 599);
    delay(300);
    noTone(speakerPin);
    //********************************************************************************

    INI = 0; // sai do estado de inicio
  }
  ////////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////// COLISOES ////////////////////////////////////
  if (Cesq == 0 && Cdir == 1 && Oesq == 0 && Odir == 1) { // COLISAO A DIREITA
    for (int i = 0; i <= 3; i++) {  //REPETE EFEITO SONORO
      //******************* EFEITO SONORO DE COLISAO *************************
      tone(speakerPin, 199);
      delay(100);
      noTone(speakerPin);
      //**********************************************************************
      digitalWrite(Od, HIGH); //ANIMÇÃO DURANTE EFEITO SONORO
      digitalWrite(Ld, LOW);
      delay(250);
      digitalWrite(Od, LOW);
      digitalWrite(Ld, HIGH);
      delay(250);
    }

    while (INI != 1) {  // REPETE ENQUANTO O JOGO NÃO É REINICIADO

      digitalWrite(Od, HIGH); //ANIMACAO DE COLISAO APÓS EFEITO SONORO
      digitalWrite(Ld, LOW);
      delay(250);
      digitalWrite(Od, LOW);
      digitalWrite(Ld, HIGH);
      delay(250);

      Besq = digitalRead(Be); //LE BOTOES
      Bdir = digitalRead(Bd);

      if (Besq || Bdir) { //RESETA O JOGO
        INI = 1;
      }
    }
  }

  if (Cesq == 1 && Cdir == 0 && Oesq == 1 && Odir == 0) { // COLISAO A ESQUERDA
    for (int i = 0; i <= 3; i++) {  //REPETE EFEITO SONORO
      //****************** EFEITO SONORO DE COLISAO ***************************
      tone(speakerPin, 199);
      delay(100);
      noTone(speakerPin);
      //***********************************************************************
      digitalWrite(Oe, HIGH); //ANIMACAO DE COLISAO APÓS EFEITO SONORO
      digitalWrite(Le, LOW);
      delay(250);
      digitalWrite(Oe, LOW);
      digitalWrite(Le, HIGH);
      delay(250);
    }

    while (INI != 1) {  // REPETE ENQUANTO O JOGO NÃO É REINICIADO
      digitalWrite(Oe, HIGH); //ANIMACAO DE COLISAO APOS EFEITO SONORO
      digitalWrite(Le, LOW);
      delay(250);
      digitalWrite(Oe, LOW);
      digitalWrite(Le, HIGH);
      delay(250);

      Besq = digitalRead(Be); //LE BOTOES
      Bdir = digitalRead(Bd);

      if (Besq || Bdir) { //RESETA O JOGO
        INI = 1;
      }
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////// CARRO SEM OBSTACULOS ////////////////////////////////////
  if (Cesq == 0 && Cdir == 1 && Oesq == 0 && Odir == 0) { //CARRO A DIREITA
    Besq = digitalRead(Be); // LE BOTOES

    digitalWrite(Od, LOW);  
    digitalWrite(Oe, LOW);
    digitalWrite(Ld, HIGH);
    digitalWrite(Le, LOW);
    if (Besq == HIGH) { // MOVE CARRO PARA A ESQUERDA SE O BOTAO DA ESQUERDA FOR APERTADO
      Cdir = 0;
      Cesq = 1;
    }
  }


  if (Cesq == 1 && Cdir == 0 && Oesq == 0 && Odir == 0) { //CARRO A ESQUERDA
    Bdir = digitalRead(Bd); //LE BOTOES

    digitalWrite(Od, LOW);
    digitalWrite(Oe, LOW);
    digitalWrite(Ld, LOW);
    digitalWrite(Le, HIGH);
    if (Bdir == HIGH) { // MOVE CARRO PARA A DIREITA SE O BOTAO DA DIREITA FOR APERTADO
      Cdir = 1;
      Cesq = 0;
    }
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////// CARRO COM OBSTACULOS ////////////////////////////////////
  if (Cesq == 0 && Cdir == 1 && Oesq == 1 && Odir == 0) { // CARRO A DIREITA OBSTACULO A ESQUERDA
    Besq = digitalRead(Be); //LE BOTOES

    digitalWrite(Od, LOW);
    digitalWrite(Oe, HIGH);
    digitalWrite(Ld, HIGH);
    digitalWrite(Le, LOW);

    if (Besq) { // MOVE CARRO PARA A ESQUERDA SE O BOTAO DA ESQUERDA FOR APERTADO
      Cdir = 0;
      Cesq = 1;
    }
  }

  if (Cesq == 1 && Cdir == 0 && Oesq == 0 && Odir == 1) { // CARRO A ESQUERDA OBSTACULO A DIREITA
    Bdir = digitalRead(Bd); //LE BOTOES

    digitalWrite(Od, HIGH);
    digitalWrite(Oe, LOW);
    digitalWrite(Ld, LOW);
    digitalWrite(Le, HIGH);

    if (Bdir) { // MOVE CARRO PARA A DIREITA SE O BOTAO DA DIREITA FOR APERTADO
      Cdir = 1;
      Cesq = 0;
    }
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////

  ValorPot = analogRead(PinoPotenciometro); // LE VALOR DO POTENCIOMETRO
  vel = map(ValorPot, 0, 1023, 0, 10);  // ATUALIZA MULTIPLICADOR DE VELOCIDADE (QUANTO MAIOR O VALOR DO POTENCIOMETRO MAIOR É VEL)

  del = map(ValorPot, 0, 1023, 300, 100); // ATUALIZA O DELAY ENTRE ATUALIZAÇOES DE POSICAO (QUANTO MAIOR O VALOR DO POTENCIOMETRO MENOR É DEL)

  pwm = map(ValorPot, 0, 1023, 0, 255); // ATUALIZA A VELOCIDADE EM QUE O MOTOR TRABALHA
  analogWrite(motor, pwm);

  pct = map(pwm, 0, 255, 0, 100); // ATUALIZA A PORCENTAGEM DA VELOCIDADE EM QUE O MOTOR TRABALHA

  if (endereco == 50) endereco = 0; // MANTEM ENDEREÇO ENTRE 0 E 49

  EEPROM.write(endereco, pct);  // GUARDA A PORCENTAGEM NA MEMORIA

  pr = digitalRead(imprVel);  // VERIFICA BOTÃO DE IMPRESSAO

  if (pr) { // IMPRIME ULTIMOS 50 VALORES GRAVADOS EM ORDEM
    Serial.print("50 velocidades mais recentes");
    Serial.print("a porcentagem da velocidade do motor é medida a cada atualizacao na posicao dos obstaculos");

    for (int i = endereco + 1; i < 50; i++) {
      Serial.print("Velocidade: ");
      Serial.print(EEPROM.read(i));
      Serial.println("%");
    }

    for (int i = 0; i <= endereco; i++) {
      Serial.print("Velocidade: ");
      Serial.print(EEPROM.read(i));
      Serial.println("%");
    }
  }

  /////////////////////////////////////////

  tempo++;  // INCREMENTA O AUXILIAR TEMPO
}
