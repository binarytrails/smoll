void setup()
{
  Serial.begin(9600);
}

void loop()
{
  //Serial.println('A');                      // A
  //Serial.println("A");                      // A
  
  //Serial.println(0);                        // 0
  //Serial.println(65);                       // 65
  
  //Serial.println(0x65);                     // 101 (hex -> dec)
  //Serial.println(101, HEX);                 // 65 (dec -> hex)

  /* multi byte character
   *  Each char -> 8 bits; cat * -> 24 bits;
   *  24 - 8 = 16 bits = 2 bytes (an int in Arduino)
   *  http://forum.arduino.cc/index.php?topic=234923.0
   */
  //Serial.println('123');                    // 12851
  
  //Serial.write(65); Serial.println("");     // A
  //Serial.write("A"); Serial.println("");    // A

  //Serial.write("A"); Serial.println("");    // A
  //Serial.println("--");
  //Serial.read(); Serial.println("");        // _

  /* Get the number of bytes available for reading
   * from the serial port. Think of it as a uni-directional
   * ring buffer with a moving cursor that restarts once it
   * reaches the end.
   */
  while (Serial.available() > 0)
  {
    Serial.println(Serial.available());
    
    int val = Serial.read();
    Serial.println(val);
    
    delay(1000);
  }
}
