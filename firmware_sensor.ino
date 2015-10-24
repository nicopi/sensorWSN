/*
 *  Temperature sensor firmware
 *  for ATtiny85 microcontrollers
 *  
 *  October 2015, Nicola Piovesan.
 *  
 *  The software reads the temperature value from the DS18B20 sensor connected on PB3 and
 *  represents the read value through leds connected on PB1 and PB3.
 * 
 *              |--------|
 *             -| *      |- VCC
 *        DATA -|        |- ANT
 *             -|        |- 
 *         GND -|        |-
 *              |--------|
 * 
 */

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Manchester.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define ONE_WIRE_BUS 3
#define ANTENNA_OUT 2
#define ANTENNA_SUPPLY 1
#define SENSOR_ID 1

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
volatile byte it=1;
//volatile byte BUFFER[20]; //For the future
volatile int wakeupnum=0;


void setup(void)
{
  cbi(ADCSRA,ADEN); // Switch ADC off, I don't need it
  setup_watchdog(9); // approximately 8 seconds sleep
  pinMode(ANTENNA_SUPPLY,OUTPUT);
  man.setupTransmit(ANTENNA_OUT, MAN_300);
  sensors.begin();
}

void loop(void)
{ 
  sensors.requestTemperatures(); // Send the command to get temperatures

  int number;
  number = sensors.getTempCByIndex(0); // Read the temperature and assign the read value to "number"
  powerOnAntenna();
  sendPacket(number,it);
  powerOffAntenna();
  it++;
  if(it>=32) it=1;
  deepsleep(75); //10 minutes
}

void sendPacket(int data, byte number){
  byte output[4];
  output[0]=(SENSOR_ID << 5) | (number); //first byte: ID + packet number;
  output[1]=((0xFF00 & data) >> 8);
  output[2]=(0x00FF & data);
  output[3]=output[0]|output[1]|output[2]; //CRC
  man.transmitArray(4,output);
}

void powerOnAntenna(){
  digitalWrite(ANTENNA_SUPPLY,HIGH);  
}

void powerOffAntenna(){
  digitalWrite(ANTENNA_SUPPLY,HIGH);  
}

void system_sleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_mode();                        // System sleeps here
  sleep_disable();                     // System continues execution here when watchdog timed out
}

void deepsleep(int moltiplicator){
  wakeupnum=0;
  while(wakeupnum<moltiplicator){
    wakeupnum++;
    system_sleep();  
  }
}
ISR(WDT_vect) {
    //nothing here
}


// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) {
  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;
 
  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}


