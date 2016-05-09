#include <LiquidCrystal.h> // include the LCD driver library
#include <Ethernet.h> //para primeira geracao do shield ethernet
#include <SD.h>
#include <SPI.h>
#include <MemoryFree.h>
#include <utility/w5100.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 199);
IPAddress gateway(192, 168, 1, 1); //Define o gateway
IPAddress subnet(255, 255, 255, 0); //Define a máscara de rede
EthernetServer server(80);

File webFile;

#define REQ_BUF_SZ    40
char HTTP_req[REQ_BUF_SZ] = { 0 };
char req_index = 0;

const int LM35 = A4;
const int LDR = A5;

// Variaveis
float tempC = 0; // variavel de ponto flutuante para a temperatura em Graus Celcius
int tempPin = A4; // Declaração do pino de entrada analógica 0 (A0) no Arduino
float samples[8]; // matriz para armazenar oito amostras para o cálculo da temperatura média
float maxi = 0, mini = 100; // variáveis ​​de temperatura máxima/mínimo com valores iniciais
int i;

int luz = 0;
int temperatura = 0;
int temp = 0;
int ValorLido = 0;
int ValorLidoL = 0;

const int carga1 = 10;
int flag1 = 0;

// Inicialização do display e pinos correspondentes
LiquidCrystal lcd(9, 8, 5, 4, 3, 2);

void setup() {
  lcd.begin(16, 2); // configura o número de linhas e colunas do LCD

  Ethernet.begin(mac, ip, gateway, subnet); //Inicializa o Ethernet Shield
  W5100.setRetransmissionTime(0x2710);
  W5100.setRetransmissionCount(5);

  server.begin();
  Serial.begin(9600);

  //Consulta Cartão sd e index.htm da pagina
  Serial.println("Inicializando cartao MicroSD...");
  if (!SD.begin(4)) {
    Serial.println("ERRO - iniciallizacao do cartao falhou!");
    return;
  }
  Serial.println("SUCESSO - cartao MicroSD inicializado.");

  if (!SD.exists("index.htm")) {
    Serial.println("ERRO - index.htm nao foi encontrado!");
    return;
  }
  Serial.println("SUCESSO - Encontrado arquivo index.htm.");
  delay(5);
}

void loop() {
  //debug dos sensores via console
  ValorLido = analogRead(LDR);
  ValorLidoL = analogRead(LM35);
  ValorLidoL = (float(analogRead(LM35)) * 5 / (1023)) / 0.01;
  //  Serial.print("Valor lido pelo LDR = ");
  //  Serial.println(ValorLido);
  //  Serial.print("Valor lido pelo LM35 = ");
  //  Serial.println(ValorLidoL);
  //  delay(500);

  samples[i] = ( 4.8 * analogRead(tempPin) * 100.0) / 1024.0; // conversão matemática para amostra legível de temperatura do LM35 e armazena resultado de conjunto de amostras. 1024 é a profundidade de bits (quantização) do Arduino.

  // Exibição atual para temperatura em grau Celcius no LCD
  //lcd.clear(); // limpa LCD
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
  simbolocoracao();
  lcd.setCursor(0, 1);
  lcd.write(3);
  lcd.setCursor(1, 1);
  lcd.print(" Luz  : "); // imprime no LCD
  lcd.print(ValorLido); // imprime no LCD

  delay(1000); // aguarda 500ms

  //inicio do servidor
  EthernetClient client = server.available();
  delay(5);

  if (client) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {

        char c = client.read();

        if (req_index < (REQ_BUF_SZ - 1)) {
          HTTP_req[req_index] = c;
          req_index++;
        }

        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();

          if (StrContains(HTTP_req, "ajax_LerDados")) {
            LerDados(client);
          }

          if (StrContains(HTTP_req, "ajax_carga1")) {
            SetCarga1();
          }

          else {

            webFile = SD.open("index.htm");
            if (webFile) {
              while (webFile.available()) {
                client.write(webFile.read());
              }
              webFile.close();
            }
          }
          Serial.println(HTTP_req);
          req_index = 0;
          StrClear(HTTP_req, REQ_BUF_SZ);
          break;
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }

    delay(1);
    client.stop();
    Serial.print(F("Memoria disponivel inicio: "));
    Serial.println(freeMemory());

  }

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

void simbolocoracao() {
  byte coracao[8] = { // vetor de bytes correspondentes ao desenho do coração
    B00000,
    B01010,
    B11111,
    B11111,
    B01110,
    B00100,
    B00000,
    B00000
  };
  lcd.createChar(3, coracao); // Cria o caractere personalizado 1 (coracao)
}

void LerDados(EthernetClient novoCliente) {
  luz = analogRead(LDR);
  //luz = map(luz, 0, 1023, 0, 100);
  novoCliente.print(luz);
  novoCliente.println("%");
  novoCliente.print("|");
  temp = (int)samples[i];
  novoCliente.print(temp);
  novoCliente.println("*C");
  novoCliente.print("|");
  novoCliente.print(flag1);
  novoCliente.print("|");
  delay(5);
  //espero receber algo como 90%|25*C|0|
}

void StrClear(char *str, char length) {
  for (int i = 0; i < length; i++) {
    str[i] = 0;
  }
}


char StrContains(char *str, char *sfind)
{
  char found = 0;
  char index = 0;
  char len;

  len = strlen(str);

  if (strlen(sfind) > len) {
    return 0;
  }

  while (index < len) {
    if (str[index] == sfind[found]) {
      found++;
      if (strlen(sfind) == found) {
        return 1;
      }
    }
    else {
      found = 0;
    }
    index++;
  }
  return 0;
}


void SetCarga1() {
  if (flag1 == 0) {
    digitalWrite(carga1, HIGH);
    flag1 = 1;
  }
  else {
    digitalWrite(carga1, LOW);
    flag1 = 0;
  }
}
