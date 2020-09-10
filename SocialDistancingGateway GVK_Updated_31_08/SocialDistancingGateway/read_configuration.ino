void read_configuration()
{
  uint8_t time_out = 5000;
  uint8_t temp = 0;
  memset(sbuf, 0, sizeof(sbuf));
  
  //while (Serial.available())
  //  uint8_t dummy_data = Serial.read();

  currentMillis = millis();
  while ((Serial.available() < 5) && (millis() - currentMillis < time_out))
  {
    yield();
  }

  //Serial.print("\nSA:");
  //Serial.println(Serial.available());

  temp = Serial.available();
  
  if (Serial.available())
    for (int i = 0; i < temp; i++)
      sbuf[i] = Serial.read();

  //Serial.println();

//  for (int i = 0; i < 5; i++)
//  {
//    Serial.print(sbuf[i]);
//    Serial.print(" ");
//  }
}
