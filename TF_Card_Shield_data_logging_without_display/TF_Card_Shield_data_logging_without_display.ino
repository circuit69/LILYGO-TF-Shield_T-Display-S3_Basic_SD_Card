/*****************************************************
   Lilygo T-Display-S3 with a Lilygo TF Card Shield
   Basic writing of millisseconds and a random number to an
   SD card and then reading the file to the serial monitor
   The display is used in this sketch and is turned off
 ****************************************************/
#define PIN_BUTTON_1    0
#define PIN_BUTTON_2    14
#define PIN_BAT_VOLT    4

#define PIN_LCD_BL      38
#define PIN_POWER_ON    15
#define I2C_SDA         43
#define I2C_SCL         44

/* SD Card External expansion */
#define PIN_SD_CMD    13
#define PIN_SD_CLK    11
#define PIN_SD_D0     12

#include "FS.h"       //libraries for the sd card
#include "SD_MMC.h"

unsigned long sdDelay = 15000;  //15 second delay between writing to sd card
unsigned long sdTrigTime;// = millis() + sdDelay;
const char path[] = "/logs8.txt";   //filename to use on the sd card
int sdWrite = 0;      //number of record written since boot

void setup() {
  Serial.begin(115200);
  //setup on board buttons but not used in this sketch
  pinMode(PIN_BUTTON_1, INPUT);
  pinMode(PIN_BUTTON_2, INPUT);

  //Needed to power TFT on battery but turn it off this time
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, LOW);  //turn the screen off

  //assign the sd card pins and begin is required
  //                    clk=11       cmd=13     d0=12
  if (!SD_MMC.setPins(PIN_SD_CLK, PIN_SD_CMD, PIN_SD_D0)) {
    Serial.println("SD_MMC.setpins Failed");
    return;
  }
  //I could not find any documentation on these parameters but trial
  //and error got me to these settings, let me know if you figured it out
  if (!SD_MMC.begin("/sdcard", true, false, true)) {
    Serial.println("Card Mount Failed");
    return;
  }
  //end of the required setup

  //optional sd card info sent to the serial minitor
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD_MMC card attached");
    return;
  }
  Serial.print("SD_MMC Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);
  Serial.printf("Total space: %lluMB\n", SD_MMC.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD_MMC.usedBytes() / (1024 * 1024));
  //end of the optional sd card info
}

void loop() {
  unsigned int timeNow = millis();
  ////////check trigger time and log to sd card//////////////////////////////////////////////////////
  if (timeNow > sdTrigTime) {
    sdTrigTime = timeNow + sdDelay;   //set new trigger time to write data
    sdWrite++;                        //add 1 to number of records written
    String message = String(sdWrite) + ", " +
                     String(timeNow) + ", " +
                     String(random(100)) + "\r\n";    //a random number for useless data
    appendFile(SD_MMC, path, message.c_str());  //write to the sd card (routine below)
    readFile(SD_MMC, path);       //read the sd card to the serial monitor (routine below)
  }
  delay(1);
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  file.print(message);
  Serial.println("Append Successful!");
  file.close();
}

void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}
