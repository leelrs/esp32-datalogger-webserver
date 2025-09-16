/*
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32
 *    D2       -
 *    D3       SS
 *    CMD      MOSI
 *    VSS      GND
 *    VDD      3.3V
 *    CLK      SCK
 *    VSS      GND
 *    D0       MISO
 *    D1       -
 */
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "helpers.h"
#include "datastruct.h"

int strainPin = 3; // PCB pin 15
int ledPin = 33; // PLED pin

int loggingFrequency = 100; // # of times per second

unsigned long previousMicros = 0;

File myFile;

const int bufferSize = 64;
dataStruct_t buffer[bufferSize];
int bufferIndex = 0;

bool ledState = false;


void setup() {
    Serial.begin(115200);

    pinMode(strainPin, INPUT);
    pinMode(ledPin, OUTPUT);

    digitalWrite(ledPin,HIGH);
    delay(500);
    digitalWrite(ledPin,LOW);

    delay(1000);

    if(!SD.begin()){
      Serial.println("Card Mount Failed");
      ledError();
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
      Serial.println("No SD card attached");
      ledError();
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
      Serial.println("MMC");
    } else if(cardType == CARD_SD){
      Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
    } else {
      Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    String fileName = getNextFileName(SD);
    Serial.print("Writing to file: ");
    Serial.println(fileName);

    // Open file
    myFile = SD.open(fileName, FILE_WRITE);
    if (myFile) {
      Serial.println("File opened successfully!");
    } else {
      Serial.println("Error opening the file!");
      ledError();
    }
}

void ledError() {
  while (true) {
    toggleLed();
    delay(1000);
  }
}

void toggleLed() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState ? HIGH : LOW);
}

void logData(dataStruct_t data) {
  buffer[bufferIndex++] = data;

  if (bufferIndex >= bufferSize) {
    myFile.write((const uint8_t *)buffer, sizeof(buffer));
    myFile.flush(); // Make sure to write to the card
    bufferIndex = 0; // Reset the buffer index

    toggleLed();
  }
}

void loop(){
  unsigned long currentMicros = micros();

  if ((currentMicros - previousMicros)/1000000 >= (1/loggingFrequency)) { //  >= 0.01
    dataStruct_t data;
    data.timeStamp_typ = (millis() << 6) | STRAIN4;
    data.data_long = analogRead(strainPin); // Do any math here
    Serial.println(analogRead(strainPin));
    logData(data);
  }
}
