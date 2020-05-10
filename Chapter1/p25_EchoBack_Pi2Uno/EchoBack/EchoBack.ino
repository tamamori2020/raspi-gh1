void setup() {
  Serial.begin(115200,SERIAL_8N2);
}

void loop() {
  if (Serial.available()) {   //受信のチェック
    char  c = Serial.read();
    Serial.write(c);   //エコーバック
  }
}
