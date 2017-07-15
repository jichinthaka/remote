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
  Serial.println("qwe");
  if (results->decode_type == UNKNOWN) {
   output=String("Unknown:");
  }
  else if (results->decode_type == NEC) {
     output=String("NEC: ");

  }
  else if (results->decode_type == SONY) {
     output=String("SONY:");
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
  else{
    output=String("NOTDETECTD:");
  }

  String p2=String(results->address,DEC);
  output=String(output+p2+":");
  
  String i1=String(results->value,DEC);
  output=String(output+i1+":");
  
  String i2=String(results->bits, DEC);
  output=String(output+i2+":");
  count=min(count,10);
  String i3=String(count, DEC);
  output=String(output+i3+":");
  
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
//    output=String(output+":");
  }

  output=String(output+"");
  
  
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
     String read=String();   
     while(BT.available()){
      
        char a=(BT.read());
        read=String(read+a);
        delay(5);
     }
     if(read.equals("?READ?")){
      Serial.println("Calling reading function");
      sendToPhone();
     }
     if(read.equals("?WRITE?")){
      Serial.println("Calling Write function");
      receiveFromPhone();
     }
     delay(200);
   }  

}

void sendToPhone(){
      if (irrecv.decode(&results)) {
        String outputResult=prepareString(&results);
        Serial.println("SENDING DATA "+outputResult);
        for(int i=0;i<1;i++){
          BT.println("*****"+outputResult+"+++++");
          delay(20);
        }
        
  
        irrecv.resume(); // Receive the next value
      }

      Serial.println("End send to phone");
}

void receiveFromPhone(){  
  String input=String("");
  while(!BT.available()){
      delay(5);
  }
  while (BT.available())
  {
    char a=BT.read();
   input=String(input+a);
   delay(10);
  }
  Serial.println(input);
  emitIRSignal(input);
  
}

void emitIRSignal(String signal){
  signal.replace("*","");
  signal.replace("+","");
  Serial.println(signal);
  
  String brand=signal.substring(0,signal.indexOf(":"));
  signal=signal.substring(signal.indexOf(":")+1);
  Serial.println("Brand");
  Serial.println(brand);
  
  int address=(signal.substring(0,signal.indexOf(":"))).toInt();
  signal=signal.substring(signal.indexOf(":")+1);
  Serial.println("Address");
  Serial.println(address);
  
  unsigned long value=(signal.substring(0,signal.indexOf(":"))).toInt();
  signal=signal.substring(signal.indexOf(":")+1);
  Serial.println("Value");
  Serial.println(value);
  
  int bits=(signal.substring(0,signal.indexOf(":"))).toInt();
  signal=signal.substring(signal.indexOf(":")+1);
  Serial.println("bits");
  Serial.println(bits);
  
  int count=(signal.substring(0,signal.indexOf(":"))).toInt();
  signal=signal.substring(signal.indexOf(":")+1);
  Serial.println("Count");
  Serial.println(count);

  int *rawbuf=new int[count];
  int arrayCount=0;
  for(int i=0;i<count;i++){
    rawbuf[count]=0;
  }
  while(true){
    int item=(signal.substring(0,signal.indexOf("_"))).toInt();
    signal=signal.substring(signal.indexOf("_")+1);
    rawbuf[arrayCount]=item;
    arrayCount+=1;
    if(signal.indexOf("_")==-1){
      int item=(signal.substring(0)).toInt();
      signal=signal.substring(signal.indexOf("_")+1);
      rawbuf[arrayCount]=item;
      arrayCount+=1;
      break;
    }
  }


  if(brand.equalsIgnoreCase("NEC")){
    irsend.sendNEC(value, bits);
  }
  if(brand.equalsIgnoreCase("SONY")){
    irsend.sendSony(value, bits);
    Serial.println("SONY Signal sent");
  }
   
  if(brand.equalsIgnoreCase("PANASONIC")){
    irsend.sendPanasonic(value, bits);
  }

  if(brand.equalsIgnoreCase("JVC")){
    irsend.sendJVC(value, bits,false);
  }

  if(brand.equalsIgnoreCase("SAMSUNG")){
    irsend.sendSony(value, bits);
  }

  if(brand.equalsIgnoreCase("UNKNOWN")){
    unsigned int rawCodes[RAWBUF]; 

    bits=bits-1;
    for(int i=1;i<bits;i++){
        rawCodes[i - 1] = rawbuf[i]*USECPERTICK - MARK_EXCESS;
    }
    irsend.sendRaw(rawCodes, bits, 38);
  }

}

