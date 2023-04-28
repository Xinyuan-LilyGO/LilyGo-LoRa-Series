//see the video here > https://www.youtube.com/watch?v=eeDC1m7ANJI&t=100s
//code here > https://gist.github.com/speters/f889faec42b510052a6ab4be437d38ca

//Purpose is to simply run a memory check on ATMEGA238P to test for counterfeit parts

#include <avr/boot.h>
#define SIGRD 5
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("");
  Serial.println("boot sig dump");
  int newLineIndex = 0;
  for (uint8_t i = 0; i <= 0x1F; i += 1) {
    Serial.print(boot_signature_byte_get(i), HEX);
    Serial.print("\t");
    newLineIndex++;
    if (newLineIndex == 8) {
      Serial.println("");
      newLineIndex = 0;
    }
  }
  Serial.println();
}

void loop() {

}
