#include <SPI.h> 
#include <MFRC522.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#define SS_PIN 10 
#define RST_PIN 9 
LiquidCrystal_I2C lcd(0x27,16,2);

MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class
int code[] = {49,222,53,12}; //This is the stored UID (Unlock Card)
int codeRead = 0;
String uidString;
void setup() {
   Serial.begin(9600); 
   SPI.begin();       // Init SPI bus
   mfrc522.PCD_Init(); // Init MFRC522 
   Serial.println("Arduino RFID reading UID");
   lcd.begin();
   lcd.backlight();
   lcd.setCursor(0,0);
   lcd.print("Show your card:)");
}

void loop() {
if ( mfrc522.PICC_IsNewCardPresent())
    {
        if ( mfrc522.PICC_ReadCardSerial())
        {
           lcd.clear();
           Serial.print("Tag UID:");
           lcd.setCursor(0,0);
           lcd.print("Tag UID:");
           lcd.setCursor(0,1);
           for (byte i = 0; i < mfrc522.uid.size; i++) {  
                  Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "); 
                  Serial.print(mfrc522.uid.uidByte[i], HEX);
                  lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                  lcd.print(mfrc522.uid.uidByte[i], HEX);
                //  lcd.print(" ");
            }
            Serial.println();
            int i = 0;
            boolean match = true;
            while(i<mfrc522.uid.size)
            {
               if(!(int(mfrc522.uid.uidByte[i]) == int(code[i])))
               {
                  match = false;
               }
              i++;
            }
            delay(3000);
           lcd.clear();
           lcd.setCursor(0,0);
           if(match)
           {
              lcd.print("Registered");
              lcd.setCursor(0,1);
              lcd.print("TAG : @nimal1");
        }
        else{
             lcd.print("Not Registered!!"); 
             Serial.println("\nUnknown Card");
           }
             Serial.println("============================");
             mfrc522.PICC_HaltA();
             delay(3000); 
             reset_state();
    }
  }
}
void reset_state()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Show your card:)");
}
