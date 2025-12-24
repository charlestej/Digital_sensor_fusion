#include <DHT.h>
#include <string.h>

// Pin Definitions...
#define TRIG_PIN 12
#define ECHO_PIN 11

#define DHT_PIN  2
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

// Thresholds...
const float NEAR_CM = 30.0;
const float HOT_C = 30.0;   // "C" as in Celsius (Temperature)
const float HUMID_P = 70.0;

// Read distance from HC-SR04 (cm)
float readDistanceCM() {
  // Trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo pulse width (microseconds), timeout after (approx.) 5m
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);

  if (duration == 0) return -1.0;

  // Speed of sound ≈ 343 m/s = 0.0343 cm/us
  return (duration * 0.0343f) / 2.0f;
}

// Sensor fusion logic - returns "LOW", "MED", "HIGH"
const char* fuseSensors(float distanceCM, float tempC, float humidity) {
  bool near  = (distanceCM > 0 && distanceCM < NEAR_CM);
  bool hot   = (!isnan(tempC) && tempC > HOT_C);
  bool humid = (!isnan(humidity) && humidity > HUMID_P);

  // HIGH - close object + hot/humid objects
  if (near && (hot || humid)) return "HIGH";

  // MED - close object + hot/humid objects
  if (near || (hot && humid)) return "MED";

  // LOW otherwise
  return "LOW";
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float distance = readDistanceCM();
  float humidity = dht.readHumidity();
  float tempC    = dht.readTemperature(); // Celsius

  const char* state = fuseSensors(distance, tempC, humidity);

  int stateLevel = (strcmp(state, "HIGH") == 0) ? 2 :
                   (strcmp(state, "MED")  == 0) ? 1 : 0;

  digitalWrite(LED_BUILTIN, (stateLevel == 2) ? HIGH : LOW);
  
  Serial.print(distance, 1);  Serial.print(" ");
  Serial.print(tempC, 1);     Serial.print(" ");
  Serial.print(humidity, 1);  Serial.print(" ");
  Serial.println(stateLevel);

  delay(500);
}
