#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h"
#include "Start_Animation_New_Bold.h"

#define SS_PIN 15 //D8
#define RST_PIN 2 //D4
#define SERVO_PIN 0 //D3
#define PROXIMITY_PIN 3
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET    -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int pos = 0;
Servo gateServo;
MFRC522 mfrc522(SS_PIN, RST_PIN);

String card_list[2] = {"7066BA14", "C85721D"};
String user_info[2][3] = {{"Archishman D", "Rs.250", "12/24"}, {"Jeet C", "230", "6/25"}};
int total_card;
String card_num;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  gateServo.attach(SERVO_PIN); // Attach servo to specified pin
  gateServo.write(0); // Set initial position of the servo

  pinMode(PROXIMITY_PIN, INPUT);

  delay(500); // Wait for the servo to reach the initial position
  Serial.println();

  // Initialize I2C communication for the OLED display
  Wire.begin(4, 5); // SDA-GPIO4(D2), SCL-GPIO5(D1)
  delay(250); // Small delay to ensure the OLED display is properly reset

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) ; // Don't proceed, loop forever
  }

  display.setTextColor(WHITE); //this is often missed!!
  
  runStartupAnimation();

  displayInitialMessage();
}

void loop() {
  if (digitalRead(PROXIMITY_PIN) == LOW) {
      openExitGate();
  }
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  } else {
    card_num = getCardNumber();
    showData();
  }
}

String getCardNumber() {
  String UID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    UID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    UID += String(mfrc522.uid.uidByte[i], HEX);
  }
  UID.toUpperCase();
  return UID;
}

void showData() {
  String name, balance, expiry;
  boolean user_found = false;
  total_card = sizeof(card_list) / sizeof(card_list[0]);

  for (int i = 0; i < total_card; i++) {
    String check_num = card_list[i];
    if (card_num.equals(check_num)) {
      user_found = true;

      name = user_info[i][0];
      balance = user_info[i][1];
      expiry = user_info[i][2];

      Serial.print("Card ID : ");
      Serial.println(card_num);

      Serial.print("Name : ");
      Serial.println(name);

      Serial.print("Balance : ");
      Serial.println(balance);

      Serial.print("Expiry : ");
      Serial.println(expiry);

      Serial.println("------------");


      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextSize(2);
      display.print("  Access    Granted");

      display.setTextSize(1);
      display.setCursor(0,33);
      display.print("Card ID: ");
      display.println(card_num);

      display.setCursor(0,41);
      display.print("Name: ");
      display.println(name);

      display.setCursor(0,49);
      display.print("Balance: ");
      display.println(balance);

      display.setCursor(0,57);
      display.print("Expiry: ");
      display.println(expiry);

      display.display();

      delay(2000);
      for (pos = 0; pos <= 180; pos += 9) { // goes from 0 degrees to 180 degrees
        gateServo.write(pos); // tell servo to go to position in variable 'pos'
        // delay(5); // waits 5ms for the servo to reach the position
      }
      delay(5000);
      for (pos = 180; pos >= 0; pos -= 6) { // goes from 180 degrees to 0 degrees
        gateServo.write(pos); // tell servo to go to position in variable 'pos'
        // delay(10); // waits 10ms for the servo to reach the position
      }
      // Clear the display after the servo closes
      display.clearDisplay();
      display.display();

      delay(1000);
      
      runStartupAnimation();

      displayInitialMessage();
    }
    
  }
  if (!user_found) {
    Serial.print("Card ID : ");
    Serial.print(card_num);
    Serial.println(" have not registered.");
    Serial.println("------------");

    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(2);
    display.print("  Access    Denied!");
    display.setCursor(0,40);
    display.setTextSize(1);
    display.print("Card ID: ");
    display.println(card_num);
    display.display();

    delay(4000);
    display.clearDisplay();
    display.display();
    delay(1000);

    runStartupAnimation();

    displayInitialMessage();

  }
  delay(1000);
}

void displayInitialMessage() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Requesting to drive   vehicle   closer");
  display.display();
}

void runStartupAnimation() {
  // Forward animation loop (a goes from 0 to 200)
  for (int a = 0; a <= 200; a++) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.drawBitmap(0, 0, bitmap_Array[a], 128, 64, WHITE);
    display.display();
  }

  // Reverse animation loop (b goes from 199 to 0)
  for (int b = 199; b >=0; b--) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.drawBitmap(0, 0, bitmap_Array[b], 128, 64, WHITE);
    display.display();
  }
}



void openExitGate() {
  Serial.println("Proximity detected, opening exit gate.");
  
  for (pos = 0; pos <= 180; pos += 9) { // goes from 0 degrees to 180 degrees
    gateServo.write(pos); // tell servo to go to position in variable 'pos'
  }
  delay(5000);
  for (pos = 180; pos >= 0; pos -= 6) { // goes from 180 degrees to 0 degrees
    gateServo.write(pos); // tell servo to go to position in variable 'pos'
  }
  Serial.println("Exit gate closed.");
  Serial.println("------------");
}
