/*******************************************************************************************************
  Based on the OV2640 Arducam programs by;
  
  ArduCAM demo (C)2017 Lee
  Web: http://www.ArduCAM.com
 *******************************************************************************************************/


void setupOV2640(uint8_t resolution);
void myCAMSaveToSDFile2();
void myCAMSaveToSDFile(char *ACfilename, uint8_t ACfilenamesize);

void setupOV2640(uint8_t resolution)
{
  uint8_t vid, pid;
  uint8_t temp;
  //Reset the CPLD
  myCAM.write_reg(0x07, 0x80);
  delay(100);
  myCAM.write_reg(0x07, 0x00);
  delay(100);

  while (1) {
    //Check if the ArduCAM SPI bus is OK
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    temp = myCAM.read_reg(ARDUCHIP_TEST1);

    if (temp != 0x55) {
      Serial.println(F("OV2640 SPI interface Error!"));
      delay(1000); continue;
    } else {
      Serial.println(F("OV2640 SPI interface OK")); break;
    }
  }

  while (1) {
    //Check if the camera module type is OV2640
    myCAM.wrSensorReg8_8(0xff, 0x01);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))) {
      Serial.println(F("Can't find OV2640 module!"));
      delay(1000); continue;
    }
    else {
      Serial.println(F("OV2640 setup on I2C")); break;
    }
  }

  myCAM.set_format(JPEG);
  myCAM.InitCAM();

  myCAM.OV2640_set_JPEG_size(resolution);
}


void myCAMSaveToSDFile2()
{
  char str[8];
  byte buf[256];
  static int i = 0;
  static int k = 0;
  uint8_t temp = 0, temp_last = 0;
  uint32_t length = 0;
  bool is_header = false;
  File outFile;

  myCAM.flush_fifo();     //Flush the FIFO

  myCAM.clear_fifo_flag();   //Clear the capture done flag

  myCAM.start_capture();   //Start capture
  Serial.println(F("start Capture"));

  while (!myCAM.get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK));
  Serial.println(F("Capture Done"));

  length = myCAM.read_fifo_length();

  Serial.print(F("The fifo length is :"));
  Serial.println(length, DEC);

  if (length >= MAX_FIFO_SIZE) //384K
  {
    Serial.println(F("Over size."));
    return ;
  }
  if (length == 0 ) //0 kb
  {
    Serial.println(F("Size is 0."));
    return ;
  }

  k = k + 1;                       //Construct a file name
  itoa(k, str, 10);
  strcat(str, ".jpg");

  outFile = SD.open(str, O_WRITE | O_CREAT | O_TRUNC);   //Open the new file

  if (!outFile) {
    Serial.println(F("File open failed"));
    return;
  }
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();

  while ( length-- )
  {
    temp_last = temp;
    temp =  SPI.transfer(0x00);
    //Read JPEG data from FIFO
    if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
    {
      buf[i++] = temp;  //save the last  0XD9
      //Write the remain bytes in the buffer
      myCAM.CS_HIGH();
      outFile.write(buf, i);
      //Close the file
      outFile.close();
      Serial.print(str);
      Serial.println(F(" Image save OK"));
      is_header = false;
      i = 0;
    }
    if (is_header == true)
    {
      //Write image data to buffer if not full
      if (i < 256)
        buf[i++] = temp;
      else
      {
        //Write 256 bytes image data to file
        myCAM.CS_HIGH();
        outFile.write(buf, 256);
        i = 0;
        buf[i++] = temp;
        myCAM.CS_LOW();
        myCAM.set_fifo_burst();
      }
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      buf[i++] = temp_last;
      buf[i++] = temp;
    }
  }
}


void myCAMSaveToSDFile(char *ACfilename, uint8_t ACfilenamesize)
{
  byte buf[256];
  char ACfilename2[ACfilenamesize+1];
  static uint16_t i = 0;
  static uint16_t k = 0;
  uint8_t temp = 0, temp_last = 0;
  uint32_t length = 0;
  bool is_header = false;
  File dataFile;
  memset(ACfilename, 0, ACfilenamesize);   //fill ACfilename array with nulls.
  myCAM.flush_fifo();                      //Flush the FIFO
  myCAM.clear_fifo_flag();                 //Clear the capture done flag
  myCAM.start_capture();                   //Start capture

  Serial.println(F("Start capture "));

  while (!myCAM.get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK));
  Serial.println(F("Done"));
  length = myCAM.read_fifo_length();
  Serial.print(F("FIFO length "));
  Serial.println(length, DEC);

  if (length >= MAX_FIFO_SIZE) //384K
  {
    Serial.println(F("FIFO over size"));
    return ;
  }
  if (length == 0 ) //0 kb
  {
    Serial.println(F("FIFO size is 0"));
    return ;
  }

  //Construct a file name
  k = k + 1;
  itoa(k, ACfilename2, 10);
  strcat(ACfilename2, ".jpg");

  ACfilename[0] = '/';
  memcpy(ACfilename+1, ACfilename2, ACfilenamesize);
  
  //Serial.print(F("Adjusted filename "));
  //Serial.println(ACfilename);

  dataFile = SD.open(ACfilename, O_WRITE | O_CREAT | O_TRUNC);      //Open the new file

  if (!dataFile)
  {
    Serial.print(ACfilename);
    Serial.println(F(" File open failed"));
    return;
  }

  myCAM.CS_LOW();
  myCAM.set_fifo_burst();

  while ( length-- )
  {
    temp_last = temp;
    temp =  SPI.transfer(0x00);

    //Read JPEG data from FIFO
    if ( (temp == 0xD9) && (temp_last == 0xFF) )
    {
      buf[i++] = temp;                            //save the last  0XD9

      myCAM.CS_HIGH();
      dataFile.write(buf, i);                     //Write the remain bytes in the buffer

      dataFile.close();                           //Close the file
      Serial.print(ACfilename);
      Serial.println();
      is_header = false;
      i = 0;
      
    }

    if (is_header == true)
    {
      //Write image data to buffer if not full
      if (i < 256)
        buf[i++] = temp;
      else
      {
        //Write 256 bytes image data to file
        myCAM.CS_HIGH();
        dataFile.write(buf, 256);
        i = 0;
        buf[i++] = temp;
        myCAM.CS_LOW();
        myCAM.set_fifo_burst();
      }
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      buf[i++] = temp_last;
      buf[i++] = temp;
    }
  }
}
