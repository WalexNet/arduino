/** Librerias externas **/
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>


/************************** Constantes y Globales *****************************/
const byte ROWS = 4;                //four rows
const byte COLS = 4;                //four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {11,10, 9, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = { 7, 6, 5, 4}; //connect to the column pinouts of the keypad

// Este arreglo contiene las teclas utilizados en la KeyPad
uint8_t button[] = {"#*AB"};

// Variable global donde guardamos la tecla presionada
char tecla;

// Nota: Los siguientes "DEFINE" son únicamente para
// mejorar la lectura del código al momento de codificar.
#define BTN_MENU  0       // #
#define BTN_EXIT  1       // *
#define BTN_UP    2       // A
#define BTN_DOWN  3       // B

// Estado de nuestro autómata
#define S_HOME  0         // Estado Home, inicio
#define S_SET_R 1         // Estado Red, seteo del Rojo
#define S_SET_G 2         // Estado Green, seteo del Verde
#define S_SET_B 3         // Estado Blue, seteo del Azul

uint8_t estado = S_HOME;  // Constante estado

// *********** Información de color RGB
uint8_t r = 0;            // Valor para Red
uint8_t g = 0;            // Valor para Green
uint8_t b = 0;            // Valor para Blue


// Helpers ** Ayudantes
// Creamos el Teclado y el LCD
Keypad teclado = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
// Inicia el LCD en la dirección 0x27, con 16 caracteres y 2 líneas
LiquidCrystal_I2C lcd(0x27, 16, 2);  


void setup() {
  Serial.begin(9600); // Inicializamos el puerto serie
  lcd.init();         // initialize the lcd 
  lcd.backlight();    // Encendemos, la retroilumincion

  // Desplegar la info por defecto
  printHome();
}


/* Máquina de estados - Automata */ 
void loop() {
  tecla = teclado.waitForKey();
  switch(estado) {
    case S_HOME:        /*** INICIO ESTADO S_HOME ***/
      if(presionaBoton(BTN_MENU)) { // Transición BTN_MENU "#"
        estado = S_SET_R;
        printRed();
        break;
      }
      break;      // *** FIN ESTADO S_HOME ***
      // *************************************
      
    case S_SET_R:       /*** INICIO ESTADO S_SET_R ***/
      if(presionaBoton(BTN_MENU)) { // Transición BTN_MENU "#"
        estado = S_SET_G;
        printGreen();
        break;
      }
      if(presionaBoton(BTN_EXIT)) { // Transición BTN_EXIT "*"
        estado = S_HOME;
        printHome();
        break;
      }
      if(presionaBoton(BTN_UP))   { // Transición BTN_UP   "A"
        if(r<255) {
          r++;
        } else {
          r = 0;
        }
        printRed();
        break;
      }
      if(presionaBoton(BTN_DOWN)) { // Transición BTN_DWN  "B"
        if(r>0) {
          r--;
        } else {
          r = 255;
        }
        printRed();
        break;
      }
      break;      // *** FIN ESTADO S_SET_R ***
      // **************************************
      
    case S_SET_G:       /*** INICIO ESTADO S_SET_G ***/
      if(presionaBoton(BTN_MENU)) { // Transición BTN_MENU "#"
        estado = S_SET_B;
        printBlue();
        break;
      }
      if(presionaBoton(BTN_EXIT)) { // Transición BTN_EXIT "*"
        estado = S_HOME;
        printHome();
        break;
      }
      if(presionaBoton(BTN_UP))   { // Transición BTN_UP   "A" 
        if(g<255) {
          g++;
        } else {
          g = 0;
        }
        printGreen();
        break;
      }
      if(presionaBoton(BTN_DOWN)) { // Transición BTN_DWN  "B"
        if(g>0) {
          g--;
        } else {
          g = 255;
        }
        printGreen();
        break;
      }
      break;      // *** FIN ESTADO S_SET_G ***
      // **************************************
      
    case S_SET_B:       /*** INICIO ESTADO S_SET_B ***/
      if(presionaBoton(BTN_MENU)) { // Transición BTN_MENU "#"
        estado = S_SET_R;
        printRed();
        break;
      }
      if(presionaBoton(BTN_EXIT)) { // Transición BTN_EXIT "*"
        estado = S_HOME;
        printHome();
        break;
      }
      if(presionaBoton(BTN_UP))   { // Transición BTN_UP   "A"
        if(b<255) {
          b++;
        } else {
          b = 0;
        }
        printBlue();
        break;
      }
      if(presionaBoton(BTN_DOWN)) { // Transición BTN_DWN  "B"
        if(b>0) {
          b--;
        } else {
          b = 255;
        }
        printBlue();
        break;
      }
      break;      // *** FIN ESTADO S_SET_B ***
      // **************************************
  }

}

// Detectamos la tecla presionada '*', '#', 'A' o 'B'
// Asume la existencia de un arreglo button
uint8_t presionaBoton(int btn) {
  uint8_t result = (tecla == button[btn]); // button = '*', '#', 'A' o 'B'
  return result;
}

/******************* Utilitarios de dibujado ****************************/
void printHome() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("[MENU] Set RGB");
  printColor();
}

void printRed() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set RED");
  printColor();
}

void printGreen() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set GREEN");
  printColor();
}

void printBlue() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set BLUE");
  printColor();
}

void printColor() {
  lcd.setCursor(0,1);
  lcd.print("RGB: ");
  lcd.print(r, DEC);
  lcd.print(" ");
  lcd.print(g, DEC);
  lcd.print(" ");
  lcd.print(b, DEC);
}
