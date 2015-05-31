/irdroid.ino

#include <SPI.h>
#include <boards.h>
#include <ble_shield.h>

#define IRpin_PIN PIND
#define IRpin 2

// the maximum duration of a pulse
#define TIMEOUT 65000

// time intervall between cheks if the IR Receiver is on HIGH or LOW
#define TIME_INTERVALL 20

void setup()
{  
  // Default pins set to 9 and 8 for REQN and RDYN
  // Set your REQN and RDYN here before ble_begin() if you need
  //ble_set_pins(3, 2);
  
  // Set your BLE Shield name here, max. length 10
  ble_set_name("IrArduino");
  
  // Init. and start BLE library.
  ble_begin();
  
  // Enable serial debug
  
  Serial.begin(9600);
  
  Serial.println("Ready to receive IR codes");
}

unsigned char buf[16] = {0};
unsigned char len = 0;

unsigned char test[200] = { 100, 50, 100, 50 , 50, 50, 100, 50 , 50, 50, 100, 50 ,
    	50, 50, 100, 50 , 50, 50, 100, 50 , 50, 50, 100, 50 , 50, 50, 100, 50 ,
    	50, 50, 100, 50 , 50, 50, 100, 50 , 50, 50, 100, 50 , 50};


void loop()
{
	Serial.println("Read code!");
	
  	unsigned char* code = readIR();

	ble_write_bytes(code, sizeof(code));
  
  	ble_do_events();
}

unsigned char* readIR(void) {
  int high, low, count = 0; // time intervalls
  unsigned char code[200];  // char to send per bluetooth
  
  while (IRpin_PIN & (1 << IRpin)) { // pin is HIGH

     high++;
     delayMicroseconds(TIME_INTERVALL); // wait intervall


     if ((high >= TIMEOUT) && (count != 0)) { // != 0 because of the pause between signals
       logCode(code);
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
       logCode(code);
       count = 0;
       return code;
     }
  }
  
  // LOW intervall
  code[count++] = low;
}

void logCode(unsigned char* code) {
  Serial.println("\nReceived Signal");
  for (char i = 0; i < sizeof(code); i++) {
    Serial.print(code[i] * TIME_INTERVALL, DEC);
    Serial.println(" usec, ");
  }
}
