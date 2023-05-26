/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 20/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-take-photo-save-microsd-card
  Github repository at https://github.com/RuiSantosdotme/ESP32-CAM-Arduino-IDE

  IMPORTANT!!!
   - Select Board "AI Thinker ESP32-CAM"
   - GPIO 0 must be connected to GND to upload a sketch
   - After connecting GPIO 0 to GND, press the ESP32-CAM on-board RESET button to put your board in flashing mode

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/


/*******************************************************************************************************
  Program Operation - This is a program using the ESP32CAM to take pictures and transmit those pictures via
  LoRa radio to another remote Arduino.

  This program is for an ESP32CAM board that has an SPI LoRa module set up on the following pins; NSS 12,
  NRESET 14, SCK 4, MISO 13, MOSI 2, RFBUSY 15, 3.3V VCC and GND. All other pins on the SX126X are not 
  connected.

  Note that the white LED on pin 4 or the transistor controlling it need to be removed so that the LoRa
  device can properly use pin 4.

  The program wakes up, takes a picture and starts the transfer of the picture (from its memory array in
  PSRAM) with LoRa, more details of the file transfer process will be found here;

  https://stuartsprojects.github.io/2021/09/20/Large-Data-Transfers-with-LoRa-Part3.html

  Note that if the camera fails then the program will attempt to send, and wait for the acknowledge, for a
  DTinfo packet reporting the fail.

  Serial monitor baud rate is set at 115200
*******************************************************************************************************/

#include <Arduino.h>
#include "FS.h"                                  //SD Card ESP32
#include "SD_MMC.h"                              //SD Card ESP32
#include "soc/soc.h"                             //disable brownout problems
#include "soc/rtc_cntl_reg.h"                    //disable brownout problems
#include "driver/rtc_io.h"

#include <SPI.h>
#include <SX126XLT.h>                            //get library here > https://github.com/StuartsProjects/SX12XX-LoRa  
#include <ProgramLT_Definitions.h>
#include "Settings.h"                            //LoRa and program settings 
SX126XLT LoRa;                                   //create a library class instance called LoRa, needed for ARtransferIRQ.h

#define ENABLEMONITOR                            //enable define to see progress messages in ARtransferIRQ.h
#define PRINTSEGMENTNUM                          //enable to print segment numbers as transfer progresses  
#define ENABLEARRAYCRC                           //enable this define to use and show CRCs
//#define DISABLEPAYLOADCRC                      //enable this define if you want to not use packet payload CRC checking
//#define DEBUG                                  //enable this define to show data transfer debug info

RTC_DATA_ATTR int16_t bootCount = 0;             //variables to save in RTC ram
RTC_DATA_ATTR uint16_t sleepcount = 0;
RTC_DATA_ATTR uint16_t pictureNumber = 0;        //number of picture taken, set to 0 on reset

#include "esp_camera.h"
camera_config_t config;                         //stores the camera configuration parameters
#include <ARtransferIRQ.h>                      //library of array transfer functions

bool SDOK;


void loop()
{
  SDOK = false;
  ARDTflags = 0;

  if (initMicroSDCard())                     //need to setup SD card before camera
  {
    Serial.println(F("SD Card OK"));
    SDOK = true;
  }
  else
  {
    Serial.println(F("****************************"));
    Serial.println(F("ERROR - SD Card Mount Failed"));
    Serial.println(F("****************************"));
    bitSet(ARDTflags, ARNoFileSave);
  }

  if (!configInitCamera())
  {
    bitSet(ARDTflags, ARNoCamera);             //set flag bit for no camera working
    Serial.println(F("Camera config failed"));
    Serial.println(F("Sending DTInfo packet"));
    setupLoRaDevice();
    ARsendDTInfo();
    startSleep();
  }
  else
  {
    if (takePhotoSend(PicturesToTake, PictureDelaymS))
    {
      //picture taken OK
      startSleep();
    }
    else
    {
      //picture take failed
      Serial.println("********************************");
      Serial.println("ERROR - Take picture send failed");
      Serial.println("********************************");
      Serial.println();
      Serial.println("Sending DTInfo packet");
      Serial.flush();
      bitSet(ARDTflags, ARNoCamera);             //set flag bit for no camera working
      setupLoRaDevice();
      ARsendDTInfo();
      startSleep();
    }
  }
  startSleep();
}

void startSleep()
{
  LoRa.setSleep(CONFIGURATION_RETENTION);
  rtc_gpio_hold_en(GPIO_NUM_4);
  rtc_gpio_hold_en(GPIO_NUM_12);              //hold LoRa device off in sleep
  esp_sleep_enable_timer_wakeup(SleepTimesecs * uS_TO_S_FACTOR);
  Serial.print(F("Start Sleep "));
  Serial.print(SleepTimesecs);
  Serial.println(F("s"));
  Serial.flush();
  sleepcount++;
  esp_deep_sleep_start();
  Serial.println("This should never be printed !!!");
}



bool initMicroSDCard()
{
  if (!SD_MMC.begin("/sdcard", true))               //use this line for 1 bit mode, pin 2 only, 4,12,13 not used
  {
    return false;
  }

  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println(F("Unknown SD card type"));
    return false;
  }

  return true;
}


void redFlash(uint16_t flashes, uint16_t ondelaymS, uint16_t offdelaymS)
{
  uint16_t index;

  pinMode(REDLED, OUTPUT);                    //setup pin as output

  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(REDLED, LOW);
    delay(ondelaymS);
    digitalWrite(REDLED, HIGH);
    delay(offdelaymS);
  }
  pinMode(REDLED, INPUT);                     //setup pin as input
}

bool setupLoRaDevice()
{
  SPI.begin(SCK, MISO, MOSI, NSS);

  if (LoRa.begin(NSS, NRESET, RFBUSY, LORA_DEVICE))
  {
    Serial.println(F("LoRa device found"));
  }
  else
  {
    Serial.println(F("LoRa Device error"));
    return false;
  }

  LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println();
  return true;
}


void setup()
{
  redFlash(4, 125, 125);

  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  //disable brownout detector
  rtc_gpio_hold_dis(GPIO_NUM_4);
  rtc_gpio_hold_dis(GPIO_NUM_12);             //LoRa NSS back to normal control after sleep

  pinMode(2, INPUT_PULLUP);
  digitalWrite(NSS, HIGH);
  pinMode(NSS, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  Serial.println(F(__FILE__));

  if (bootCount == 0)                         //run this only the first time after programming or power up
  {
    bootCount = bootCount + 1;
  }

  Serial.println(F("Awake !"));
  Serial.print(F("Bootcount "));
  Serial.println(bootCount);
  Serial.print(F("Sleepcount "));
  Serial.println(sleepcount);

#ifdef DISABLEPAYLOADCRC
  LoRa.setReliableConfig(NoReliableCRC);
#endif

  if (LoRa.getReliableConfig(NoReliableCRC))
  {
    Serial.println(F("Payload CRC disabled"));
  }
  else
  {
    Serial.println(F("Payload CRC enabled"));
  }
}


//***********************************************************************************************
// Start camera Code
//***********************************************************************************************

bool configInitCamera()
{
  Serial.println(F("Initialising the camera module "));

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;      //YUV422,GRAYSCALE,RGB565,JPEG

  //Select lower framesize if the camera doesn't support PSRAM
  if (psramFound())
  {
    Serial.println(F("PSRAM found"));
    config.frame_size = FRAMESIZE_SVGA;      //FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA, XUGA == 100K+, SVGA = 25K+
    config.jpeg_quality = 10;                //10-63 lower number means higher quality
    config.fb_count = 2;
  }
  else
  {
    Serial.println(F("No PSRAM"));
    config.frame_size = FRAMESIZE_XGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);   //Initialize the Camera
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    Serial.println();
    return false;
  }

  sensor_t * s = esp_camera_sensor_get();
  s->set_brightness(s, 0);     // -2 to 2
  s->set_contrast(s, 0);       // -2 to 2
  s->set_saturation(s, 0);     // -2 to 2
  s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
  s->set_wb_mode(s, 1);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
  s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
  s->set_aec2(s, 1);           // 0 = disable , 1 = enable
  s->set_ae_level(s, 0);       // -2 to 2
  s->set_aec_value(s, 450);    // 0 to 1200
  s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
  s->set_agc_gain(s, 0);       // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
  s->set_bpc(s, 1);            // 0 = disable , 1 = enable
  s->set_wpc(s, 0);            // 0 = disable , 1 = enable
  s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
  s->set_lenc(s, 0);           // 0 = disable , 1 = enable
  s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
  s->set_vflip(s, 0);          // 0 = disable , 1 = enable
  s->set_dcw(s, 1);            // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);       // 0 = disable , 1 = enable
  return true;
}


uint16_t  takePhotoSend(uint8_t num, uint32_t gapmS)
{
  uint8_t index = 1;
  char filenamearray[32];
  bool sentOK = false;
  String path;
  camera_fb_t  * fb = esp_camera_fb_get();

  for (index = 1; index <= num; index++)                     //take a number of pictures, send last
  {
    pictureNumber++;
    path = "/pic" + String(pictureNumber) + ".jpg";
    Serial.print("Next picture file name ");
    Serial.println(path.c_str());

    if (!fb)
    {
      Serial.println(F("*****************************"));
      Serial.println(F("ERROR - Camera capture failed"));
      Serial.println(F("*****************************"));
      delay(1000);
      pictureNumber--;                                       //restore picture number
      bitSet(ARDTflags, ARNoFileSave);
    }

    Serial.println(F("Camera capture success"));

#ifdef DEBUG
    Serial.print(F("First 8 bytes "));
    printarrayHEX(fb->buf, 0, 8);
    Serial.println();
    Serial.print(F("Last 8 bytes "));
    printarrayHEX(fb->buf, (fb->len - 8), 8);
    Serial.println();
#endif

    if (SDOK)
    {
      Serial.println(F("Save picture to SD card"));
      fs::FS &fs = SD_MMC;                            //save picture to microSD card
      File file = fs.open(path.c_str(), FILE_WRITE);
      if (!file)
      {
        Serial.println(F("*********************************************"));
        Serial.println(F("ERROR Failed to open SD file in writing mode"));
        Serial.println(F("*********************************************"));
        bitSet(ARDTflags, ARNoFileSave);
      }
      else
      {
        file.write(fb->buf, fb->len); // payload (image), payload length
        Serial.printf("Saved file to path: %s\r\n", path.c_str());
      }
      file.close();
    }
    else
    {
      Serial.println(F("***************"));
      Serial.println(F("No SD available"));
      Serial.println(F("***************"));
    }
  }

  SD_MMC.end();
  if (setupLoRaDevice())
  {
    Serial.print(F("Send with LoRa "));
    Serial.println(path.c_str());
    uint8_t tempsize = path.length();
    path.toCharArray(filenamearray, tempsize + 1);             //copy file name to the local filenamearray
    filenamearray[tempsize + 1] = 0;                           //ensure there is a null at end of filename in filenamearray
    sentOK = ARsendArray(fb->buf, fb->len, filenamearray, tempsize + 1); //pass array pointer and length across to LoRa send function
  }
  else
  {
    Serial.println(F("LoRa device not available"));
  }

  esp_camera_fb_return(fb);                                    //return the frame buffer back to the driver for reuse

  delay(gapmS);

  if (sentOK)
  {
    Serial.print(filenamearray);
    Serial.println(F(" Sent OK"));
    return pictureNumber;
  }
  else
  {
    Serial.print(filenamearray);
    Serial.println(F(" Send picture failed"));
  }
  return 0;
}


void printarrayHEX(uint8_t *buff, uint32_t startaddr, uint32_t len)
{
  uint32_t index;
  uint8_t buffdata;

  for (index = startaddr; index < (startaddr + len); index++)
  {
    buffdata = buff[index];
    if (buffdata < 16)
    {
      Serial.print(F("0"));
    }
    Serial.print(buffdata, HEX);
    Serial.print(F(" "));
  }
}
