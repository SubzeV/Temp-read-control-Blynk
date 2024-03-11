#define BLYNK_TEMPLATE_ID   "user7"
#define BLYNK_TEMPLATE_NAME "user7@server.wyns.it"
#define BLYNK_PRINT Serial

#define ANALOG_PIN 34 // Analog pin

int pin = 19;
int LEDPin = pin;

int ingesteldeTemp = 0;
int lastAppTemp = 0;

int potValue = 0;
int prevPotValue = 0;

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32AnalogRead.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP280 bmp; // I2C

unsigned long delayTime;

// Auth token
char auth[] = "g-XKdpm4G85L3PXLOtDss8LFHdqFaJl6";

// WiFi credentials.
char ssid[] = "embed";
char pass[] = "weareincontrol";

void setup() {
  Serial.begin(9600);

  Serial.println(F("BME280 test"));

  bool status;

  status = bmp.begin(0x76);

  delay (100);
  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    delay(1000);
  }

  Serial.println("-- Default Test --");
  delayTime = 1000;

  Serial.println();



  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  

  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");

  // Corrected Blynk.begin() parameters
  Blynk.begin(auth, ssid, pass, "server.wyns.it", 8081);

  // Additional lines to define the Blynk template ID and name
  #define BLYNK_TEMPLATE_ID   "user7"
  #define BLYNK_TEMPLATE_NAME "user7@server.wyns.it"

}

BLYNK_WRITE(V10) {
  ingesteldeTemp = param.asInt(); // Update the set temperature from the app slider
}

void loop() {
  Blynk.run();

  int tempRead = bmp.readTemperature() * 10;
  float temperatureReading = float(tempRead) / 10;
  Blynk.virtualWrite(V1, temperatureReading);

  float pressureReading = bmp.readPressure() / 100.0F;
  Blynk.virtualWrite(V2, pressureReading);

  // ----- Potentiometer/slider -----

  // Read the potentiometer value
  prevPotValue = potValue;

  potValue = int(map(analogRead(ANALOG_PIN), 0, 4095, 0, 30));
  delay(50);
  if (prevPotValue != potValue) {
    ingesteldeTemp = potValue;
    Blynk.virtualWrite(V10, ingesteldeTemp);
  }

  // Update the virtual pin with the current set temperature
  Blynk.virtualWrite(V3, ingesteldeTemp);

  // Check the temperature and control the LED accordingly
  if (temperatureReading <= ingesteldeTemp) {
    Serial.println("Temperature too low.");
    digitalWrite(LEDPin, HIGH);
    Blynk.virtualWrite(V4, 1023);
  } else if (temperatureReading >= ingesteldeTemp + 1) {
    Serial.println("Temperature too high.");
    digitalWrite(LEDPin, LOW);
    Blynk.virtualWrite(V4, 0);
  } else {
    Serial.println("Temperature OK.");
    digitalWrite(LEDPin, LOW);
    Blynk.virtualWrite(V4, 0);
  }

  delay(50);
}