//#include <Ethernet_v2.h> //para segunda geracao do shield ethernet
#include <Ethernet.h> //para primeira geracao do shield ethernet
#include <SD.h>
#include <SPI.h>
#include <LiquidCrystal.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// mac 50-85-69-84-C3-86
IPAddress ip(192, 168, 1, 199);
EthernetServer server(80);

File webFile;

#define REQ_BUF_SZ    40
char HTTP_req[REQ_BUF_SZ] = { 0 };
char req_index = 0;

const int LM35 = A4;
const int LDR = A5;
LiquidCrystal  lcd(8, 9, 3, 2, 1, 0);

int luz = 0;
int temperatura = 0;
int temp = 0;
int ValorLido = 0;
int ValorLidoL = 0;

const int carga1 = 9;
int flag1 = 0;

void setup() {
  pinMode(carga1, OUTPUT);
  analogReference(INTERNAL);

  Ethernet.begin(mac, ip);
  server.begin();
  lcd.begin(16, 2);

  Serial.begin(9600);

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

}

void loop() {

  ValorLido = analogRead(LDR);
  ValorLidoL = analogRead(LM35);
  ValorLidoL = (float(analogRead(LM35)) * 5 / (1023)) / 0.01;
  Serial.print("Valor lido pelo LDR = ");
  Serial.println(ValorLido);
  Serial.print("Valor lido pelo LM35 = ");
  Serial.println(ValorLidoL);
  delay(500);

  EthernetClient client = server.available();

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

  }
}

void LerDados(EthernetClient novoCliente) {
  luz = analogRead(LDR);
  luz = map(luz, 0, 1023, 0, 100);
  novoCliente.print(luz);
  novoCliente.println("%");

  novoCliente.print("|");

  temperatura = analogRead(LM35);
  temperatura = temperatura * 0.1075268817204301;
  temp = (int)temperatura;
  novoCliente.print(temp);
  novoCliente.println("*C");

  novoCliente.print("|");

  novoCliente.print(flag1);

  novoCliente.print("|");

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
