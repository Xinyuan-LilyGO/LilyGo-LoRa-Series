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
  Serial.println("\n\nButton Demo");
  
  buttonA.setChangedHandler(changed);
  //buttonA.setPressedHandler(pressed);
  //buttonA.setReleasedHandler(released);

  // captures any type of click, longpress or shortpress
  buttonA.setTapHandler(tap);
}

/////////////////////////////////////////////////////////////////

void loop() {
  buttonA.loop();
}

/////////////////////////////////////////////////////////////////

void pressed(Button2& btn) {
    Serial.println("pressed");
}
void released(Button2& btn) {
    Serial.print("released: ");
    Serial.println(btn.wasPressedFor());
}
void changed(Button2& btn) {
    Serial.println("changed");
}
void tap(Button2& btn) {
    Serial.println("tap");
}
/////////////////////////////////////////////////////////////////

