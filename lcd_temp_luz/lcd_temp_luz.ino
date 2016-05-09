#include <LiquidCrystal.h> // include the LCD driver library

// Variaveis
float tempC = 0; // variavel de ponto flutuante para a temperatura em Graus Celcius
int tempPin = A4; // Declaração do pino de entrada analógica 0 (A0) no Arduino
float samples[8]; // matriz para armazenar oito amostras para o cálculo da temperatura média
float maxi = 0, mini = 100; // variáveis ​​de temperatura máxima/mínimo com valores iniciais
int i;

// Inicialização do display e pinos correspondentes
LiquidCrystal lcd(9, 8, 5, 4, 3, 2);

void setup()
{
  lcd.begin(16, 2); // configura o número de linhas e colunas do LCD
  lcd.clear(); // limpa LCD
}

void loop()
{
  // Início dos cálculos para o laço FOR
  for (i = 0; i <= 7; i++) { // colhe 8 amostras de temperatura
    samples[i] = ( 4.8 * analogRead(tempPin) * 100.0) / 1024.0; // conversão matemática para amostra legível de temperatura do LM35 e armazena resultado de conjunto de amostras. 1024 é a profundidade de bits (quantização) do Arduino.
    // 5 é a voltagem fornecida ao LM35. Alterar apropriadamente para ter a medida correta. No meu circuito é de 4.8Volts.

    // Exibição atual para temperatura em grau Celcius no LCD
    simbolotermometro();
    lcd.setCursor(0, 0);
    lcd.write(2);
    lcd.setCursor(1, 0); // set posição do cursor LCD
    lcd.print(" Atual: "); // imprime no LCD
    lcd.setCursor(9, 0);
    lcd.print(samples[i]); // imprime amostra da temperatrua atual no LCD
    simbolograu();
    lcd.setCursor(14, 0);
    lcd.write(1);
    lcd.print("C");

    tempC = tempC + samples[i]; // faz a adição de temperatura média
    delay(800); // aguarda 800ms
  } // FIM de faço FOR

  tempC = tempC / 8.0; // calculada a média de 8 amostras em grau Celcius

  if (tempC > maxi) {
    maxi = tempC; // grava a temperatura maxima na variavel maxi
  }
  if (tempC < mini) {
    mini = tempC; // grava a temperatura minima na variavel mini
  }

  // Envia resultados ao LCD.
  lcd.setCursor(0, 0);
  lcd.print("Media Max Min");
  lcd.setCursor(0, 1); // seta o cursor para a coluna 0, linha 1
  lcd.print(tempC); // imprime a temperatura media medida
  lcd.setCursor(6, 1);
  lcd.print(maxi); // imprime a temperatura maxima
  lcd.setCursor(12, 1);
  lcd.print(mini); // imprime a temperatura minima
  delay(3000); // Aguarde cerca de 3 segundos para exibir os resultados antes de iniciar o ciclo da sua tela LCD novamente
  tempC = 0; // Seta tempC a 0 para que os cálculos possam ser feitos novamente
} //Fim da rotina principal

void simbolograu()
{
  byte grau[8] = { // Cria um array de bytes com o simbolo de grau
    B00110,
    B01001,
    B00110,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000
  };
  lcd.createChar(1, grau); // Cria o caractere personalizado de grau
} // fim rotina simbolograu

void simbolotermometro()
{
  byte termometro[8] = { // Cria um array de bytes com o simbolo de termometro
    B00100,
    B01010,
    B01010,
    B01110,
    B01110,
    B11111,
    B11111,
    B01110
  };
  lcd.createChar(2, termometro); // Cria o caractere personalizado 1 (termometro)
} // fim rotina simbolotermometro
