#define ENABLE_MESSAGE 0x1
#define DISABLE_MESSAGE 0x2
#define SYNC_ENABLE_TIME_MESSAGE 0x3

bool enabled = false;
unsigned long enableTime = -1;

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    uint8_t message = Serial.read();

    if(message == ENABLE_MESSAGE) {
      enableSensor();
    } else if(message == DISABLE_MESSAGE) {
      disableSensor();
    } else if(message == SYNC_ENABLE_TIME_MESSAGE) {
      resetEnableTime();
    }
  }

  if(!enabled) return;

  sendCurrentSensorFrame();
}

void resetEnableTime() {
  enableTime = micros();
}

void enableSensor() {
  digitalWrite(LED_BUILTIN, HIGH);
  enabled = true;
  resetEnableTime();
}

void disableSensor() {
  digitalWrite(LED_BUILTIN, LOW);
  enabled = false;
}

void sendCurrentSensorFrame() {
  // Send the timestamp for this data
  Serial.print(micros() - enableTime);
  Serial.print(",");
  for (int i = 0; i < 6; i++) {
    int data = analogRead(A0 + i);

    if(i == 5) {
      Serial.println(data);
    } else {
      Serial.print(data);
      Serial.print(",");
    }
  }
}
