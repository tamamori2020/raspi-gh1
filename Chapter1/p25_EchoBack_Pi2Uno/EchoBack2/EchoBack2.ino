void setup() {
  Serial.begin(115200,SERIAL_8N2);
}

void loop() {
  if (Serial.available()) {   //受信のチェック
    char  c = Serial.read();
    if (c == 'z'){            //zを受信したとき大文字のZを返します。
      Serial.write('Z');
    }else{
      Serial.write(c);   //エコーバック
    }
  }
}
