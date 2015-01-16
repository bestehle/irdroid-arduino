//irdroid.ino

#include <SPI.h>
#include <boards.h>
#include <ble_shield.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define IRpin_PIN PIND
#define IRpin 2

// the maximum duration of a pulse
#define TIMEOUT 65000

// time intervall between cheks if the IR Receiver is on HIGH or LOW
#define TIME_INTERVALL 20

#define FREQUENCY "006d "

void setup()
{  
  // Default pins set to 9 and 8 for REQN and RDYN
  // Set your REQN and RDYN here before ble_begin() if you need
  //ble_set_pins(3, 2);
  
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

int power[200] = {169,168,21,63,21,63,21,63,21,21,21,21,21,21,21,21,21,21,21,
63,21,63,21,63,21,21,21,21,21,21,21,21,21,21,21,21,21,63,21,21,21,21,21,21,21
,21,21,21,21,21,21,64,21,21,21,63,21,63,21,63,21,63,21,63,21,63,21,1794,169,
168,21,21,21,3694};

void loop()
{
	Serial.println("Read code!");


	ble_do_events();

	int length[1];
  	int* code = readIR(length);
  	
  	Serial.println(length[0]);
  	for (int i = 0; i < length[0]; i++) {
  		Serial.println(code[i]);
  	}

	//ble_write_bytes(code, sizeof(code));
	
	delay(5000);
	
	sendAsProntoHex(code, length[0]);

  	ble_do_events();
  	ble_do_events();
  	ble_do_events();
  	ble_do_events();
  	ble_do_events();
}

void sendAsProntoHex(int code[], int length) {
	char prontoCode[5];
	
	strcpy(prontoCode, "0000 "); 		// Dummy
	ble_write_bytes((unsigned char*) prontoCode, 5);
	strcpy(prontoCode, FREQUENCY); 		// Frequnecy
	ble_write_bytes((unsigned char*) prontoCode, 5);
	strcpy(prontoCode, "0000 "); 	// Sequenc 1
	ble_write_bytes((unsigned char*) prontoCode, 5);
	strcpy(prontoCode, "0000 "); 	// Sequenc 2
	ble_write_bytes((unsigned char*) prontoCode, 5);
	
		
	for (int i = 0; i < length; i++) {
		char* string = {"0000"};
		strcpy(prontoCode, string);
		dechex(code[i], prontoCode, 4, 0);
		strcpy(prontoCode + 4, " ");
		ble_write_bytes((unsigned char*) prontoCode, 5);
		Serial.print(i);
		Serial.print(" - ");
		Serial.println(prontoCode);
		if (i % 5 == 0)
			ble_do_events();
	}
}

int* readIR(int* lengthOut) {
  int high, low, count = 0; // time intervalls
  int code[200];  // char to send per bluetooth
  
  while (IRpin_PIN & (1 << IRpin)) { // pin is HIGH

     high++;
     delayMicroseconds(TIME_INTERVALL); // wait intervall


     if ((high >= TIMEOUT) && (count != 0)) { // != 0 because of the pause between signals
       logCode(code);
       lengthOut[0] = count;
       count = 0;
       return 0;
     }
  }
  // HIGH intervall
  code[count++] = high;
  Serial.println(high);
  
  // same as above
  while (! (IRpin_PIN & _BV(IRpin))) { // pin is LOW
  
     low++;
     delayMicroseconds(TIME_INTERVALL);
     
     if ((low >= TIMEOUT) && (count != 0)) {
       logCode(code);
       lengthOut[0] = count;
       count = 0;
       return code;
     }
  }
  
  // LOW intervall
  code[count++] = low;
  Serial.println(low);
}

void logCode(int* code) {
  Serial.println("\nReceived Signal");
  for (char i = 0; i < sizeof(code); i++) {
    Serial.print(code[i] * TIME_INTERVALL, DEC);
    Serial.println(" usec, ");
  }
}

char* dechex (unsigned dec, char *hex, int len, int fill) {
	int mhexdec[256];
	char mdechex[17] = "0123456789ABCDEF";
	
	char *hex2 = hex+len, *end = hex2;
	*hex2 = '\0';
	for (--hex2; ; hex2--) {
		*hex2 = mdechex[dec & 0xF];
		dec >>= 4;
		if (dec == 0) {
			break;
		}
	}
	// Make the string start at correct address.
	if (fill) {
		if (hex2 > hex) {
			char *c = hex, *s = hex2;
			for (c, s; s < end; s++) {
				*(c++) = *s;
			}
			// Fill with zeros at end
			for (c; c < end; c++) {
				*(c++) = '\0';
			}
		}
		return hex;
	}
	return hex2;
};
