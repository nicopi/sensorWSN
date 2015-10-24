/**
 * SensorServer
 * Sends data to ThingSpeak 
 *
 * October 2015, Nicola Piovesan
 */

import processing.serial.*;
import processing.net.*;

//CONFIGURATION
String APIKEY = ""; //your api key
String SERVER= "http://api.thingspeak.com/update?key="+APIKEY;
//END CONFIGURATION

Serial arduino;
Client c;
String data;
int number; //read from arduino
int sid, pid, temp;
int[] time=new int [3];
int totup=0;
String totupserv="";

void setup() {
  size(600, 400);
  //setup the serial port
  // List all the available serial ports:
  println(Serial.list());
  //Init the Serial object
  arduino = new Serial(this, Serial.list()[PORTNUM], 9600);
}

void draw() {
  background(50);
  fill(255);
  text("WSN Data Receiver", 10, 20);

  fill(100, 255, 0);
  text("Temperature:  " + temp, 10, 40);
  text("Received from sensor "+sid+" with packet "+pid,15,55);
  text("Last update received @ "+time[0]+":"+time[1]+":"+time[2]+". (Total updates in this session: "+totup+", updates on server: "+totupserv+")",15,70);
  if( data != null ) {
    fill(0, 255, 0);
    text("Server Response:", 10, 100);
    fill(200);
  }

  //Send new data to the server
  String ln;

  ln=arduino.readStringUntil('\n');
  if (ln!=null){
    println(ln);
    String[] m = match(ln, "Temperature: (.*)");
    if(m!=null){
      m[1]= m[1].substring( 0, m[1].length()-2 ); 
      temp=Integer.parseInt(m[1]);
      println(m[1]);
      
      time[0]=hour();
      time[1]=minute();
      time[2]=second();
      totup++;
      sendNumber(temp);
    }
    
    m=match(trim(ln), "Packet ID: (.*)");
    if(m!=null){
     // m[1]= m[1].substring( 0, m[1].length()-1 ); 
      pid=Integer.parseInt(m[1]);
      println(m[1]);
    }
    
    m=match(ln, "Sensor ID: (.*)");
    if(m!=null){
      m[1]= m[1].substring( 0, m[1].length()-2 ); 
      sid=Integer.parseInt(m[1]);
      println(m[1]);
    }
  }
}

void sendNumber(float num) {
  String url=SERVER+"&field1="+num;
  try { 
      java.io.BufferedReader reader= new java.io.BufferedReader(new java.io.InputStreamReader(new java.net.URL(url).openStream())); 
       String line= reader.readLine(); 
       while (line != null) { 
         System.out.println(line); 
         totupserv=line;
         line= reader.readLine(); 
       } 
     }  
     catch (java.io.IOException e) { 
       e.printStackTrace(); 
    } 
}