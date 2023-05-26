/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 14/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a simple program to test a GPS. It reads characters from the GPS using
  a hardware serial port, set to Serial1 as default, and then sends them (echoes) to the Arduino IDE
  serial monitor. If your ever having problems with a GPS (or just think you are) use this program first.

  If you get no data displayed on the serial monitor, the most likely cause is that you have the receive
  data pin into the Arduino (RX) pin connected incorrectly.

  At program start you should see '26A_GPS_Echo_Hardware_Serial Starting' in the serial monitor, if you
  dont the serial monitor baud rate is probably incorrectly set. If you then see data displayed on the
  serial terminal which appears to be random text with odd symbols its very likely you have the GPS
  serial baud rate set incorrectly.

  Change 'Serial1' in the program to match the hardware serial port you are using, for an Arduino Mega
  this would normally be Serial1, Serila2 or Serial3.

  Serial monitor baud rate is set at 115200.

*******************************************************************************************************/

void loop()
{
  while (Serial3.available())
  {
    Serial.write(Serial3.read());
  }
}


void setup()
{
  Serial3.begin(9600);
  Serial.begin(115200);
  Serial.println();
  Serial.println("26B_GPS_Echo_Hardware_Serial Starting");
}
