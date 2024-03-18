// Display
// ST7789 library example
// (c) 2019-20 Pawel A. Hernik

/*
 ST7789 240x240 IPS (without CS pin) connections (only 6 wires required):

 #01 GND -> GND
 #02 VCC -> VCC (3.3V only!)
 #03 SCL -> D13/SCK
 #04 SDA -> D11/MOSI
 #05 RES -> D8 or any digital
 #06 DC  -> D7 or any digital
 #07 BLK -> NC
*/

#define SCR_WD   240
#define SCR_HT   240
#include <SPI.h>
#include <Adafruit_GFX.h>
#define TFT_DC  9
#define TFT_RST 8
#include <Arduino_ST7789_Fast.h>
Arduino_ST7789 lcd = Arduino_ST7789(TFT_DC, TFT_RST);

// Encoder

const int dirPin = 7; // asignación de salida para marcar el sentido de giro en el driver del motor PAP
const int stepPin = 6; // asignación de salida para mandar los pulsos al driver del motor PAP

const int steps = 20; //pasos por vuelta del encoder
int stepDelay;

#include <ClickEncoder.h>
#include <TimerOne.h>

int menuItem = 1; // opción de menú seleccionada
int setMenu = 0; // setMenu 1 el encoder cambia valores en la opción de menú seleccionada

String sn = "NO";

boolean up = false;
boolean down = false;
boolean middle = false;

ClickEncoder *encoder;
int16_t last, value;

// Variables

boolean Inicio = false; //variable desde la que se lanza el giro
int nFoto=20; // número de fotos por giro de 360º
int Foto; // indíca en el display cuantas fotos quedan por hacer en ese giro
boolean Enfoque = false; // activa/desactiva el enfoque en cada foto.
int tEspera = 1; // tiempo de espera desde que para el motor y se dispara la foto
int tFoto = 1; // tiempo de espera para hacer la foto
int azul = 127; // color que indica item no seleccionado (azul)
int verde = 4000; // color que indica item seleccionado
int blanco = 65535; // color que indica item no seleccionado (blanco)
int cInicio = 65535; // color que indica la opción INICIO (blanco por defecto)
int cnFoto = 65535; // color que indica item no seleccionado (blanco)
int crnFoto = 65535; // color que indica item no seleccionado (blanco)
int cEnfoque = 65535; // color que indica item no seleccionado (blanco)
int crEnfoque = 65535; // color que indica item no seleccionado (blanco)
int ctEspera = 65535; // color que indica item no seleccionado (blanco)
int crtEspera = 65535; // color que indica item no seleccionado (blanco)
int ctFoto = 65535; // color que indica item no seleccionado (blanco)
int crtFoto = 65535; // color que indica item no seleccionado (blanco)
int tgrande = 5; // tamaño del texto grande
int tmedio = 4; // tamaño del texto medio
int tpequeno = 2; // tamaño del texto pequeño
int tamano[6] = {0, 5, 2, 2, 2, 2}; // tamaño de texto en las líneas de menú, no uso la línea 0
const int Focus = 3; // asignación de salida para autofoco
const int Shoot = 4; // asignación de salida para disparador



void setup() {

  Serial.begin(9600);

// Display

  lcd.init();
  lcd.setRotation(1);
  lcd.fillScreen(BLACK);
  inicio();
   
// Encoder 
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  encoder = new ClickEncoder(A1, A0, A2);
  encoder->setAccelerationEnabled(false);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);
  last = encoder->getValue();

// Cámara
  pinMode(Focus, OUTPUT);
  pinMode(Shoot, OUTPUT);

}

void loop() {

  readRotaryEncoder();

  ClickEncoder::Button b = encoder->getButton();
   if (b != ClickEncoder::Open) {
   switch (b) {
      case ClickEncoder::Clicked:
         middle=true;
        break;
    }
  }    



  // Código para definir la posición del cursor descendente.
  
  if (up && menuItem == 1){
    up=false;
    menuItem = 5;
    tamano[1] = tpequeno;
    tamano[5] = tmedio;
    inicio();
    } else if (up && menuItem == 5){
       up=false;
       if (setMenu == 1){
         tFoto--;
         if (tFoto < 0){
            tFoto = 0;
           }
           inicio();
           } else {
             menuItem = 4;
             tamano[5] = tpequeno;
             tamano[4] = tmedio;
             inicio();
            }
           } else if (up && menuItem == 4){
             up=false;
             if (setMenu == 1){
               tEspera--;
               if (tEspera < 0){
                 tEspera = 0;
                }
                inicio();
               } else {
                 menuItem = 3;
                 tamano[4] = tpequeno;
                 tamano[3] = tmedio;
                 inicio();
                 }
                } else if (up && menuItem == 3){
                  up=false;
                  if (setMenu == 1){
                    if (Enfoque == false){
                      Enfoque = true;
                      inicio();
                    } else {
                      Enfoque = false;
                      inicio();
                      }
                    } else {
                      menuItem = 2;
                      tamano[3] = tpequeno;
                      tamano[2] = tmedio;
                      inicio();
                      }
                 } else if (up && menuItem == 2){
                   up=false;
                   if (setMenu == 1){
                     nFoto--;
                     if (nFoto < 1){
                       nFoto = 1;
                       }
                     inicio();
                    } else {
                      menuItem = 1;
                      tamano[2] = tpequeno;
                      tamano[1] = tgrande;
                      inicio();
                      } 
                  }
                  
// Código para definir la posición del cursor ascendente.
  
  if (down && menuItem == 1){
    down=false;
    menuItem = 2;
    tamano[1] = tpequeno;
    tamano[2] = tmedio;
    inicio();
    } else if (down && menuItem == 2){
       down=false;
       if (setMenu == 1){
         nFoto++;
         inicio();
         } else {
           menuItem = 3;
           tamano[2] = tpequeno;
           tamano[3] = tmedio;
           inicio();
           }   
         } else if (down && menuItem == 3){
           down=false;
           if (setMenu == 1){
             if (Enfoque == false){
                Enfoque = true;
                inicio();
                } else {
                  Enfoque = false;
                  inicio();
                  }
             } else {
               menuItem = 4;
               tamano[3] = tpequeno;
               tamano[4] = tmedio;
               inicio();
               }
             } else if (down && menuItem == 4){
               down=false;
               if (setMenu == 1){
               tEspera++;
               inicio();
               } else {
                 menuItem = 5;
                 tamano[4] = tpequeno;
                 tamano[5] = tmedio;
                 inicio();
                 }
              } else if (down && menuItem == 5){
                down=false;
                if (setMenu == 1){
                  tFoto++;
                  inicio();
                  } else {
                    menuItem = 1;
                    tamano[5] = tpequeno;
                    tamano[1] = tgrande;
                    inicio();
                    }
                 }
  
// Código para las pulsaciones en las diferentes posiciones del menú

  if (middle && menuItem == 1){ //lanzo el giro
    middle=false;
    giro360();
    inicio();
    } else if (middle && menuItem == 2){ 
      middle=false;
      if (setMenu == 0){
      setMenu = 1;
      crnFoto = verde;
      } else {
        setMenu = 0;
        crnFoto = blanco;
        }
       inicio();
       } else if (middle && menuItem == 3){ 
           middle=false;
           if (setMenu == 0){
             setMenu = 1;
             crEnfoque = verde;
             } else {
               setMenu = 0;
               crEnfoque = blanco;
               }
             inicio();
           } else if (middle && menuItem == 4){ 
               middle=false;
               if (setMenu == 0){
               setMenu = 1;
               crtEspera = verde; 
               } else {
                  setMenu = 0;
                  crtEspera = blanco;
                  }
               inicio();
              } else if (middle && menuItem == 5){ 
                 middle=false;
                 if (setMenu == 0){
                 setMenu = 1;
                 crtFoto = verde; 
                 } else {
                    setMenu = 0;
                    crtFoto = blanco;
                    }
                 inicio();
                }
   
 }


  void timerIsr() {
   encoder->service();
 }

 void readRotaryEncoder(){
  
  value += encoder->getValue();
  
  if (value/2 > last) {
    last = value/2;
    down = true;
    delay(150);
    }else   if (value/2 < last) {
      last = value/2;
      up = true;
      delay(150);
      }
 }

 void inicio(){
  
  lcd.fillScreen(BLACK);
  lcd.setCursor(0, 40);
  lcd.setTextSize(tamano[1]);
  lcd.setTextColor(cInicio);
  lcd.println("INICIO");
  lcd.setTextSize(2);
  lcd.println(" ");
  lcd.setTextSize(tamano[2]);
  lcd.setTextColor(cnFoto);
  lcd.print("Fotos/360: ");
  lcd.setTextColor(crnFoto);
  lcd.println(nFoto);
  lcd.setTextSize(2);
  lcd.println(" ");
  lcd.setTextSize(tamano[3]);
  lcd.setTextColor(cEnfoque);
  lcd.print("Enfoque:  ");
  lcd.setTextColor(crEnfoque);
  if (Enfoque){
    lcd.println("si");
    } else {
      lcd.println("no");
      }
  lcd.setTextSize(2);
  lcd.println(" ");
  lcd.setTextSize(tamano[4]);
  lcd.setTextColor(ctEspera);
  lcd.print("t enfoque:");
  lcd.setTextColor(crtEspera);
  lcd.print(tEspera);
  lcd.println("s");
  lcd.setTextSize(2);
  lcd.println(" ");
  lcd.setTextSize(tamano[5]);
  lcd.setTextColor(ctFoto);
  lcd.print("t foto:   ");
  lcd.setTextColor(crtFoto);
  lcd.print(tFoto);
  lcd.println("s");
 }

 void secuencia(){

  Foto--;
  lcd.fillScreen(BLACK);
  lcd.setCursor(40, 40);
  lcd.setTextSize(3);
  lcd.setTextColor(blanco);
  lcd.println("Quedan");
  lcd.setCursor(40, 100);
  lcd.setTextSize(9);
  lcd.setTextColor(verde);
  lcd.println(Foto);
  lcd.setCursor(20, 190);
  lcd.setTextSize(2);
  lcd.setTextColor(blanco);
  lcd.println("fotos por hacer");
  
 }

 void foto(){
  if (Enfoque){
     digitalWrite(Focus, HIGH); 
     }
  delay(tEspera*1000);
  digitalWrite(Focus, LOW);
 
  digitalWrite(Shoot, HIGH);
  delay(300);
  digitalWrite(Shoot, LOW);
  delay(tFoto*1000);
 // Foto--;
  secuencia();
  
 }

void giro360(){
  
   int pasosfoto = 1600 / nFoto; // Calculamos los pasos por foto
   Foto = nFoto;
   //secuencia();
   if (pasosfoto * nFoto != 1600){ // Cuando la división del número de pasos no da como resultado un entero, el último giro corrige la cantidad de pasos para quedar en la misma posición de inicio
       int pasosfinal = 1600 - (pasosfoto * (Foto-1));
       digitalWrite(dirPin, HIGH);
       stepDelay = 1500;
       for (int x = 0; x < nFoto-1; x++){ 
          for (int y = 0; y < pasosfoto ; y++) {
             digitalWrite(stepPin, HIGH);
             delayMicroseconds(stepDelay);
             digitalWrite(stepPin, LOW);
             delayMicroseconds(stepDelay);
             }
             foto(); 
         }
          for (int z = 0; z < pasosfinal ; z++) {
             digitalWrite(stepPin, HIGH);
             delayMicroseconds(stepDelay);
             digitalWrite(stepPin, LOW);
             delayMicroseconds(stepDelay);
             }
             foto();
                                  
      } else {
        digitalWrite(dirPin, HIGH);  // Esta es la secuencia cuando el número de pasos por vuelta da una cifra entera.
        stepDelay = 1000;
        for (int a = 0; a < nFoto; a++){
           foto();
           for (int b = 0; b < pasosfoto ; b++) {
              digitalWrite(stepPin, HIGH);
              delayMicroseconds(stepDelay);
              digitalWrite(stepPin, LOW);
              delayMicroseconds(stepDelay);
              }
          }    
      }
  }
