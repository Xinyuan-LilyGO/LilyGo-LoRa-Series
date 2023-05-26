/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 20/01/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - This test program has been written to check that hardware for reading the battery
  voltage has been assembled correctly such that it is funtional. The value defined as 'ADMultiplier'
  in settings.h is used to adjust the value read from the 91K\11K resistor divider and convert into mV.

  There is also an option of using a logic pin to turn the resistor divider used to read battery voltage on
  and off. This reduces current used in sleep mode. To use the feature set the define for pin BATVREADON
  in 'Settings.h' to the pin used. If not using the feature set the pin number to -1.

  Serial monitor baud rate is set at 9600
*******************************************************************************************************/
/*
******************************************************************************************************
  Program operation -

******************************************************************************************************
*/

#define ADMultiplier 6.36                      //adjustment to convert AD value read into mV of battery voltage 
#define BATVREADON 8                           //used to turn on the resistor divider to measure voltage
#define LED1 8                                 //pin for PCB LED  
#define SupplyAD A0                            //Resitor divider for battery connected here 

void loop()
{
  Serial.println(F("LED Flash"));
  led_Flash(4, 125);
  printSupplyVoltage();
  Serial.println();
  delay(1500);
}


void led_Flash(uint16_t flashes, uint16_t delaymS)
{
  uint16_t index;

  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(LED1, HIGH);
    delay(delaymS);
    digitalWrite(LED1, LOW);
    delay(delaymS);
  }
}


void printSupplyVoltage()
{
  //get and display supply volts on terminal or monitor
  Serial.print(F("Supply Volts "));
  Serial.print(readSupplyVoltage());
  Serial.println(F("mV"));
}


uint16_t readSupplyVoltage()
{
  //relies on internal reference and 91K & 11K resistor divider
  //returns supply in mV @ 10mV per AD bit read
  uint16_t temp;
  uint16_t volts = 0;
  byte index;

  if (BATVREADON >= 0)
  {
    digitalWrite(BATVREADON, HIGH);           //turn MOSFET connection resitor divider in circuit
  }

  analogReference(INTERNAL);
  temp = analogRead(SupplyAD);

  for (index = 0; index <= 4; index++)      //sample AD 5 times
  {
    temp = analogRead(SupplyAD);
    volts = volts + temp;
  }
  volts = ((volts / 5) * ADMultiplier);

  if (BATVREADON >= 0)
  {
    digitalWrite(BATVREADON, LOW);            //turn MOSFET connection resitor divider in circuit
  }

  return volts;
}


void setup()
{
  Serial.begin(9600);                       //setup Serial console ouput
  Serial.println();
  Serial.println(__FILE__);
  Serial.print(F("Compiled "));
  Serial.print(__TIME__);
  Serial.print(F(" "));
  Serial.println(__DATE__);
  Serial.println("45_Battery_Voltage_Read_Test Starting");

  pinMode(LED1, OUTPUT);                    //for PCB LED

  if (BATVREADON >= 0)
  {
    pinMode (BATVREADON, OUTPUT);             //for turning on resistor divider
  }
}

