/** Librerias externas **/
#include <LiquidCrystal_I2C.h>


/************************** Constantes y Globales *****************************/

// Constantes y caracteres especiales
byte desmarcado[8] = { B11111, B10001, B10001, B10001, B10001, B10001, B10001, B11111 };
byte marcado[8]    = { B00000, B00001, B00011, B10110, B11100, B01000, B00000, B00000 };



// Nota: Los siguientes "DEFINE" son únicamente para
// mejorar la lectura del código al momento de codificar.
#define BTN_EXIT  0         // B1
#define BTN_MENU  1         // B2
#define BTN_SEL   2         // B3
#define BTN_SET   3         // B4

// Estado de nuestro autómata
#define S_OFF     0         // Estado OFF (Stand By), inicio
#define S_MENU    1         // Estado MENU, maquina encendida, empezamos a movernos por el menu
#define S_AUTO    2         // Estado AUTO, modo Automático
#define S_MANUAL  3         // Estado MANUAL, nodo de funcionamiento manual
#define S_INFO    4         // Estado INFO, muestra información del estado activo
#define S_CONFIG  5         // Estado CONFIG, configuracion del automata, Fecha/Hora y Nro de Box
#define S_ANO     50        // Sub-Estado CONFIG, configura el Año        0000
#define S_MES     51        // Sub-Estado CONFIG, configura el Mes        00
#define S_DIA     52        // Sub-Estado CONFIG, configura el Día        00
#define S_HORA    53        // Sub-Estado CONFIG, configura la Hora       00
#define S_MIN     54        // Sub-Estado CONFIG, configura los Minutos   00
#define S_BOX     55        // Sub-Estado CONFIG, configura el Nro de BOX 00

// Pines a Usar en el PIC
#define LED_OFF   6         // OFF Stand By (Led Rojo)
#define LED_ON    7         // ON Activo (Led Verde)
#define K1        11        // Rele K1
#define K2        10        // Rele K2
#define K3        9         // Rele K3
#define K4        8         // Rele K4
uint8_t button[]={5,4,3,2}; // Este arreglo contiene los pines de los botones B1, B2, B3, B4 respectivamente

/************** VARIABLES **************/
uint8_t estado            = S_OFF;  // Variable estado actual
int     vaca              = 1;      // Identificador de la vaca
double  lit               = 0;      // Litros de leche ordeñados
boolean automatico        = false;  // Variable global del estado de automatico
boolean manual            = false;  // Variable global del estado de manual
uint8_t button_estate[4];           // Este arreglo contiene el último estado conocido de cada línea
unsigned long tempo       = 0;      // Tiempo del ordeñe
String  tiempo;
int     aaaa              = 2020;      // Variable Año
int     mm                = 1;      // Variable Mes        
int     dd                = 1;      // Variable Día
int     hh                = 0;      // Variable Horas
int     ii                = 0;      // Variable Minutos
int     box               = 0;      // El Nº de BOX donde ira el micro



// ******************* Helpers / Ayudantes **********************
// Creamos el LCD
// Inicia el LCD en la dirección 0x27, con 16 caracteres y 2 líneas
LiquidCrystal_I2C lcd(0x27, 16, 2);



void setup() {
  // Configuramos Botones como PULL-UP para ahorrar resistencias
  pinMode(button[BTN_MENU], INPUT_PULLUP);
  pinMode(button[BTN_EXIT], INPUT_PULLUP);
  pinMode(button[BTN_SEL],  INPUT_PULLUP);
  pinMode(button[BTN_SET],  INPUT_PULLUP);

  // Configuramos Leds
  pinMode(LED_ON,  OUTPUT);
  pinMode(LED_OFF, OUTPUT);
  digitalWrite(LED_ON,  LOW);
  digitalWrite(LED_OFF, LOW);

  // Configuramos Reles
  pinMode(K1, OUTPUT);
  pinMode(K2, OUTPUT);
  pinMode(K3, OUTPUT);
  pinMode(K4, OUTPUT);
  digitalWrite(K1, LOW);
  digitalWrite(K2, LOW);
  digitalWrite(K3, LOW);
  digitalWrite(K4, LOW);

  // Se asume que el estado inicial es HIGH
  button_estate[0] = HIGH;
  button_estate[1] = HIGH;
  button_estate[2] = HIGH;
  button_estate[3] = HIGH;
  
  Serial.begin(9600);             // Inicializamos el puerto serie
  lcd.init();                     // initialize the lcd
  lcd.createChar(0, desmarcado);  // Creamos el caracter de desmarcado
  lcd.createChar(1, marcado);     // Creamos el caracter de marcado
  // Desplegar la info por defecto
  digitalWrite(LED_OFF, HIGH); 
  printOff();
}


/* Máquina de estados - Automata */
void loop() {
  switch (estado) {
    case S_OFF:        /*** INICIO ESTADO S_OFF ***/
      if (presionaBoton(BTN_MENU)) { // Transición BTN_MENU "B2" de OFF a MENU
        // Pasamos al estado menu
        estado = S_MENU;
        digitalWrite(LED_OFF, LOW);
        digitalWrite(LED_ON, HIGH);        
        printMenu();
        break; // Fin estado Menu
      }
      break;      // *** FIN ESTADO S_OFF ****
      // *************************************

    case S_MENU:       /*** INICIO ESTADO S_MENU ***/
      if (presionaBoton(BTN_MENU)) { // Transición BTN_MENU "B2" de MENU a AUTOMATICO
        // Pasamos al estado AUTOMATICO
        estado = S_AUTO;
        printAutomatico(automatico);
        break; // Fin estado automatico
      }
      if (presionaBoton(BTN_EXIT)) { // Transición BTN_EXIT "B1" de MENU a OFF
        // Pasamos a Estado OFF
        estado = S_OFF;
        digitalWrite(LED_OFF, HIGH);
        digitalWrite(LED_ON,   LOW);
        printOff();
        break; // Fin estado OFF
      }
      break;      // *** FIN ESTADO S_MENU ****
      // **************************************

    case S_AUTO:       /*** INICIO ESTADO S_AUTO ***/
      if (presionaBoton(BTN_MENU)) { // Transición BTN_MENU "B2" de AUTOMATICO a MANUAL
        // Pasamos al estado MANUAL
        estado = S_MANUAL;
        printManual(manual);
        break; // Fin estado MANUAL
      }
      if (presionaBoton(BTN_EXIT)) { // Transición BTN_EXIT "B1" de AUTOMATICO a MENU
        // Pasamos a estado MENU
        estado = S_MENU;
        printMenu();
        break; // Fin estado MENU
      }
      if (presionaBoton(BTN_SEL))  {  // Transición BTN_SEL  "B3" de AUTOMATICO a SEL
        // Seguimos en AUTOMATICO solo seleccionamos la accion
        estado = S_AUTO;
        if (!manual) printAutomatico(!automatico); // Si esta manual no podemos poner automatico
        else printError("Modo Manu ACTIVO");
        break; // Fin Accion
      }
      break;      // *** FIN ESTADO S_AUTO ****
      // **************************************

    case S_MANUAL:       /*** INICIO ESTADO S_MANUAL ***/
      if (presionaBoton(BTN_MENU)) { // Transición BTN_MENU "B2" de MANUAL a INFO
        // Pasamos a estado INFO
        estado = S_INFO;
        printInfo("T: 00:00", "   00.0L");
        break; // Fin estado INFO
      }
      if (presionaBoton(BTN_EXIT)) { // Transición BTN_EXIT "B1" de MANUAL a MENU
        // Pasamos a estado MENU
        estado = S_MENU;
        printMenu();
        break; // Fin menu
      }
      if (presionaBoton(BTN_SEL))  {  // Transición BTN_SEL  "B3" de MANUAL a SEL
        // Estado MANUAL Seleccionamos
        estado = S_MANUAL;
        if (!automatico) printManual(!manual);     // Si esta automatico no podemos poner manual
        else printError("Modo auto ACTIVO");
        break; // Fin Seleccion
      }
      break;      // *** FIN ESTADO S_MANUAL ***
      // ***************************************

    case S_INFO:       /*** INICIO ESTADO S_INFO ***/
      if (presionaBoton(BTN_MENU)) { // Transición BTN_MENU "B2" de INFO a CONFIG
        estado = S_CONFIG;
        printConfig();
        break;
      }
      if (presionaBoton(BTN_EXIT)) { // Transición BTN_EXIT "B1" de INFO a MENU
        estado = S_MENU;
        printMenu();
        break;
      }
      break;      // *** FIN ESTADO S_INFO ****
      // **************************************

    case S_CONFIG:     /*** INICIO ESTADO S_CONFIG ***/
      if (presionaBoton(BTN_MENU)) { // Transición BTN_MENU "B2" de CONFIG a AUTO
        estado = S_AUTO;
        printAutomatico(automatico);
        break;
      }
      if (presionaBoton(BTN_EXIT)) { // Transición BTN_EXIT "B1" de CONFIG a MENU
        estado = S_MENU;
        printMenu();
        break;
      }
      if (presionaBoton(BTN_SEL))  {  // Transición BTN_SEL  "B3" de CONFIG a ANO
        estado = S_ANO;
        printSetFechaHora;
        break;
      }
      break;      // *** FIN ESTADO S_CONFIG ****
      // **************************************

    case S_ANO:     /*** INICIO ESTADO S_ANO ***/
      if (presionaBoton(BTN_MENU)) { // Transición BTN_MENU "B2" de ANO a MES
        estado = S_MES;
        printSetFechaHora();
        break;
      }
      if (presionaBoton(BTN_EXIT)) { // Transición BTN_EXIT "B1" de ANO a CONFIG
        estado = S_CONFIG;
        printConfig();
        break;
      }
      if (presionaBoton(BTN_SEL))  { // Transición BTN_SEL "B3" incrementamos AÑO
        aaaa++;
        printSetFechaHora();
        break;
      }
      if (presionaBoton(BTN_SET))  { // Transición BTN_SET "B4" decrementamos AÑO
        aaaa--;
        printSetFechaHora();
        break;
      }
      break;      // *** FIN ESTADO S_ANO  ****
      // **************************************

    case S_MES:     /*** INICIO ESTADO S_MES ***/   
      if (presionaBoton(BTN_MENU)) { // Transición BTN_MENU "B2" de MES a DIA
        estado = S_DIA;
        printSetFechaHora();
        break;    
      }
      if (presionaBoton(BTN_EXIT)) { // Transición BTN_EXIT "B1" de MES a CONFIG
        estado = S_CONFIG;
        printConfig();
        break;
      }
      if (presionaBoton(BTN_SEL))  { // Transición BTN_SEL "B3" incrementamos MES
        if (mm<12) {
          mm++;
        } else {
          mm = 1;
        }
        printSetFechaHora();
        break;
      }
      if (presionaBoton(BTN_SET))  { // Transición BTN_SET "B4" decrementamos MES
        if (mm>1) {
          mm--;
        } else {
          mm = 12;
        }
        printSetFechaHora();
        break;
      }     
      break;      // *** FIN ESTADO S_MES  ****
      // **************************************
      
    case S_DIA:     /*** INICIO ESTADO S_DIA ***/   
      if (presionaBoton(BTN_MENU)) { // Transición BTN_MENU "B2" de DIA a HORA
        estado = S_HORA;
        printSetFechaHora();
        break;    
      }
      if (presionaBoton(BTN_EXIT)) { // Transición BTN_EXIT "B1" de DIA a CONFIG
        estado = S_CONFIG;
        printConfig();
        break;
      }
      if (presionaBoton(BTN_SEL))  { // Transición BTN_SEL "B3" incrementamos DIA
        if (dd<31) {
          dd++;
        } else {
          dd = 1;
        }
        printSetFechaHora();
        break;
      }
      if (presionaBoton(BTN_SET))  { // Transición BTN_SET "B4" decrementamos DIA
        if (dd>1) {
          dd--;
        } else {
          dd = 31;
        }
        printSetFechaHora();
        break;
      }      
      break;        //*** FIN ESTADO S_DIA ****
      // **************************************

    case S_HORA:     /*** INICIO ESTADO S_HORA ***/   
      if (presionaBoton(BTN_MENU)) { // Transición BTN_MENU "B2" de HORA a MIN
        estado = S_MIN;
        printSetFechaHora();
        break;    
      }
      if (presionaBoton(BTN_EXIT)) { // Transición BTN_EXIT "B1" de HORA a CONFIG
        estado = S_CONFIG;
        printConfig();
        break;
      }
      if (presionaBoton(BTN_SEL))  { // Transición BTN_SEL "B3" incrementamos HORA
        if (hh<23) {
          hh++;
        } else {
          hh = 0;
        }
        printSetFechaHora();
        break;
      }
      if (presionaBoton(BTN_SET))  { // Transición BTN_SET "B4" decrementamos HORA
        if (hh>0) {
          hh--;
        } else {
          hh = 23;
        }
        printSetFechaHora();
        break;
      }        
      break;      // *** FIN ESTADO S_HORA ****
      // **************************************

    case S_MIN:     /*** INICIO ESTADO S_MIN ***/   
      if (presionaBoton(BTN_MENU)) { // Transición BTN_MENU "B2" de MIN a BOX
        estado = S_BOX;
        printSetBox();
        break;    
      }
      if (presionaBoton(BTN_EXIT)) { // Transición BTN_EXIT "B1" de MIN a CONFIG
        estado = S_CONFIG;
        printConfig();
        break;
      }
      if (presionaBoton(BTN_SEL))  { // Transición BTN_SEL "B3" incrementamos MIN
        if (ii<59) {
          ii++;
        } else {
          ii = 0;
        }
        printSetFechaHora();
        break;
      }
      if (presionaBoton(BTN_SET))  { // Transición BTN_SET "B4" decrementamos MIN
        if (ii>0) {
          ii--;
        } else {
          ii = 59;
        }
        printSetFechaHora();
        break;
      }        
      break;      // *** FIN ESTADO S_MIN  ****
      // **************************************

    case S_BOX:     /*** INICIO ESTADO S_BOX ***/   
      if (presionaBoton(BTN_MENU)) { // Transición BTN_MENU "B2" de BOX a ANO
        estado = S_ANO;
        printSetFechaHora();
        break;    
      }
      if (presionaBoton(BTN_EXIT)) { // Transición BTN_EXIT "B1" de BOX a CONFIG
        estado = S_CONFIG;
        printConfig();
        break;
      }
      if (presionaBoton(BTN_SEL))  { // Transición BTN_SEL "B3" incrementamos BOX
        box++;
        printSetBox();
        break;
      }
      if (presionaBoton(BTN_SET))  { // Transición BTN_SET "B4" decrementamos BOX
        if (box>0) {
          box--;
        } else {
          box = 0;
        }
        printSetBox();
        break;
      }        
      break;      // *** FIN ESTADO S_BOX  ****
      // **************************************
      
  } // Fin switch
} // Fin loop()


// Asume la existencia de un arreglo button
uint8_t presionaBoton(int btn) {
  uint8_t valor_nuevo = digitalRead(button[btn]);
  uint8_t result = button_estate[btn]!=valor_nuevo && valor_nuevo == 1;
  button_estate[btn] = valor_nuevo;
  // Serial.println(button[btn]);
  return result;
}

/******************* Utilitarios de dibujado ****************************/
void printOff() {
  lcd.backlight();        // Encendemos, la retroilumincion
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("<< VIA LACTEA >>");
  lcd.setCursor(0, 1);
  lcd.print(" OFF - STAND BY");
  delay(2000);
  lcd.noBacklight();      // Apagamos, la retroilumincion
  
  // Pruebas Reles
  digitalWrite(K2, LOW);
}

void printMenu() {
  lcd.backlight();        // Encendemos, la retroilumincion
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("OPCIONES:");
  lcd.setCursor(0, 1);
  lcd.print("Pres: 2 para opci");

  // Pruebas Reles
  digitalWrite(K2, HIGH);
}

void printManual(boolean x) {
  lcd.backlight();        // Encendemos, la retroilumincion
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MODO MANUAL");
  lcd.setCursor(0, 1);
  lcd.print("Activado: ");
  manual = x;
  if (manual) lcd.write(1);
  else lcd.write(0);
}

void printAutomatico(boolean x) {
  lcd.backlight();        // Encendemos, la retroilumincion
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MODO AUTOMATICO");
  lcd.setCursor(0, 1);
  lcd.print("Activado: ");
  automatico = x;
  if (automatico) lcd.write(1);
  else lcd.write(0);
}

void printInfo(String tiempo, String lit) {
  lcd.backlight();        // Encendemos, la retroilumincion
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[INFO] Vaca: 001");
  lcd.setCursor(0, 1);
  lcd.print(tiempo+lit);
}

void printError(String msg) {
  lcd.backlight();        // Encendemos, la retroilumincion
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[ERROR]");
  lcd.setCursor(0, 1);
  lcd.print(msg);
}

void printConfig() {
  lcd.backlight();        // Encendemos, la retroilumincion
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[CONFIG] FEC/HOR");
  lcd.setCursor(0, 1);
  lcd.print("Pres: 3 para opci");
}

void printSetFechaHora() {
  char fechaHora[17];
  String seteo = "Nada que setear";
  switch (estado){
    case S_ANO:  seteo = "Set Ano:";      break;
    case S_MES:  seteo = "Set Mes:";      break;
    case S_DIA:  seteo = "Set Dia:";      break;
    case S_HORA: seteo = "Set Hora:";     break;
    case S_MIN:  seteo = "Set Minutos:";  break;
  }
  
  lcd.backlight();        // Encendemos, la retroilumincion
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(seteo);
  lcd.setCursor(0, 1);
  sprintf(fechaHora, "%04d-%02d-%02d %02d:%02d", aaaa, mm, dd, hh, ii); 
  lcd.print(fechaHora);
}

void printSetBox() {
  char caja[12];
  lcd.backlight();        // Encendemos, la retroilumincion
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set BOX:");
  lcd.setCursor(0, 1);
  sprintf(caja, "Box Nro: %02d", box);
  lcd.print(caja);
}
