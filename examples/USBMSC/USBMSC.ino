/**
 * @file      USBMSC.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2024  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2024-08-16
 * @note      If you are unable to upload the firmware after changing the USB mode, please put the board into download mode. 
 *            For how to enter download mode, please see README and FAQ.
 */

#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else
#include "USB.h"
#include "USBMSC.h"
#include "SD.h"


// T3-S3-V1.2 PINMAP
#define SDCARD_MOSI                 11
#define SDCARD_MISO                 2
#define SDCARD_SCLK                 14
#define SDCARD_CS                   13


#undef LED_BUILTIN
#define LED_BUILTIN                 37

SPIClass SDCardSPI(HSPI);
USBMSC MSC;

static  uint32_t DISK_SECTOR_COUNT = 0;
static  uint16_t DISK_SECTOR_SIZE = 0;   


static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
{
    // Serial.printf("MSC WRITE: lba: %lu, offset: %lu, bufsize: %lu\n", lba, offset, bufsize);
    uint32_t secSize = SD.sectorSize();
    if (!secSize) {
        return 0;  // disk error
    }
    for (int x = 0; x < bufsize / secSize; x++) {
        uint8_t blkbuffer[secSize];
        memcpy(blkbuffer, (uint8_t *)buffer + secSize * x, secSize);
        if (!SD.writeRAW(blkbuffer, lba + x)) {
            return 0;
        }
    }
    return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
{
    // Serial.printf("MSC READ: lba: %lu, offset: %lu, bufsize: %lu\n", lba, offset, bufsize);
    uint32_t secSize = SD.sectorSize();
    if (!secSize) {
        return false;  // disk error
    }
    for (int x = 0; x < bufsize / secSize; x++) {
        if (!SD.readRAW((uint8_t *)buffer + (x * secSize), lba + x)) {
            return 0;  // outside of volume boundary
        }
    }
    return bufsize;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject)
{
    Serial.printf("MSC START/STOP: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);
    return true;
}

static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == ARDUINO_USB_EVENTS) {
        arduino_usb_event_data_t *data = (arduino_usb_event_data_t *)event_data;
        switch (event_id) {
        case ARDUINO_USB_STARTED_EVENT: Serial.println("USB PLUGGED"); break;
        case ARDUINO_USB_STOPPED_EVENT: Serial.println("USB UNPLUGGED"); break;
        case ARDUINO_USB_SUSPEND_EVENT: Serial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en); break;
        case ARDUINO_USB_RESUME_EVENT:  Serial.println("USB RESUMED"); break;

        default: break;
        }
    }
}

void setup()
{
    Serial.begin(115200);

    Serial.setDebugOutput(true);

    pinMode(LED_BUILTIN, OUTPUT);

    SDCardSPI.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI);

    if (!SD.begin(SDCARD_CS, SDCardSPI, 16000000U)) {
        while (1) {
            Serial.println("SD FAILED!");
            digitalWrite(LED_BUILTIN, 1 - digitalRead(LED_BUILTIN));
            delay(1000);
        }
    } else {
        Serial.println("SD OK!");
    }

    USB.onEvent(usbEventCallback);
    MSC.vendorID("ESP32");       //max 8 chars
    MSC.productID("USB_MSC");    //max 16 chars
    MSC.productRevision("1.0");  //max 4 chars
    MSC.onStartStop(onStartStop);
    MSC.onRead(onRead);
    MSC.onWrite(onWrite);

    MSC.mediaPresent(true);
    // MSC.isWritable(true);  // true if writable, false if read-only

    DISK_SECTOR_COUNT = SD.numSectors();
    DISK_SECTOR_SIZE =  SD.sectorSize();

    MSC.begin(DISK_SECTOR_COUNT, DISK_SECTOR_SIZE);
    USB.begin();

    digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
    // put your main code here, to run repeatedly:
}
#endif /* ARDUINO_USB_MODE */
