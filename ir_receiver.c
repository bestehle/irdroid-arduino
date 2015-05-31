//ir_receiver.c

#include "ir_receiver.h"

unsigned char* readIR(void) {
  int high, low, count = 0; // time intervalls
  unsigned char code[200];  // char to send per bluetooth
  
  while (IRpin_PIN & (1 << IRpin)) { // pin is HIGH

     high++;
     delayMicroseconds(TIME_INTERVALL); // wait intervall


     if ((high >= TIMEOUT) && (count != 0)) { // != 0 because of the pause between signals
       log(code);
       count = 0;
       return 0;
     }
  }
  // HIGH intervall
  code[count++] = high;
  
  // same as above
  while (! (IRpin_PIN & _BV(IRpin))) { // pin is LOW
  
     low++;
     delayMicroseconds(TIME_INTERVALL);
     
     if ((low >= TIMEOUT) && (count != 0)) {
       log(code);
       currentpulse = 0;
       return code;
     }
  }
  
  // LOW intervall
  code[count++] = low;
}

void log(unsigned char* code) {
  Serial.println("\nReceived Signal");
  for (char i = 0; i < sizeof(code); i++) {
    Serial.print(pulses[i] * TIME_INTERVALL, DEC);
    Serial.println(" usec, ");
  }
}