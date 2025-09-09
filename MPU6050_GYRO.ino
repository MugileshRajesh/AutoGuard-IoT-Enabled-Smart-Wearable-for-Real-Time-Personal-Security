#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

// Thresholds for fall detection
float freeFallThreshold = 1.0;  // G-force near 0 during free fall
float impactThreshold = 2.0;   // High G-force on impact

bool inFreeFall = false;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA, SCL pins for ESP32

  Serial.println("Initializing MPU6050...");
  byte status = mpu.begin();
  if (status != 0) {
    Serial.print("MPU6050 failed with code: ");
    Serial.println(status);
    while (1);
  }

  Serial.println("Calibrating gyroscope...");
  mpu.calcGyroOffsets();
  Serial.println("MPU6050 ready!");
}

void loop() {
  mpu.update();

  float ax = mpu.getAccX();
  float ay = mpu.getAccY();
  float az = mpu.getAccZ();

  // Calculate total acceleration magnitude (in g's)
  float totalAccel = sqrt(ax * ax + ay * ay + az * az);

  Serial.print("Total Accel: ");
  Serial.print(totalAccel);
  Serial.print(" g | Status: ");

  if ( totalAccel < freeFallThreshold) {
    Serial.println("🟡 FREE FALL detected!");
    inFreeFall = true;
  } 
  else if ( totalAccel > impactThreshold) {
    Serial.println("🔴 IMPACT detected → FALL CONFIRMED!");
    inFreeFall = false;
    // You can add buzzer/vibration/alert here
  } 
  else {
    Serial.println("🟢 Normal");
  }

  delay(200); // Tune this based on responsiveness
}
