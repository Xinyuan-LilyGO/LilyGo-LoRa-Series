//SD_Logger_Library.h
/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 02/04/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

void printModemSettingsSD();
void printOperatingSettingsSD();
void printOperatingModeSD(uint8_t opmode);
uint8_t setup_SDLOG();
void printDeviceSD();
void printSXBufferHEXSD(uint8_t start, uint8_t end);
void printHEXByteSD(uint8_t temp);
void packet_is_OKSD();
void packet_is_ErrorSD();
void printDigitsSD(int8_t digits);
void printtimeSD();


void printModemSettingsSD()
{
  uint8_t regdata;
  uint8_t sf;
  uint32_t bandwidth;
  uint8_t cr;
  uint8_t opt;
  uint16_t syncword;
  uint8_t  invertIQ;
  uint16_t preamble;
  uint32_t freqint;

  if (LORA_DEVICE == DEVICE_SX1272)
  {
    regdata = (LT.readRegister(REG_MODEMCONFIG1) & READ_BW_AND_2);
  }
  else
  {
    regdata = (LT.readRegister(REG_MODEMCONFIG1) & READ_BW_AND_X);
  }

  //get al the data frome the lora device in one go to avoid swapping
  //devices on the SPI bus all the time

  if (LORA_DEVICE == DEVICE_SX1272)
  {
    regdata = (LT.readRegister(REG_MODEMCONFIG1) & READ_BW_AND_2);
  }
  else
  {
    regdata = (LT.readRegister(REG_MODEMCONFIG1) & READ_BW_AND_X);
  }

  bandwidth = LT.returnBandwidth(regdata);
  freqint = LT.getFreqInt();
  sf = LT.getLoRaSF();
  cr = LT.getLoRaCodingRate();
  opt = LT.getOptimisation();
  syncword = LT.getSyncWord();
  invertIQ = LT.getInvertIQ();
  preamble = LT.getPreamble();

  printDeviceSD();
  logFile.print(F(","));
  logFile.print(freqint);
  logFile.print(F("hz,SF"));
  logFile.print(sf);

  logFile.print(F(",BW"));
  logFile.print(bandwidth);

  logFile.print(F(",CR4:"));
  logFile.print(cr);
  logFile.print(F(",LDRO_"));

  if (opt)
  {
    logFile.print(F("On"));
  }
  else
  {
    logFile.print(F("Off"));
  }

  logFile.print(F(",SyncWord_0x"));
  logFile.print(syncword, HEX);

  if (invertIQ == LORA_IQ_INVERTED)
  {
    logFile.print(F(",IQInverted"));
  }
  else
  {
    logFile.print(F(",IQNormal"));
  }
  logFile.print(F(",Preamble_"));
  logFile.print(preamble);
  logFile.flush();
}



void printOperatingSettingsSD()
{
  //get al the data frome the lora device in one go to avoid swapping
  //devices on the SPI bus all the time

  uint8_t ver = LT.getVersion();
  uint8_t pm = LT.getPacketMode();
  uint8_t hm = LT.getHeaderMode();
  uint8_t crcm = LT.getCRCMode();
  uint8_t agc = LT.getAGC();
  uint8_t lnag = LT.getLNAgain();
  uint8_t boosthf = LT.getLNAboostHF();
  uint8_t boostlf = LT.getLNAboostLF();
  uint8_t opmode = LT.getOpmode();

  printDeviceSD();
  logFile.print(F(","));

  printOperatingModeSD(opmode);

  logFile.print(F(",Version_"));
  logFile.print(ver, HEX);

  logFile.print(F(",PacketMode_"));

  if (pm)
  {
    logFile.print(F("LoRa"));
  }
  else
  {
    logFile.print(F("FSK"));
  }

  if (hm)
  {
    logFile.print(F(",Implicit"));
  }
  else
  {
    logFile.print(F(",Explicit"));
  }

  logFile.print(F(",CRC_"));
  if (crcm)
  {
    logFile.print(F("On"));
  }
  else
  {
    logFile.print(F("Off"));
  }


  logFile.print(F(",AGCauto_"));
  if (agc)
  {
    logFile.print(F("On"));
  }
  else
  {
    logFile.print(F("Off"));
  }

  logFile.print(F(",LNAgain_"));
  logFile.print(lnag);

  logFile.print(F(",LNAboostHF_"));
  if (boosthf)
  {
    logFile.print(F("On"));
  }
  else
  {
    logFile.print(F("Off"));
  }

  logFile.print(F(",LNAboostLF_"));
  if (boostlf)
  {
    logFile.print(F("On"));
  }
  else
  {
    logFile.print(F("Off"));
  }
  logFile.flush();
}



void printOperatingModeSD(uint8_t opmode)
{
  switch (opmode)
  {
    case 0:
      logFile.print(F("SLEEP"));
      break;

    case 1:
      logFile.print(F("STDBY"));
      break;

    case 2:
      logFile.print(F("FSTX"));
      break;

    case 3:
      logFile.print(F("TX"));
      break;

    case 4:
      logFile.print(F("FSRX"));
      break;

    case 5:
      logFile.print(F("RXCONTINUOUS"));
      break;

    case 6:
      logFile.print(F("RXSINGLE"));
      break;

    case 7:
      logFile.print(F("CAD"));
      break;

    default:
      logFile.print(F("NOIDEA"));
      break;
  }
}


uint8_t setup_SDLOG()
{
  //checks if the SD card is present and can be initialised
  //returns log number, 1-99, if OK, 0 if not

  uint8_t i;

  Serial.print(F("SD card..."));

  if (!SD.begin(SDCS))
  {
    Serial.println(F("ERROR SD card fail"));
    Serial.println();
    SD_Found = false;
    return 0;
  }

  Serial.print(F("Initialized OK - "));
  SD_Found = true;

  for (i = 1; i < 100; i++) {
    filename[4] = i / 10 + '0';
    filename[5] = i % 10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logFile = SD.open(filename, FILE_WRITE);
      break;
    }
  }

  Serial.print(F("Writing to "));
  Serial.println(filename);

  return i;
}


void printDeviceSD()
{

  switch (LORA_DEVICE)
  {
    case DEVICE_SX1272:
      logFile.print(F("SX1272"));
      break;

    case DEVICE_SX1276:
      logFile.print(F("SX1276"));
      break;

    case DEVICE_SX1277:
      logFile.print(F("SX1277"));
      break;

    case DEVICE_SX1278:
      logFile.print(F("SX1278"));
      break;

    case DEVICE_SX1279:
      logFile.print(F("SX1279"));
      break;

    default:
      logFile.print(F("Unknown Device"));

  }
}


void printHEXPacketSD(uint8_t *buffer, uint8_t size)
{
  uint8_t index;

  for (index = 0; index < size; index++)
  {
    printHEXByteSD(buffer[index]);
    logFile.print(F(" "));
  }
}


void printHEXByteSD(uint8_t temp)
{

  if (temp < 0x10)
  {
    logFile.print(F("0"));
  }
  logFile.print(temp, HEX);
}


void packet_is_OKSD()
{
  //uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, HIGH);
  }

  logFile.print(F(" FreqErrror,"));
  logFile.print(LT.getFrequencyErrorHz());
  logFile.print(F("hz  "));

  printHEXPacketSD(RXBUFFER, RXPacketL);

  logFile.print(F(" RSSI,"));
  logFile.print(PacketRSSI);
  logFile.print(F("dBm,SNR,"));
  logFile.print(PacketSNR);
  logFile.print(F("dB,Length,"));
  logFile.print(RXPacketL);
  logFile.print(F(",Packets,"));
  logFile.print(RXpacketCount);
  logFile.print(F(",Errors,"));
  logFile.print(errors);
  logFile.print(F(",IRQreg,"));
  logFile.println(IRQStatus, HEX);
  logFile.flush();
}


void packet_is_ErrorSD()
{
  //uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                    //get the IRQ status
  RXPacketL = LT.readRXPacketL();                    //get the real packet length

  if (IRQStatus & IRQ_RX_TIMEOUT)
  {
    logFile.print(F(" RXTimeout"));
  }
  else
  {
    logFile.print(F(" PacketError"));
    logFile.print(F(",RSSI,"));
    logFile.print(PacketRSSI);
    logFile.print(F("dBm,SNR,"));
    logFile.print(PacketSNR);
    logFile.print(F("dB,Length,"));
    logFile.print(RXPacketL);
    logFile.print(F(",Packets,"));
    logFile.print(RXpacketCount);
    logFile.print(F(",Errors,"));
    logFile.print(errors);
    logFile.print(F(",IRQreg,"));
    logFile.println(IRQStatus, HEX);
  }
  logFile.flush();
}


void printDigitsSD(int8_t digits)
{
  //utility function for digital clock display: prints preceding colon and leading 0
  logFile.print(F(":"));
  if (digits < 10)
    logFile.print('0');
  logFile.print(digits);
}


void printtimeSD()
{
  logFile.print(hour(recordtime));
  printDigitsSD(minute(recordtime));
  printDigitsSD(second(recordtime));
}



