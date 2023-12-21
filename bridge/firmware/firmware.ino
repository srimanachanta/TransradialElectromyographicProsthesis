bool enabled = false;
unsigned long enableTime = -1;
byte dataBuffer[20];

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // Enable the status LED
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // Check if Logger has sent any data to change the status
  if (Serial.available() > 0) {
    if (!enabled) {
      enabled = true;
      digitalWrite(LED_BUILTIN, HIGH);
      enableTime = micros();
    } else {
      enabled = false;
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  // Wait for enable message if disabled
  if (!enabled) return;

  writeLongToBuffer(micros() - enableTime, dataBuffer, 0);
  for (int i = 0; i < 6; i++) {
    int data = analogRead(A0 + i);
    dataBuffer[(i * 2) + 4] = lowByte(data);
    dataBuffer[(i * 2) + 5] = highByte(data);
  }
  writeLongToBuffer(micros() - enableTime, dataBuffer, 16);

  Serial.write(dataBuffer, 20);
}

void writeLongToBuffer(long n, byte buf[], int startPose) {
  buf[startPose] = (byte)n;
  buf[startPose + 1] = (byte)n >> 8;
  buf[startPose + 2] = (byte)n >> 16;
  buf[startPose + 3] = (byte)n >> 24;
}
