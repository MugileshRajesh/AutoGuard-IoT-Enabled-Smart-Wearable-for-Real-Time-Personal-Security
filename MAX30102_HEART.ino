#include <Wire.h>
#include "MAX30105.h"  // Use SparkFun MAX3010X Library
MAX30105 particleSensor;

double avered = 0;
double aveir = 0;
double sumirrms = 0;
double sumredrms = 0;
int i = 0;
int Num = 100;
float ESpO2 = 95.0;
double FSpO2 = 0.7;
double frate = 0.95;

#define TIMETOBOOT 3000
#define SCALE 88.0
#define SAMPLING 100
#define FINGER_ON 30000
#define USEFIFO

void setup() {
  Serial.begin(115200);
  Wire.begin(4, 5); // SDA = D2 (GPIO4), SCL = D1 (GPIO5)

  Serial.println("Booting...");
  delay(3000);

  Serial.println("Initializing MAX30102...");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 NOT FOUND. Check wiring!");
    while (1);
  }

  byte ledBrightness = 0x7F;
  byte sampleAverage = 4;
  byte ledMode = 2;
  int sampleRate = 200;
  int pulseWidth = 411;
  int adcRange = 16384;

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
  particleSensor.enableDIETEMPRDY();
}

void loop() {
  uint32_t ir, red;
  double fred, fir;
  double SpO2 = 0;

#ifdef USEFIFO
  particleSensor.check();

  while (particleSensor.available()) {
    red = particleSensor.getFIFORed();
    ir = particleSensor.getFIFOIR();

    i++;
    fred = (double)red;
    fir = (double)ir;

    avered = avered * frate + fred * (1.0 - frate);
    aveir = aveir * frate + fir * (1.0 - frate);
    sumredrms += (fred - avered) * (fred - avered);
    sumirrms += (fir - aveir) * (fir - aveir);

    if ((i % SAMPLING) == 0 && millis() > TIMETOBOOT) {
      float irGraph = (2.0 * fir - aveir) / aveir * SCALE;
      float redGraph = (2.0 * fred - avered) / avered * SCALE;
      irGraph = constrain(irGraph, 80.0, 100.0);
      redGraph = constrain(redGraph, 80.0, 100.0);

      Serial.print("Red: "); Serial.print(red);
      Serial.print(", Infrared: "); Serial.print(ir); Serial.print("    ");

      if (ir < FINGER_ON) {
        Serial.println("No finger detected");
        break;
      }

      if (ir > FINGER_ON) {
        Serial.print("Oxygen % = ");
        Serial.print(ESpO2);
        Serial.println("%");
      }
    }

    if ((i % Num) == 0) {
      double R = (sqrt(sumredrms) / avered) / (sqrt(sumirrms) / aveir);
      SpO2 = -23.3 * (R - 0.4) + 100;
      ESpO2 = FSpO2 * ESpO2 + (1.0 - FSpO2) * SpO2;
      sumredrms = 0.0; sumirrms = 0.0; i = 0;
      break;
    }

    particleSensor.nextSample();
  }
#endif
}
