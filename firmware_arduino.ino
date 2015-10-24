/*
sensorWSN: Arduino firmware v0.1
  Setup: antenna RX on pin11

  October 2015, Nicola Piovesan.
*/

#include <Manchester.h>

byte BUF[4];
void setup()
{
  Serial.begin(9600);
  Serial.println("Device is ready");

  man.setupReceive(11,MAN_300);
  man.beginReceiveArray(4,BUF);
}

void loop() {
  if (man.receiveComplete()) {
    byte id=BUF[0]>>5;
    byte pkt_id=BUF[0] & 31;
    int value=(BUF[2] & 0x00FF)|((BUF[1]<<8));
    Serial.print("Sensor ID: ");
    Serial.println(id);
    Serial.print("Packet ID: ");
    Serial.println(pkt_id);
    Serial.print("Temperature: ");
    Serial.println(value);
    if((BUF[0]|BUF[1]|BUF[2])==BUF[3]){
      Serial.println("CRC okay");
    }else{
      Serial.println("CRC wrong");
    }
    Serial.println("------------");
    man.beginReceiveArray(4,BUF);
  }
}

