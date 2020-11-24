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
  
  // buttonA.setChangedHandler(changed);
  // buttonA.setPressedHandler(pressed);
  buttonA.setReleasedHandler(released);

  // buttonA.setTapHandler(tap);
  buttonA.setClickHandler(click);
  buttonA.setLongClickHandler(longClick);
  buttonA.setDoubleClickHandler(doubleClick);
  buttonA.setTripleClickHandler(tripleClick);
}

/////////////////////////////////////////////////////////////////

void loop() {
  button.loop();
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
void click(Button2& btn) {
    Serial.println("click\n");
}
void longClick(Button2& btn) {
    Serial.println("long click\n");
}
void doubleClick(Button2& btn) {
    Serial.println("double click\n");
}
void tripleClick(Button2& btn) {
    Serial.println("triple click\n");
}
void tap(Button2& btn) {
    Serial.println("tap");
}
/////////////////////////////////////////////////////////////////