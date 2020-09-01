
#include <LiquidCrystal_I2C.h>
 
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Inicia el LCD en la dirección 0x27, con 16 caracteres y 2 líneas
 
void setup()
{
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Hello, world!");
  lcd.setCursor(0,1);
  lcd.print("Walex-NET!");
  // lcd.home(); // Envia el cursor al home 0,0
  lcd.print(" Prueba");
}
 
void loop()
{
  //lcd.noBacklight();
  lcd.scrollDisplayLeft(); // Simpre movera el display completo
  delay(300);
}
