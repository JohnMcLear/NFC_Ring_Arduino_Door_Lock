#include <EEPROM.h>

/**************************************************************************/
/*! 
    This example will attempt to connect to an ISO14443A
    card or tag and retrieve some basic information about it
    that can be used to determine what type of card it is.   
   
    Note that you need the baud rate to be 115200 because we need to print
    out the data and read from the card at the same time!

    
*/
/**************************************************************************/

int solanoidPin = 2;

// choose to SPI or I2C or HSU
#if 0
  #include <SPI.h>
  #include <PN532_SPI.h>
  #include "PN532.h"

  PN532SPI pn532spi(SPI, 10);
  PN532 nfc(pn532spi);
#elif 0
  #include <PN532_HSU.h>
  #include <PN532.h>
      
  PN532_HSU pn532hsu(Serial1);
  PN532 nfc(pn532hsu);
#else 
  #include <Wire.h>
  #include <PN532_I2C.h>
  #include <PN532.h>

  PN532_I2C pn532i2c(Wire);
  PN532 nfc(pn532i2c);
#endif

void setup(void) {
  pinMode(solanoidPin, OUTPUT);
  Serial.begin(115200);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print(versiondata);
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
    
  Serial.println("Waiting for an ISO14443A card");
}

void loop(void) {
  String ringUid;
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  if (success) {
    // Display some basic information about the card
    // Serial.println("Found an ISO14443A card");
    // Serial.print("  UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("  UID Value: ");
    for (uint8_t i = 0; i < uidLength; i++)
    {
      Serial.print("..");
      Serial.print(uid[i], HEX);
      ringUid += String(uid[i], HEX);

    }
    Serial.println(ringUid + "\n");
    if(ringUid == "4ac125a473484" || ringUid == "4bfc55a473480"){
      Serial.println("PERMISSION GRANTED");
      digitalWrite(solanoidPin, HIGH);   // sets the LED on
      delay(1000);                  // waits for a second
      digitalWrite(solanoidPin, LOW);    // sets the LED off
    }
  }
}
