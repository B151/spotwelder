// Simple spot welder timer with zero crossing detection and Velleman LCD shield VMA203 https://www.velleman.eu/downloads/29/infosheets/vma203_scheme.pdf
// inspiration  about zero crossing from here https://www.avdweb.nl/popular/spotwelder
// Andrejs Pukitis 2019.14.04

#include <LiquidCrystal.h>

const int bta = 12;      //optotriac
int lcd_key     = 0;
int adc_key_in  = 0; //analog in for buttons
const int sinusMax_eu = 5000; //1000ms divided by 50Hz divided by 4SinusPeriods equals 5000us
const byte zeroCrossPin = 11;   // input AC signal for zero crossing

int lastReportedPos = 1;
int lastReportedPos2 = 1;
volatile  int sec = 40; //delay in mS
volatile  int rep = 0; //repeat times


// define some values used by the panel and buttons

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5


LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int read_LCD_buttons()
{
  adc_key_in = analogRead(0);      // read the value from the sensor
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result

  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 150)  return btnUP;
  if (adc_key_in < 300)  return btnDOWN;
  if (adc_key_in < 500)  return btnLEFT;
  if (adc_key_in < 850)  return btnSELECT;
}

void setup() {

  // setup screen
  lcd.begin(16, 2);
  lcd.setCursor(2, 0);
  lcd.print("SPOT WELDER");
  lcd.setCursor(2, 1);
  lcd.print("    B151    ");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pulse:    mS");
  lcd.setCursor(0, 1);
  lcd.print("Repeat:   ");
  
  pinMode(bta, OUTPUT);
  pinMode(zeroCrossPin, INPUT);
}
void fire() {

  for (int i = 1; i <= rep; i++) {
   //sinusMax(); //wait for zero crossing
    digitalWrite(bta, HIGH);
    delay (sec);
    digitalWrite(bta, LOW);
    delay (sec);
  }
  delay(1000);

}
void sinusMax()
{
  //to prevent inrush current, turn-on at the sinus max
  while (digitalRead(zeroCrossPin)); //AC sinus is above zero do nothing
  while (!digitalRead(zeroCrossPin)); //AC sinus is below zero do nothing
  delayMicroseconds(sinusMax_eu); // just crossed zero at beginning of sinus wait until max
}

void loop()
{



  if (sec <= 9 ) {
    sec = 10;
    lastReportedPos = 11;
  }

  if (sec >= 201 ) {
    sec = 200;
    lastReportedPos = 199;
  }
  else
  { if (lastReportedPos != sec) {
      lcd.setCursor(7, 0);
      lcd.print("   ");
      lcd.setCursor(7, 0);
      lcd.print(sec);
      lastReportedPos = sec;
    }
  }

  if (rep <= 0 ) {
    rep = 1;
    lastReportedPos2 = 2;
  }

  if (rep >= 11 ) {
    rep = 10;
    lastReportedPos2 = 9;
  }
  else
  { if (lastReportedPos2 != rep) {
      lcd.setCursor(8, 1);
      lcd.print("   ");
      lcd.setCursor(8, 1);
      lcd.print(rep);
      lastReportedPos2 = rep;
    }
  }
  lcd_key = read_LCD_buttons();  // read the buttons

  switch (lcd_key)               // depending on which button was pushed, we perform an action
  {
    case btnRIGHT: {

        sec += 1;
        delay(250);
        break;
      }
    case btnLEFT: {

        sec -= 1;
        delay(250);
        break;
      }
    case btnUP: {

        rep += 1;
        delay(250);
        break;
      }
    case btnDOWN: {


        rep -= 1;
        delay(250);
        break;
      }
    case btnSELECT: {

        lcd.setCursor(11, 1);
        lcd.print("WELD!");
        delay(350);
        lcd.setCursor(11, 1);
        lcd.print("     ");
        fire();
        break;
      }
    case btnNONE: {
        break;
      }
  }
}

