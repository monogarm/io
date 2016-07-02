#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <utility/w5100.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 199);
IPAddress gateway(192, 168, 1, 1); //Define o gateway
IPAddress subnet(255, 255, 255, 0); //Define a máscara de rede
EthernetServer server(80);

char MakerIFTTT_Key[] = "dDGYqbaY_xYWmKU0jH86SK";
char MakerIFTTT_Event[] = "arduino";

const int LDR = A4;
const int LM35 = A5;
int luz = 0;
int ValorLido = 0;
float temperatura = 0;
int ADClido = 0;

//luz = map(luz, 0, 1023, 0, 100);
ADClido = analogRead(LM35);
temperatura = ADClido * 0.1075268817;

char *append_str(char *here, char *s) {
  while (*here++ = *s++)
    ;
  return here - 1;
}

char *append_ul(char *here, unsigned long u) {
  char buf[20];       // we "just know" this is big enough

  return append_str(here, ultoa(u, buf, 10));
}



void update_event() {
  EthernetClient client = EthernetClient();

  // connect to the Maker event server
  client.connect("maker.ifttt.com", 80);

  // construct the POST request
  char post_rqst[256];    // hand-calculated to be big enough

  char *p = post_rqst;
  p = append_str(p, "POST /trigger/");
  p = append_str(p, MakerIFTTT_Event);
  p = append_str(p, "/with/key/");
  p = append_str(p, MakerIFTTT_Key);
  p = append_str(p, " HTTP/1.1\r\n");
  p = append_str(p, "Host: maker.ifttt.com\r\n");
  p = append_str(p, "Content-Type: application/json\r\n");
  p = append_str(p, "Content-Length: ");

  // we need to remember where the content length will go, which is:
  char *content_length_here = p;

  // it's always two digits, so reserve space for them (the NN)
  p = append_str(p, "NN\r\n");

  // end of headers
  p = append_str(p, "\r\n");

  // construct the JSON; remember where we started so we will know len
  char *json_start = p;

  // As described - this example reports a pin, uptime, and "hello world"
  p = append_str(p, "{\"value1\":\"");
  p = append_ul(p, temperatura); //temperatura
  p = append_str(p, " C\",\"value2\":\"");
  p = append_ul(p, luz); //luminosidade
  p = append_str(p, " %\",\"value3\":\"");
  p = append_str(p, "hello, world!");
  p = append_str(p, "\"}");

  // go back and fill in the JSON length
  // we just know this is at most 2 digits (and need to fill in both)
  int i = strlen(json_start);
  content_length_here[0] = '0' + (i / 10);
  content_length_here[1] = '0' + (i % 10);

  // finally we are ready to send the POST to the server!
  client.print(post_rqst);
  client.stop();
}

// called once at system reset/startup time
void setup() {
  // this sets up the network connection, including IP addr via DHCP
  Ethernet.begin(mac);
  analogReference(INTERNAL);

  // the input pin for this example
  pinMode(A5, INPUT_PULLUP);
}

// how often to read the pins and update IFTTT
#define LOOP_DELAY_MSEC     (300*10000L)   // 60 minutes

// main body; called over and over if it ever returns
void loop() {

  // DHCP lease check/renewal (library only sends request if expired)
  Ethernet.maintain();

  // read the pins, send to IFTTT/Maker
  update_event();

  // only "this often"
  delay(LOOP_DELAY_MSEC);

}
