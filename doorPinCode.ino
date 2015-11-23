#include <Keypad.h> //Library for the matrix keypad available from http://playground.arduino.cc/code/Keypad
#include <Wire.h>
#include <LiquidCrystal_I2C.h> //You must use F. Malpartida's new LCD library for Serial LCD https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home
#include <Servo.h> 
//define a lcd 
//0x3F adalah alamat LCD nya, jika tidak tampil bisa diubah jadi 0x20, atau cari datasheetnya
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
// define a servo instance
Servo myservo;
// define keypad mapping
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
// Define the Keymap. Note the position of # and * alter between models
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
// Connect keypad ROW0, ROW1, ROW2 and ROW3 (top to bottom) to these Arduino pins.
byte rowPins[ROWS] = { 9, 8, 7, 6 };
// Connect keypad COL0, COL1 and COL2 (left to right) to these Arduino pins.
byte colPins[COLS] = { 5, 4, 3, 2 }; 
 
// Create the Keypad instance
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
 
int ledGreen = 13;
int ledRed = 12;
int servoPin = 11; 
int buzzerPin = 10;
int salahMasukkin = 0;
int terkunci = 1;
//waktu jika salah masukkan kode 3x
int menit= 1; //dalam menit
int detik= 4;
String mastercode = "0000"; // default kode pin
void setup(){
   pinMode(ledGreen, OUTPUT); // pin for green LED
   pinMode(ledRed, OUTPUT); // pin for red LED
   
   myservo.attach(servoPin);
   
   lcd.begin (16,2); // initialise LCD
   
   myservo.write(0); // set servo position to one extent
 
   // Switch on the backlight
   lcd.backlight();
   // clear the LCD
   resetLcd();
}
 
void loop(){
  String enteredcode = ""; // the code you enter
  String reason = "   KODE SALAH   "; // the default reason why entry failed
  char key = kpd.getKey();
  if(key == 'A' && terkunci == 1) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Tkn C untk Batal");
    lcd.setCursor(0,1);
    lcd.print("Kode : ");
    // read a 4 digit code (you can have any length)
    for (int i = 0; i < 4; i++){
      char key1 = kpd.waitForKey();
      if(key1 == 'C'){
        i = 4; //skip to the end
        reason = "Batal Masuk"; 
      }
      if(key1 != NO_KEY){ // if you aren't reading no keypress
        tone(buzzerPin, 400, 20);
        lcd.print(key1); 
        enteredcode += key1; // append the keypress to the end of the entered code string
      }
    }
    if (enteredcode == mastercode) // if the code is correct
    {
      lcd.clear();
      lcd.setCursor (0, 0);
      lcd.print("   KODE BENAR   ");
      lcd.setCursor (0, 1);
      lcd.print("Pintu Terbuka!!!"); // print a success message
      digitalWrite(ledGreen, HIGH);
      myservo.write(180); // move the servo 180 degrees and open the door lock
      salahMasukkin = 0;
      terkunci = 0;
      
    }
    else {  
      // if the code is wrong or cancelled
      // tell the user what happened
      terkunci = 1;
      lcd.clear();
      lcd.setCursor (0, 0);
      lcd.print("   Informasi!   ");
      lcd.setCursor (0, 1);
      lcd.print(reason); // display the reason for failure. Incorrect code by default unless cancel is pressed
      digitalWrite(ledRed, HIGH); 
      tone(buzzerPin, 1000, 50);
      delay (2000); // wait 2 seconds before allowing a retry
      digitalWrite(ledRed, LOW); 
      //Cek 3 kali salah masukkan kode
      salahMasukkin++;
      if(salahMasukkin >= 3) {
        lcd.clear();
        lcd.setCursor (0, 0);
        lcd.print("   TERKUNCI!!   ");
        lcd.setCursor(0,1);
        lcd.print("COBA LAGI NANTI");
        ngeBlink(ledRed, 1, 100);
        digitalWrite(ledRed, HIGH);
        waktuTerkunci();
        digitalWrite(ledRed, LOW);
        salahMasukkin = 0;
      }
      resetLcd();
    }

  }
  if(key == 'B'){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("******BELL******");
    lcd.setCursor(0, 1);
    lcd.print("================"); 
    ngeBlink(ledGreen, 1, 500);
    tone(buzzerPin,440,500);
    delay(100);
    tone(buzzerPin,4699,500);
    delay(100);
    tone(buzzerPin,640,500);
    delay(100);
    tone(buzzerPin,3000,500);
    delay(100);
    resetLcd();
  }
  if(key =='C'){
    terkunci = 1;
    digitalWrite(ledGreen, LOW);
    myservo.write(0); // close the door lock
    resetLcd();
  }
  //Update Kode
  if(key == 'D' && terkunci == 0){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Update  Kode  ");
    lcd.setCursor(0, 1);
    lcd.print("Kode Baru = ");
    String kodeBaru = "";
    String pesanUbah = "Sukses Ubah Kode";
    for (int i = 0; i < 4; i++){
      char key2 = kpd.waitForKey();
      if(key2 == 'C'){ 
        i = 4; //skip to the end
        kodeBaru = mastercode;
        pesanUbah = "Batal Ubah Kode";
        break;
      }
      if(key2 != NO_KEY){ 
        tone(buzzerPin, 400, 20);
        lcd.print(key2); 
        kodeBaru += key2;
      }
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(pesanUbah);
    lcd.setCursor(0, 1);
    lcd.print("Kode = "+kodeBaru);
    mastercode = kodeBaru;
    ngeBlink(ledGreen, 2, 100);
    ngeBlink(ledRed, 2, 100);
    digitalWrite(ledGreen, HIGH);
  }
 
}
void ngeBlink(int pinLed, int ulang, int durasi){
    for(int i=0; i<=ulang; i++){
      digitalWrite(pinLed, HIGH);
      delay(durasi);
      digitalWrite(pinLed, LOW);
      delay(durasi);
    }
}
void resetLcd() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("*Selamat Datang*");
  lcd.setCursor(0, 1);
  lcd.print("Tkn A utk Masuk"); 
}
void waktuTerkunci(){
    for(int lim=(menit*60)+detik;lim>=0;lim--){
      lcd.setCursor(0,1);
      if(menit>0){
        lcd.print(menit);
        lcd.print(" mnt, ");
      } if(detik>=0){
        lcd.print(detik);
        lcd.print(" detik                ");
      }
      delay(1000);
      if(detik >0){
        detik -=1;
      }
      else if(menit>0){
        detik = 59;
        menit -=1;
      }
    }
}
