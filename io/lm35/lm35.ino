#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>

const int LM35 = 5;
float temperatura = 0;
int ADClido = 0;
void setup(){
Serial.begin(9600);
analogReference(INTERNAL);
//Se estiver usando Arduino Mega, use INTERNAL1V1
//se estiver usando Arduino Leonardo, remova esta linha pois o Leonardo não aceita
//este comando
}
void loop(){
ADClido = analogRead(LM35);
temperatura = ADClido * 0.1075268817; //no Leonardo use 0.4887585532
Serial.print("Temperatura = ");
Serial.print(temperatura);
Serial.println(" *C");
delay(1000);
}
