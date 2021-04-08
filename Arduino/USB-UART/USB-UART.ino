#include <SoftwareSerial.h>
#define TX 2 // Желтый
#define RX 3 // Зеленый

SoftwareSerial softSerial(RX, TX);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    // ждём, пока не откроется монитор последовательного порта
    // для того, чтобы отследить все события в программе
  }
  Serial.print("Serial init OK\r\n");
  softSerial.begin(9600);
}

void loop() {
  if (softSerial.available()) {
    Serial.write(softSerial.read());
  }
  if (Serial.available()) {
    softSerial.write(Serial.read());
  }
}
