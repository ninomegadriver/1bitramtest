/*
 * 1 BIT RAM TESTER
 * Nino MegaDriver - nino@nino.com.br
 * http://www.megadriver.com.br
 * 
 * License GPL v3.0
 *  
 * Pin configuration for Arduino Mega 2560:
 * 
 * [Arduino Pin]  [Chip Pina]  [Arduino Pins] 
 * 
 *       A0 <= [~CS         Vcc] => 12
 *       A1 <= [A0     H    Din] => 11
 *       A2 <= [A1     M    ~WE] => 10
 *       A3 <= [A2     2     A9] => 9
 *       A4 <= [A3     5     A8] => 8
 *       A5 <= [A4     1     A7] => 7
 *       A6 <= [Dout   0     A6] => 6
 *       A7 <= [GND          A5] => 5
 * 
 * NOTES:
 *       - Code also compatible with Uno, just change the pin configuration bellow.
 *       - You may use it with HM2510 or any other any x 1bit devices
 *       - Warning: using arduino pins as Vcc and GND, so don't use this setup with
 *                  devices that draw 40ma or more, otherwise you'll fry your arduino
 *                  if the device requires more current, isolate the pins and provide
 *                  an external power source
 * 
 */

// Pin configuration
uint8_t A[]  = { A1, A2, A3, A4, A5, 5, 6, 7, 8, 9};     // Address Inputs

                                                         // Power Pins
uint8_t    Vcc  = 12;                                    // IMPORTANT: Use only with devices that dosen't 
uint8_t    GND  = A7;                                    // draw more than 40ma or you'll fry your Arduino.
                                                         // If it needs nore, provide external power supply.

uint8_t    CS   = A0;                                    // Chip select or Chip Enable
uint8_t    Din  = 11;                                    // Data in Pin
uint8_t    Dout = A6;                                    // Data out Pin
uint8_t    WE   = 10;                                    // Write Enable pin
int device_size = 1024;                                  // Address range being tested from 0 to device_size


// Return the bit value at specified position
bool getBit(uint16_t data, uint8_t position)
{
    return (data >> position) & 0x1;
}

void setup() {
  Serial.begin(115200);
  for(int i=0;i<sizeof A;i++) pinMode(A[i], OUTPUT); // Address as outputs
  pinMode(Vcc,  OUTPUT); digitalWrite(Vcc, HIGH);    // Let's power the chip
  pinMode(GND,  OUTPUT); digitalWrite(GND, LOW);
  pinMode(Din,  OUTPUT);                             // Device's data in as output
  pinMode(Dout, INPUT);                              // Device's data out as input
  pinMode(CS,   OUTPUT);                             // CE as Output
  pinMode(WE,   OUTPUT);                             // WE as Output
}

// Set an address on the chip and return a String for debugging purposes
// Output String format:
// [Bit notation] [      Device Address pin      ]
// [    0xb     ] [A9 A8 A7 A6 A5 A4 A3 A2 A1 A0 ]
String setAddress(uint16_t address){
  String ret;
  uint8_t status;
  for(uint8_t i=0; i<sizeof A;i++){                  // Process all address pins
    status = getBit(address,i);                      // Get the correspondent bit on the address variable
    digitalWrite(A[i], status);                      // Set the correspondent address pin on the device
    ret = status + ret;                              // Concat the output string for debugging
  }
  ret = "0b" + ret;                                  // Add the bit notation
  return ret;
}


void loop() {
  char ret[100];
  uint8_t retval;
  int errors = 0;
  String address_bits;
  Serial.println("********************************************");
  Serial.println("*              1 BIT RAM TESTER            *");
  Serial.println("********************************************");
  Serial.println("*      Input any char to start testing     *");
  Serial.println("********************************************");
  while(!Serial.available() ){ } // Let's pause until the user is ready
  Serial.read();
  Serial.println("Writing 1 all over it...");
  for(uint16_t address=0; address<device_size;address++){
    digitalWrite(CS, HIGH); // Disable chip
    address_bits = setAddress(address); // Set the address
    digitalWrite(Din, HIGH); // would you pleas save that bit for me?
    digitalWrite(WE, LOW); // we're writing now
    digitalWrite(CS, LOW); // Enable chip;
  }
  Serial.println("Reading back...");
  for(uint16_t address=0; address<device_size;address++){
    digitalWrite(CS, HIGH); // Disable chip
    address_bits = setAddress(address); // Set the address
    digitalWrite(WE, HIGH); // This time we're reading not writing
    digitalWrite(CS, LOW); // Enable chip
    retval = digitalRead(Dout); // Device: Here's your bit sir!
    if(retval == 0) // Oops! It's supposed to be 1, this address is bad!
    {
      sprintf(ret, "[%s]%0.4X:%d => BAD!", address_bits.c_str(), address, retval);
      errors++;
      Serial.println(ret);
    }
    
  }
  if(errors>0) sprintf(ret, "Test result: BAD, %d bad addresse(s) found", errors);
  else sprintf(ret, "Test result: ALL GOOD!");
  Serial.println("********************************************");
  Serial.println(ret);

}
