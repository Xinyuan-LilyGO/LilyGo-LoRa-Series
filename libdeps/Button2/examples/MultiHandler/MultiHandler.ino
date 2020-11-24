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
  Serial.println("\n\nMulti Handler Demo");
  
  buttonA.setClickHandler(handler);
  buttonA.setLongClickHandler(handler);
  buttonA.setDoubleClickHandler(handler);
  buttonA.setTripleClickHandler(handler);
}

/////////////////////////////////////////////////////////////////

void loop() {
  buttonA.loop();
}

/////////////////////////////////////////////////////////////////

void handler(Button2& btn) {
    switch (btn.getClickType()) {
        case SINGLE_CLICK:
            break;
        case DOUBLE_CLICK:
            Serial.print("double ");
            break;
        case TRIPLE_CLICK:
            Serial.print("triple ");
            break;
        case LONG_CLICK:
            Serial.print("long");
            break;
    }
    Serial.print("click");
    Serial.print(" (");
    Serial.print(btn.getNumberOfClicks());    
    Serial.println(")");
}
/////////////////////////////////////////////////////////////////