#include <SoftwareSerial.h>
#include <IRremote.h>

int RECV_PIN = 11;

IRrecv irrecv(RECV_PIN);

decode_results results;

SoftwareSerial BT(4, 2); 

IRsend irsend;

String prepareString(decode_results *results) {
 
  String output=String();
  String intermediate=String();
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
   output=String("Unknown:");
  }
  else if (results->decode_type == NEC) {
     output=String("NEC: ");

  }
  else if (results->decode_type == SONY) {
     output=String("SONY: ");
  }
  else if (results->decode_type == RC5) {
     output=String("RC5: ");
  }
  else if (results->decode_type == RC6) {
     output=String("RC6: ");
  }
  else if (results->decode_type == PANASONIC) {
    output=String("PANASONIC");
    String p1=String(results->address, HEX);
    output=String(output+p1);
  }
  else if (results->decode_type == LG) {
     output=String("Decoded LG: ");
  }
  else if (results->decode_type == JVC) {
     output=String("Decoded JVC: ");
  }
  else if (results->decode_type == AIWA_RC_T501) {
     output=String("Decoded AIWA RC T501: ");
  }
  else if (results->decode_type == WHYNTER) {
     output=String("Decoded Whynter: ");
  }

  String p2=String(results->address,HEX);
  output=String(output+p2+":");
  
  String i1=String(results->value,HEX);
  output=String(output+i1+":");
  
  String i2=String(results->bits, DEC);
  output=String(output+i2+":");
  
  for (int i = 1; i < count; i++) {
    if (i & 1) {
      String i4=String(results->rawbuf[i]*USECPERTICK, DEC);
      output=String(output+i4+"_");
    }
    else {
      output=String(output+'-');
      String i5=String((unsigned long) results->rawbuf[i]*USECPERTICK, DEC);
      output=String(output+i5+"_");
    }
    output=String(output+": ");
  }

  String i3=String(count, DEC);
  output=String(output+i3+":");
  
  Serial.println(output);
  return output;
}

void setup() {
  Serial.begin(9600); 
  BT.begin(9600);
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");

}

void loop() {

  if (BT.available())
  {
     char a=(BT.read());
     Serial.println(a);
     if(a=='+'){
      sendToPhone();
     }
     else if(a=='*'){
      receiveFromPhone();
     } 
   }  
/*
  if (irrecv.decode(&results)) {
    Serial.println("************************");
    String outputResult=prepareString(&results);
    delay(100);
    Serial.println("************************");
    irrecv.resume(); // Receive the next value
    BT.println("aaa");
//    BT.println(
//      results.decode_type+':'+
//      results.address+':'+
//      results.value+':'+
//      results.bits+':'+
//      results.&rawbuf+':'+
//      results.rawlen+':'+
//      results.overflow
//    );
    //BT.write(results.value);
  }
  delay(100);
  if (BT.available())
  {
   char a=(BT.read());
   Serial.println(a);
  }  
  */
}

void sendToPhone(){
    if (irrecv.decode(&results)) {
      String outputResult=prepareString(&results);
      delay(100);
      irrecv.resume(); // Receive the next value
      BT.println(outputResult);
    }
}

void receiveFromPhone(){
  String input=String("");
  while (BT.available())
  {
   input=String(input+(BT.read()));
  }
  Serial.println(input);
}

