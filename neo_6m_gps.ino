#include <TinyGPSPlus.h>

// Create TinyGPSPlus object
TinyGPSPlus gps;

// Use UART2 on ESP32 (Serial2), connect GPS TX → GPIO16, GPS RX → GPIO17
#define GPS_RX 16
#define GPS_TX 17
HardwareSerial ss(2); // UART2

void setup() {
  Serial.begin(115200);        // Debug output
  ss.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX); // GPS Serial

  Serial.println(F("ESP32 + NEO-6M GPS Example"));
  Serial.println(F("Waiting for GPS signal..."));
}

void loop() {
  // Feed GPS data
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }

  // Display data every second
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    lastPrint = millis();

    if (gps.location.isValid()) {
      Serial.print(F("Location: "));
      Serial.print(gps.location.lat(), 6);
      Serial.print(F(", "));
      Serial.print(gps.location.lng(), 6);
    } else {
      Serial.print(F("Location: INVALID"));
    }

    Serial.print(F("  Date/Time: "));
    if (gps.date.isValid() && gps.time.isValid()) {
      Serial.printf("%02d/%02d/%04d ", gps.date.day(), gps.date.month(), gps.date.year());
      Serial.printf("%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
    } else {
      Serial.print("INVALID");
    }

    Serial.println();
  }
}
