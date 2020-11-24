/////////////////////////////////////////////////////////////////

#include "Button2.h";

/////////////////////////////////////////////////////////////////

#define BUTTON_A_PIN  2

/////////////////////////////////////////////////////////////////

Button2 buttonA = Button2(BUTTON_A_PIN);

/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  delay(50);
  Serial.println("\n\nLongpress Handler Demo");
  
  buttonA.setLongClickHandler(longpress);
}

/////////////////////////////////////////////////////////////////

void loop() {
  buttonA.loop();
}

/////////////////////////////////////////////////////////////////

void longpress(Button2& btn) {
    unsigned int time = btn.wasPressedFor();
    Serial.print("You clicked ");
    if (time > 1500) {
        Serial.print("a really really long time.");
    } else if (time > 1000) {
        Serial.print("a really long time.");
    } else if (time > 500) {
        Serial.print("a long time.");        
    } else {
        Serial.print("long.");        
    }
    Serial.print(" (");        
    Serial.print(time);        
    Serial.println(" ms)");
}

/////////////////////////////////////////////////////////////////