// Projeto 22 – Acendo LED com o Shield Ethernet

#include <SPI.h>
// Biblioteca utilizada para comunicação com o Arduino
#include <Ethernet.h>

// A linha abaixo permite definir o endereço físico (MAC ADDRESS) da placa...
//de rede.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

byte ip[] = { 192, 168, 2, 103 }; // Define o endereço IP.

// Porta onde estará aberta para comunicação Internet e Arduino.
EthernetServer server(80);

String readString;
int Pin = 9; //  Pino digital onde será ligado e desligado o LED.

void setup() {

  pinMode(Pin, OUTPUT); // Define o Pino 9 como saída.
  Ethernet.begin(mac, ip); // Chama o MAC e o endereço IP da placa Ethernet.
  //  Inicia o servidor que esta inserido junto a placa Ethernet.
  server.begin();
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        if (readString.length() < 100) {
          readString += c;
        }

        if (c == '\n') {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();

          // A partir daqui começa os códigos html.

          client.println("<HTML>");
          client.println("<BODY>");
          client.println("<H1>Acende LED</H1>");
          client.println("<H1>Projeto basico para demonstracao com Shield
                         Ethernet</H1>");
          client.println("<hr />");
          client.println("<br />");

          client.println("<a href=\"/facacomarduino/LedOn\"\">Ascender
                         led</a>");
          client.println("<a href=\"/facacomarduino/LedOff\"\">Apagar
                         led</a><br />");

          client.println("</BODY>");
          client.println("</HTML>");

          delay(1);
          client.stop();

          if (readString.indexOf("facacomarduino/LedOn") > 0)
          {
            digitalWrite(Pin, HIGH);  // Liga LED.
          }
          else {
            if (readString.indexOf("facacomarduino/LedOff") > 0)
            {
              digitalWrite(Pin, LOW);  // Desliga LED.
            }
          }
          readString = "";
        }
      }
    }
  }
}
