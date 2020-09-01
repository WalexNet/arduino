/*  Dibujo de Caracteres nuevos
 *  Con LCD 2x16 y Arduino
 *  By: www.elprofegarcia.com  componentes comprados en: www.dinastiatecnologica.com
 *  Programa para crear los nuevos caracteres o dibujos :
 */

#include <LiquidCrystal_I2C.h>

  
// defino los nuevo caracter o imagenes
byte letra1[8] = { B01110, B01110, B01110, B00100, B11111, B00100, B01010, B01010 }; // dibujo 1
byte letra2[8] = { B01110, B01110, B01110, B10101, B01110, B00100, B01010, B10001 }; // dibujo 2

LiquidCrystal_I2C lcd(0x27, 16, 2);   // Configura pines de salida para el LCD

void setup() {
  lcd.init();                 // Configura el LCD con 16 columnas y 2 filas
  lcd.backlight();
  lcd.createChar(1, letra1);      // asigna los nuevos caracteres por numeros
  lcd.createChar(2, letra2);

  lcd.clear();           // borrar Pantalla
} 

void loop() {
  lcd.setCursor(6, 1);   // Posicion del dibujo en 3,1
  lcd.write(1);      // Presenta el dibujo 1
  delay (500);           // Espera 500 mili segundos
  lcd.setCursor(6, 1);   // Posicion del dibujo en 6,1
  lcd.write(2);      // Presenta el dibujo 2
  delay (500);           // Espera 500 mili segundos
}
