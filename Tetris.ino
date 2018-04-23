

#include <LiquidCrystal_I2C.h>                //Librería para el uso de la pantalla LCD
#include <LedControl.h>                       //Librería para el control de las matrices de LEDs


//==============================================================================
/*
 * Proyecto: Tetris
 * Autores: Jorge Méndez Tello, María Angeles Charco Moreno y Carlos Fernando Mena Zamora
 * Componentes: 6 pulsadores, 1 zumbador pasivo, 1 pantalla LCD y 2 matrices de LEDs 8x8
*/
//==============================================================================


LedControl lc = LedControl(13,11,12,2);                              //Pines: entrada, clk, chip select, numero de matrices
LiquidCrystal_I2C lcd(0x27, 2,  1,  0,  4,  5,  6,  7,  3, POSITIVE);

//Matrices
int lc0[] = {0, 0, 0, 0, 0, 0, 0, 0};                                //Matriz superior
int lc1[] = {0, 0, 0, 0, 0, 0, 0, 0};                                //Matriz inferior
long active[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};    //Cuadro activo
long screen[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};    //Cuadro inactivo (debajo)
int tmpCol = 0;                                                      //Columna temporal para poder eliminar una línea

//Variables
int figura = 0;                                                      //Numero de la figura dibujada
int figuraSiguiente = 0;                                             //Numero de la siguiente figura
int fromLeft = 0;                                                    //Distancia entre la figura y el borde izquierdo
int fromRight = 0;                                                   //Distancia entre la figura y el borde derecho
int angulo = 0;                                                      //Ángulo de rotación
int cCol = 0;                                                        //Contador para la columna que se mueve hacia abajo
int cFigura = 0;                                                     //Contador para mover a un lado la figura
int puntuacion = 0;                                                  //Puntuación
int iniciar = 0;                                                     //Reiniciar
int nFil = 16;                                                       //Número de filas (8 x 2 = 16)
int nCol = 8;                                                        //Número de columnas (8)
int cLineas = 0;                                                     //Contador de líneas eliminadas
int pLineas = 0;                                                     //Contador de líneas actualmente eliminadas para puntuar: 1 línea = 1p, 2 líneas = 4p, 3 líneas = 16p, 4 líneas = 64p
int velCaida = 0;                                                    //Variable de la velocidad de bajada de la figura
int vCaida = 500;                                                    //Velocidad de caída
int nivel = 0;                                                       //Niveles del 0 al 5. Subes de nivel al borrar 10 líneas
int silencio = 0;                                                    //Reproducir musica o no (1 NO, 0 SI)

//Pines
int zumbador = 7;         
int rotar = 2;            
int abajo = 4;            
int derecha = 3;          
int izquierda = 5;        
int empezar = 6;
int mute = 8;          

//Caracteres propios de la pantalla LCD
byte X[8] = 
{
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b01010,
  0b10001,
  0b00000,
  0b00000
};

byte O[8] = 
{
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b00000,
  0b00000
};

byte L[8] = 
{
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11111,
  0b11111,
  0b00000
};

byte J[8] = 
{
  0b00011,
  0b00011,
  0b00011,
  0b00011,
  0b00011,
  0b11111,
  0b11111,
  0b00000
};

byte T[8] = 
{
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b01110,
  0b01110,
  0b00000,
  0b00000
};

byte I[8] = 
{
  0b01100,
  0b01100,
  0b01100,
  0b01100,
  0b01100,
  0b01100,
  0b01100,
  0b00000
};

byte Z[8] = 
{
  0b00000,
  0b00000,
  0b11110,
  0b11110,
  0b01111,
  0b01111,
  0b00000,
  0b00000
};

byte S[8] = 
{
  0b00000,
  0b00000,
  0b01111,
  0b01111,
  0b11110,
  0b11110,
  0b00000,
  0b00000
};

//Música
int length = 99;   //Número de notas
char notas[] = "EbCDCbaaCEDCbbCDECaa DFAGFEECEDCbbCDECaa EbCDCbaaCEDCbbCDECaa DFAGFEECEDCbbCDECaa ECDbCab ECDbCEAJ ";
int beats[] =
{ 
  2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 2, 2, 4, 2, 
  2, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 2, 2, 4, 1, 
  2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 2, 2, 4, 2, 
  2, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 2, 2, 4, 1, 
  5, 5, 5, 5, 5, 5, 7, 2, 5, 5, 5, 5, 2, 2, 5, 5, 3
};  
int tempo = 128;        //Tempo


//Reproducción de sonido
void playTone(int tone, int duracion) {
  for (long i = 0; i < duracion * 1000L; i += tone * 2) {
    digitalWrite(zumbador, HIGH);
    delayMicroseconds(tone);
    digitalWrite(zumbador, LOW);
    delayMicroseconds(tone);
  }
}


//Reproducción de música
void playNote(char nota, int duracion) {
  char nombres[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' , 'D', 'E', 'F', 'G', 'J', 'A', 'B'};
  //int tonos[] = { 3830, 3400, 3038, 2864, 2550, 2272, 2028, 1915, 1700, 1519, 1432, 1275, 1205, 1136, 1014 };
  int tonos[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956, 850, 760, 716, 637, 603, 568, 507 };
  //int tonos[] = { 956, 850, 760, 716, 637, 568, 507, 478, 425, 380, 358, 318, 302, 284, 253 };
 
  //Reproduce el sonido correspondiente a la nota
  for (int i = 0; i < 14; i++) {
    if (nombres[i] == nota) {
      playTone(tonos[i], duracion);
    }
  }
}



//Ceros a la izquierda en la pantalla LCD
void lcdImprimirCerosIzquierda(LiquidCrystal_I2C lcd, int valor, int colum, int fila)
{
     lcd.setCursor(colum, fila);
     if(valor < 10)
     {
         lcd.print("000");
         lcd.setCursor(colum+3, fila);
     }
     else if(valor < 100)
     {
         lcd.print("00");
         lcd.setCursor(colum+2,fila);
     }
     else if(valor < 1000)
     {
         lcd.print("0");
         lcd.setCursor(colum+1,fila);
     }
     lcd.print(valor);
}



//Actualizar columna de la matriz de LEDs
void actualizarColumna(int colum)
{ 
    lc0[colum] = active[colum] >> (nFil / 2);                             //Mostraremos en lc0 activo una imagen desplazada por el número de filas en una sola matriz
    lc1[colum] = active[colum];                                           //Mostraremos la imagen activa en lc1 (en la medida en que se ajusten los bits inferiores)
    lc.setColumn(0,colum,(screen[colum] >> (nFil / 2)) | lc0[colum]);     //Mostramos la imagen activa superpuesta en el fondo compensada por el número de filas en una sola matriz
    lc.setColumn(1,colum,screen[colum] | lc1[colum]);                     //Mostramos la imagen activa superpuesta en el fondo en lc1
}



//Retraso del botón
void retrasoBoton(int vCaida)
{
    if(velCaida > vCaida)          //Reducir la demora de movimiento
    {
        velCaida -= vCaida;
    }
    delay(vCaida);
}

                         
//Pantalla incial
void pantallaInicial()
{
     int arriba[] =
     {
         B11101110,   //   o o o   o o o  
         B01001000,   //     o     o   
         B01001100,   //     o     o o  
         B01001000,   //     o     o   
         B01001110,   //     o     o o o 
         B00000000,   //           
         B11101110,   //   o o o   o o o  
         B01001010    //     o     o   o 
     };
 
     int debajo[] =
     {
         B01001100,   //     o     o o   
         B01001010,   //     o     o   o 
         B01001001,   //     o     o     o
         B00000000,   //           
         B01000111,   //     o       o o o 
         B01000100,   //     o       o     
         B01000010,   //     o         o   
         B01001110    //     o     o o o   
     };

     for(int nFil = 0; nFil < 8; nFil++)
     {
         lc.setRow(0,nFil,arriba[nFil]);
         lc.setRow(1,nFil,debajo[nFil]);
     }
}



void setup() {
  pinMode(zumbador, OUTPUT);  
  pinMode(rotar, INPUT);
  pinMode(abajo, INPUT);
  pinMode(derecha, INPUT);
  pinMode(izquierda, INPUT);
  pinMode(empezar, INPUT);
  pinMode(mute, INPUT);

  //Encender matrices
  lc.shutdown(0,false);
  lc.shutdown(1,false);

  //Ajuste de brillo
  lc.setIntensity(0,5);
  lc.setIntensity(1,5);

  //Claridad
  lc.clearDisplay(0);
  lc.clearDisplay(1);

  //seed z szumu na nieużywanym analogowym wejściu
  randomSeed(analogRead(0));   

  //Inicialización del LCD
  lcd.begin(16,2);          //16 caracteres, 2 líneas

  //Creador de sellos para las figuras LCD
  lcd.createChar(0, X);
  lcd.createChar(1, O);
  lcd.createChar(2, L);
  lcd.createChar(3, J);
  lcd.createChar(4, T);
  lcd.createChar(5, I);
  lcd.createChar(6, Z);
  lcd.createChar(7, S);
  
  //Pantalla inicial LCD
  lcd.setCursor(1,0);
  lcd.print("ARDUINO TETRIS");
  lcd.setCursor(1,1);
  lcd.print("by URJC");
}


void loop() 
{  

if(iniciar == 0)   //Si el juego no se inicia
 { 

     //Pantalla de inicio
     pantallaInicial();

     //Melodía
     //La condición del botón debe verificarse en cada turno; de lo contrario, tendrías que esperar a que tocara toda la melodía
     if(silencio == 0)
     {
         for (int i = 0; i < length; i++) 
         {
             if(digitalRead(empezar) == HIGH)   //Si se presiona "Iniciar / reiniciar"
             {
                 iniciar = 1;                        //Comenzamos el juego
                 break;                              //Dejamos de tocar la melodía
             }
             if(digitalRead(mute) == HIGH)
             {
                silencio = 1;
                break;
             }
             if (notas[i] == ' ') 
             {
                 delay(beats[i] * tempo);   //Pausa
             } 
             else 
             {
                 playNote(notas[i], beats[i] * tempo);   //Reproducir melodía
             }
    
             //Tempo entre las notas
            delay(tempo / 2); 
         }
     }
     else
     {
          if(digitalRead(mute) == HIGH){
              silencio = 0;
          }
          if(digitalRead(empezar) == HIGH)   //Si se presiona "Iniciar / reiniciar"
             {
                 iniciar = 1;                        //Comenzamos el juego
             }
     }
 }

else   //Si el juego se inicia
 {

     //Borrar las matrices y la pantalla LCD
     lc.clearDisplay(0);
     lc.clearDisplay(1); 
     memset(lc0, 0, sizeof(lc0));
     memset(lc1, 0, sizeof(lc1));
     memset(active, 0, sizeof(active));
     memset(screen, 0, sizeof(screen));
     tmpCol = 0;
     lcd.clear();


     while(iniciar == 1)   //Hasta que termine el juego
     {
         //Impresión de líneas eliminadas LCD
         lcd.setCursor(0,0);
         lcd.print("LINES:");
         lcdImprimirCerosIzquierda(lcd, cLineas, 6, 0);


         //Impresión de puntuación LCD
         lcd.setCursor(0,1);
         lcd.print("SCORE:");
         lcdImprimirCerosIzquierda(lcd, pLineas, 6, 1);


         //Niveles
         if(cLineas < 10)         
         {
             nivel = 0;              //Nivel 0 de 0 a 9 líneas
         }
         else if(cLineas < 20)
         {
             nivel = 1;              //Nivel 1 de 10 a 19 líneas
         }
         else if(cLineas < 30)    
         {   
             nivel = 2;              //Nivel 2 de 20 a 29 líneas
         }
         else if(cLineas < 40)
         {
             nivel = 3;              //Nivel 3 de 30 a 39 líneas
         }
         else if(cLineas < 50)
         {
             nivel = 4;              //Nivel 4 de 40 a 49 líneas
         }
         else
         {
             nivel = 5;              //Nivel 5 de 50 líneas en adelante
         }

         //Imprimir nivel en LCD
         lcd.setCursor(11,0);
         lcd.print("LVL:");
         lcd.setCursor(15,0);
         lcd.print(nivel);


         
         //Restablecer la velocidad de bajada de la figura dependiendo del ninvel
         vCaida = (5 - nivel) * 100;
         velCaida = vCaida;


      
         //Dibujar figuras -> de 1 a 8, es decir, puede dar 1, 2, 3, 4, 5, 6, 7, 8
         if(figura == 0)                 //Si la cifra == 0, significa que fue la primera figura y no se generó ninguna al azar, por lo que debe dibujar una
         {
              figura = random(1,8);      //Sorteo al azar de la primera figura
         }
         else
         {
             figura = figuraSiguiente;        //Si no es la primera figura, ésta deberá ser la que ya se calculó como siguiente
         }
         figuraSiguiente = random(1,8);       //Sorteo al azar de la siguiente figura
         angulo = 0;                          //Ángulo de rotación. Al comienzo de la nueva figura, siempre es 0
         //Imprimir siguiente figura LCD
         lcd.setCursor(11,1);
         lcd.print("NXT:");
         lcd.setCursor(15,1);
         lcd.write(byte(figuraSiguiente));
         


         //Generando la figura actual
         switch(figura)
         {
             case 1:         
             //"O"
                 active[3] = 131072 + 65536;
                 active[4] = 131072 + 65536;
                 fromLeft = 3;
                 fromRight = 3;
                 break;
                 
             case 2:         
             //"L"
                 active[3] = 262144 + 131072 + 65536;
                 active[4] = 65536;
                 fromLeft = 3;
                 fromRight = 3;
                 break;
                 
             case 3:
             //"J"
                 active[3] = 65536;
                 active[4] = 262144 + 131072 + 65536;
                 fromLeft = 3;
                 fromRight = 3;
                 break;
                 
             case 4:         
             //"T"
                 active[2] = 131072;
                 active[3] = 131072 + 65536;
                 active[4] = 131072;
                 fromLeft = 2;
                 fromRight = 3;
                 break;
                 
             case 5:         
             //"I"
                 active[3] = 524288 + 262144 + 131072 + 65536;
                 fromLeft = 3;
                 fromRight = 4;
                 break;
                 
             case 6:         
             //"Z"
                 active[2] = 131072;
                 active[3] = 131072 + 65536;
                 active[4] = 65536;
                 fromLeft = 2;
                 fromRight = 3;
                 break;
                 
             case 7:
             //"S"
                 active[2] = 65536;
                 active[3] = 131072 + 65536;
                 active[4] = 131072;
                 fromLeft = 2;
                 fromRight = 3;
                 break;
         }



         //Caída de figuras
         for(int pasos = 0; pasos < nFil + 1; pasos++)   //Pueden ser 16 pasos máximos porque hay 2 matrices de 8 leds, + 1 en "cambios astutos"
         {
             cCol = 0;  //Restablecemos el contador de columna al comienzo de la caída de la nueva figura

             //Moviendo a la izquierda
             for(int i = 0; i < (nCol / 2); i++)                                                            //Hasta 4 turnos por paso
             {
                 if((digitalRead(izquierda) == HIGH) && (fromLeft > 0))                                     //Si se presiona el botón "Izquierda" y la figura no está en el borde izquierdo
                 {
                     cFigura = 0;                                                                           //Restablecer el control de cambio lateral
                     for(int col = fromLeft; col < (nCol - fromRight); col++)                               //Loop por columnas, que potencialmente pueden participar en el cambio
                     {
                         if((active[col] & screen[col - 1]) == 0)                                           //Máscara de bits -> Si la figura y la columna de fondo (uno por uno) después del producto dan 0, entonces está bien (no se superponen)
                         {
                             cFigura++;                                                                     //Aumentamos el controlador
                         }
                     }

                     if(cFigura == (nCol - fromLeft - fromRight))                                           //Si ha verificado tantas columnas como el ancho de la figura
                     {
                         for(int col = (fromLeft - 1); col < (nCol - fromRight); col++)                     //Bucle después de todas las columnas para moverse
                         {
                             if(col < (nCol - 1))                                                           //Si no es la última columna (índice 7)
                             {
                                 active[col] = active[col+1];                                               //Mover columnas
                             }
                             else
                             {
                                 active[col] = 0;                                                           //Si esta es la última columna, reiniciamos, porque no hay ningún lugar para moverse
                             }
                             actualizarColumna(col);                                                        //Actualización de pantalla
                         }
                         fromLeft--;                                                                        //La distancia desde el borde izquierdo se reduce
                         fromRight++;                                                                       //La distancia desde el borde derecho aumenta
                         playNote('E', 10);
                         retrasoBoton(200);                                                                 //Demora, no mover demasiado con solo presionar el botón
                     }
                 }
             }



             //Mover a la derecha
             for(int i = 0; i < (nCol / 2); i++)                                                             //Hasta 4 turnos por paso posible
             {
                 if((digitalRead(derecha) == HIGH) && (fromRight > 0))                                       //Si se presiona el botón "Derecha" y la figura no está en el borde derecho
                 {
                     cFigura = 0;                                                                            //Restablecer el control de cambio lateral
                     for(int col = fromLeft; col < (nCol - fromRight); col++)                                //Loop por columnas, que potencialmente pueden participar en el cambio
                     {
                         if((active[col] & screen[col + 1]) == 0)                                            //Máscara de bits -> Si la figura y la columna de fondo (una a la derecha) después del producto dan 0, entonces está bien (no se superponen)
                         { 
                             cFigura++;                                                                      //Aumentamos el controlador
                         }
                     }
 
                     if(cFigura == (nCol - fromLeft - fromRight))                                            //Si ha verificado tantas columnas como el ancho de la figura  
                     {
                         for(int col = (nCol - fromRight); col > (fromLeft - 1); col--)                      //Bucle después de todas las columnas para mover (pero desde la parte posterior, porque nos movemos hacia la derecha)
                         {
                             if(col > 0)                                                                     //Si esta no es la primera columna (índice 0)
                             {
                                 active[col] = active[col-1];                                                //Mover columnas
                             }
                             else      
                             {
                                 active[col] = 0;                                                            //Si esta es la primera columna, reiniciamos, porque no hay ningún lugar para moverse
                             }
                             actualizarColumna(col);                                                         //Actualización de pantalla
                         }
                         fromLeft++;
                         fromRight--;
                         playNote('E', 10);
                         retrasoBoton(200);                                                                  //Demora, no mover demasiado con solo presionar el botón
                     }
                 }
             }



             //Aceleración de caída (botón hacia abajo)
             if(digitalRead(abajo) == HIGH)                       //Si se presiona el botón "Abajo"
             {
                 velCaida = 0;                                    //Apagamos el retraso de la caída
                 playNote('b', 10);
             }
             else                                                 //Si no se presiona
             {
                 velCaida = vCaida;                               //Establecemos el retraso de caída
             }



             //Rotación de figura
             for(int i = 0; i < (nCol / 2); i++)
             {
                 if(digitalRead(rotar) == HIGH)                   //Si se presiona el botón "Girar"
                 {                     
                      /* La rotación es diferente dependiendo de la figura
                      // Todas las figuras, excepto "O" e "I", se escriben en un cuadrado de 3x3 y giran en el campo intermedio; este fue el original en 1984.
                      // Debido a que se manipulan mucho los bits durante la rotación, las medias figuras en los comentarios serán de la siguiente manera:
                      // 
                      // 1 2 3
                      // 4 5 6
                      // 7 8 9
                      //
                      // Para "I" será diferente:
                      //
                      // 1 2 3 4
                      // 5 6 7 8
                      // 9 10 11 12
                      // 13 14 15 16
                      //
                      // La figura "o" representada gráficamente significará un diodo encendido, y "." no
                      //*/
                     switch(figura)
                     {
                         case 1:
                         //"O"
                             break;                                                                                                                     //No hacemos nada, la figura no tiene razón para rotar porque después de girarlo se vería igual

                         case 2:
                         //"L" 
                             switch(angulo)                                                                                                             //Verificamos el ángulo actual
                             {
                                 case 0:
                                 // . o .        . . .
                                 // . o .  --->  o o o
                                 // . o o        o . .
                                     if((fromLeft > 0)                                                                                                 //Añadiremos una columna de la izquierda, así que verificamos si estamos en el borde
                                     && (((active[fromLeft + 1] | (active[fromLeft + 1] << 1)) & screen[fromLeft - 1]) == 0))                          //¿Están vacías las casillas 4 y 7?
                                     {
                                         active[fromLeft - 1] = (active[fromLeft + 1] | (active[fromLeft + 1] << 1));                                  //Permitimos los campos 4 y 7
                                         actualizarColumna(fromLeft - 1);                                                                              //Actualización de columna
                                         active[fromLeft] = (active[fromLeft + 1] << 1);                                                               //Apagamos los campos 2 y 8, 5 todavía brilla
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         active[fromLeft + 1] = (active[fromLeft + 1] << 1);                                                           //Enciende 6, deshabilite 9
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                         fromLeft--;                                                                                                   //La distancia a la izquierda se reduce (ha llegado una nueva columna)
                                         angulo = 1;                                                                                                   //El ángulo actual es 1
                                     }
                                     break;

                                 case 1:
                                 // . . .        o o .
                                 // o o o  --->  . o .
                                 // o . .        . o .
                                     if((((active[fromLeft + 2] << 1) & screen[fromLeft]) == 0)                                                        //Está el campo 1 vacío
                                     && ((((active[fromLeft + 1] << 1) | (active[fromLeft + 1] >> 1)) & screen[fromLeft + 1]) == 0))                   //¿Están vacías las casillas 2 y 8?
                                     {
                                         active[fromLeft] = (active[fromLeft + 2] << 1);                                                               //Encienda el campo 1, apague 4 y 7
                                         actualizarColumna(fromLeft);                                                                                  //Actualizando la columna
                                         active[fromLeft + 1] = active[fromLeft + 1] | (active[fromLeft + 1] << 1) | (active[fromLeft + 1] >> 1);      //Activamos los campos 2 y 8, el campo 5 aún brilla
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualizando la columna
                                         active[fromLeft + 2] = 0;                                                                                     //Apagamos toda la última columna
                                         actualizarColumna(fromLeft + 2);                                                                              //Actualizando la columna
                                         fromRight++;                                                                                                  //La distancia desde el borde derecho aumenta
                                         angulo = 2;                                                                                                   //El ángulo actual es 2
                                     }
                                     break;

                                 case 2:
                                 // o o .        . . o
                                 // . o .  --->  o o o
                                 // . o .        . . .
                                     if((fromRight > 0)                                                                                                // Vamos a agregar una columna a la derecha, así que verificamos si estamos en el borde
                                     && (((active[fromLeft] >> 1) & screen[fromLeft]) == 0)                                                            // ¿Está vacío el campo 4?
                                     && ((((active[fromLeft + 1] << 1) & active[fromLeft + 1]) & screen[fromLeft + 1]) == 0))                          // ¿Está vacío el campo 5?
                                     {
                                         active[fromLeft] = (active[fromLeft] >> 1);                                                                   // Desactiva el campo 1, activa 4
                                         actualizarColumna(fromLeft);                                                                                  // Actualizar la columna
                                         active[fromLeft + 1] = active[fromLeft];                                                                      // Desactivar los campos 2 y 8, 5 permanecen
                                         actualizarColumna(fromLeft + 1);                                                                              // Actualizar la columna
                                         active[fromLeft + 2] = ((active[fromLeft + 1] << 1) | active[fromLeft + 1]);                                  // Habilite los campos 3 y 6
                                         actualizarColumna(fromLeft + 2);                                                                              // Actualizar la columna
                                         fromRight--;                                                                                                  // La distancia desde el borde derecho se reduce
                                         pasos--;                                                                                                      // No hay más campos habilitados en la fila inferior, por lo que reducimos el paso
                                         angulo = 3;                                                                                                   // El ángulo actual es 3
                                     }
                                     break;

                                 case 3:
                                 // . . o        . o .
                                 // o o o  --->  . o .
                                 // . . .        . o o
                                     if(((((active[fromLeft] << 1) | (active[fromLeft] >> 1)) & screen[fromLeft + 1]) == 0)                            // Están los cuadros 2 y 8 vacíos
                                     && (((active[fromLeft] >> 1) & screen[fromLeft + 2]) == 0)                                                        // ¿El campo 9 está activado?
                                     && (pasos < nFil))                                                                                                // ¿No es este el último paso? Vamos a agregar una fila en la parte inferior
                                     {
                                         active[fromLeft] = 0;                                                                                         // Desactivar toda la primera columna
                                         actualizarColumna(fromLeft);                                                                                  // Actualizar la columna
                                         active[fromLeft + 1] = (active[fromLeft + 2] | (active[fromLeft + 2] >> 1));                                  // Habilita los campos 2 y 8
                                         actualizarColumna(fromLeft + 1);                                                                              // Actualizar la columna
                                         active[fromLeft + 2] = ((active[fromLeft + 2] >> 1) & (active[fromLeft + 2] >> 2));                           // Active el campo 9, apague 3 y 6
                                         actualizarColumna(fromLeft + 2);                                                                              // Actualizar la columna
                                         fromLeft++;                                                                                                   // La distancia desde el borde izquierdo aumenta
                                         pasos++;                                                                                                      // Hemos agregado una fila en la parte inferior, por lo que debe aumentar el paso
                                         angulo = 0;                                                                                                   // El ángulo actual es 0
                                     }
                                     break;
                             }
                             break;

                         case 3:
                         //"J"
                             switch(angulo)                                                                                                             //Verificamos el ángulo actual
                             {
                                 case 0:
                                 // . o .        o . .
                                 // . o .  --->  o o o
                                 // o o .        . . .
                                     if((fromRight > 0)                                                                                                //Agregaremos una columna a la derecha, así que verificamos si estamos en el borde
                                     && ((((active[fromLeft] << 2) | (active[fromLeft] << 1)) & screen[fromLeft]) == 0)                                //¿Están vacías las casillas 1 y 4?
                                     && (((active[fromLeft] << 1) & screen[fromLeft + 2]) == 0))                                                       //El campo 6 está vacío
                                     {
                                         active[fromLeft] = ((active[fromLeft] << 2) | (active[fromLeft] << 1));                                       //Encienda los campos 1 y 4, apague 7
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         active[fromLeft + 1] = ((active[fromLeft + 1] << 1) & (active[fromLeft + 1] >> 1));                           //Desactivar los campos 2 y 8
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                         active[fromLeft + 2] = active[fromLeft + 1];                                                                  //Permitimos el campo 6
                                         actualizarColumna(fromLeft + 2);                                                                              //Actualización de columna
                                         fromRight--;                                                                                                  //La distancia desde el borde derecho se reduce
                                         pasos--;                                                                                                      //No hay más campos habilitados en la fila inferior, por lo que reducimos el paso
                                         angulo = 1;                                                                                                   //El ángulo actual es 1
                                     }
                                     break;

                                 case 1:
                                 // o . .        . o o
                                 // o o o  --->  . o .
                                 // . . .        . o .
                                     if((pasos < nFil)                                                                                                 //¿No es este el último paso? Porque agregaremos una nueva linea en la parte inferior
                                     && ((((active[fromLeft + 1] << 1) | (active[fromLeft + 1] >> 1)) & screen[fromLeft + 1]) == 0)                    //¿Están vacías las casillas 2 y 8?
                                     && (((active[fromLeft + 2] << 1) & screen[fromLeft + 2]) == 0))                                                   //El campo 3 está vacío
                                     {
                                         active[fromLeft] = 0;                                                                                         //Apagamos toda la primera columna
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         active[fromLeft + 1] = (active[fromLeft + 1] | (active[fromLeft + 1] << 1) | (active[fromLeft + 1] >> 1));    //Permitimos los campos 2 y 8
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                         active[fromLeft + 2] = (active[fromLeft + 2] << 1);                                                           //Encienda el campo 3, desactive 6
                                         actualizarColumna(fromLeft + 2);                                                                              //Actualización de columna
                                         fromLeft++;                                                                                                   //La distancia desde el borde izquierdo aumenta
                                         pasos++;                                                                                                      //Hemos agregado una linea al final, por lo que debe aumentar el paso
                                         angulo = 2;                                                                                                   //El ángulo actual es 2
                                     }
                                     break;

                                 case 2:
                                 // . o o        . . .
                                 // . o .  --->  o o o
                                 // . o .        . . o
                                     if((fromLeft > 0)                                                                                                 //Agregaremos una columna a la izquierda, así que verificamos si estamos en el borde
                                     && (((active[fromLeft + 1] >> 1) & screen[fromLeft - 1]) == 0)                                                    //El campo 4 está vacío
                                     && ((((active[fromLeft + 1] >> 1) | (active[fromLeft + 1] >> 2)) & screen[fromLeft + 1]) == 0))                   //¿Están vacías las casillas 6 y 9?
                                     {
                                         active[fromLeft - 1] = (active[fromLeft + 1] >> 1);                                                           //Permitimos el campo 4
                                         actualizarColumna(fromLeft - 1);                                                                              //Actualización de columna
                                         active[fromLeft] = active[fromLeft - 1];                                                                      //Desactivar los campos 2 y 8
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         active[fromLeft + 1] = (active[fromLeft] | (active[fromLeft + 1] >> 2));                                      //Desactiva el campo 3, activa 6 y 9
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                         fromLeft--;                                                                                                   //La distancia a la izquierda se reduce (ha llegado una nueva columna)
                                         angulo = 3;                                                                                                   //El ángulo actual es 3
                                     }
                                     break;

                                 case 3:
                                 // . . .        . o .
                                 // o o o  --->  . o .
                                 // . . o        o o .
                                     if((((active[fromLeft] >> 1) & screen[fromLeft]) == 0)                                                            //¿Está vacío el campo 7?
                                     && ((((active[fromLeft] << 1) | (active[fromLeft >> 1])) & screen[fromLeft + 1]) == 0))                           //¿Están vacías las casillas 2 y 8?
                                     {
                                         active[fromLeft] = (active[fromLeft] >> 1);                                                                   //Encienda el campo 7, desactive 4
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         active[fromLeft + 1] = ((active[fromLeft + 1] << 1) | active[fromLeft + 2]);                                  //Permitimos los campos 2 y 8
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                         active[fromLeft + 2] = 0;                                                                                     //Apagamos toda la última columna
                                         actualizarColumna(fromLeft + 2);                                                                              //Actualización de columna
                                         fromRight++;                                                                                                  //La distancia desde el borde derecho aumenta
                                         angulo = 0;                                                                                                   //El ángulo actual es 0
                                     }
                                     break;
                             }
                             break;

                         case 4:
                         //"T"
                             switch(angulo)                                                                                                            //Verificamos el ángulo actual
                             {
                                 case 0:
                                 // . . .        . o .
                                 // o o o  --->  o o .
                                 // . o .        . o .
                                     if(((active[fromLeft + 1] << 1) & screen[fromLeft + 1]) == 0)                                                     //Está el campo 2 vacío
                                     {
                                         active[fromLeft + 1] = active[fromLeft + 1] | (active[fromLeft + 1] << 1);                                    //Permitimos el campo 2
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                         active[fromLeft + 2] = 0;                                                                                     //Apagamos toda la última columna
                                         actualizarColumna(fromLeft + 2);                                                                              //Actualización de columna
                                         fromRight++;                                                                                                  //La distancia con el borde derecho aumenta
                                         angulo = 1;                                                                                                   //El ángulo actual es 1
                                     }
                                     break;
                                 
                                 case 1:
                                 // . o .        . o .
                                 // o o .  --->  o o o
                                 // . o .        . . .
                                     if((fromRight > 0)                                                                                                //Agregaremos una columna a la derecha, así que verificamos si estamos en el borde
                                     && ((active[fromLeft] & screen[fromLeft + 2])== 0))                                                               //El campo 6 está vacío
                                     {
                                         active[fromLeft + 1] = active[fromLeft + 1] & (active[fromLeft + 1] << 1);                                    //Desactivar el campo 8
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                         active[fromLeft + 2] = active[fromLeft];                                                                      //Permitimos el campo 6
                                         actualizarColumna(fromLeft + 2);                                                                              //Actualización de columna
                                         fromRight--;                                                                                                  //La distancia desde el borde derecho se reduce
                                         pasos--;                                                                                                      //No hay más campos habilitados en la fila inferior, por lo que reducimos el paso
                                         angulo = 2;                                                                                                   //El ángulo actual es 2
                                     }
                                     break;
                                 
                                 case 2:
                                 // . o .        . o .
                                 // o o o  --->  . o o
                                 // . . .        . o . 
                                     if((((active[fromLeft + 1] >> 1) & screen[fromLeft + 1]) == 0)                                                    //¿Está vacía la casilla 8?
                                     && (pasos < nFil))                                                                                                //¿No es este el último paso? Porque agregaremos una linea en la parte inferior
                                     {
                                         active[fromLeft] = 0;                                                                                         //Apagamos toda la primera columna
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         active[fromLeft + 1] = active[fromLeft + 1] | (active[fromLeft + 1] >> 1);                                    //Activamos el campo 8
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                         fromLeft++;                                                                                                   //La distancia con el borde izquierdo aumenta
                                         pasos++;                                                                                                      //Hemos agregado una linea al final, por lo que debe aumentar el paso
                                         angulo = 3;                                                                                                   //El ángulo actual es 3
                                     }
                                     break;
                                 
                                 case 3:
                                     if((fromLeft > 0)                                                                                                 //Agregaremos una columna a la izquierda, así que verificamos si estamos en el borde
                                     && ((active[fromLeft + 1] & screen[fromLeft - 1]) == 0))                                                          //El campo 4 está vacío
                                     {
                                         active[fromLeft - 1] = active[fromLeft + 1];                                                                  //Permitimos el campo 4
                                         actualizarColumna(fromLeft - 1);                                                                              //Actualización de columna
                                         active[fromLeft] = active[fromLeft] & (active[fromLeft] >> 1);                                                //Desactivar el campo 2
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         fromLeft--;                                                                                                   //La distancia desde el borde izquierdo se reduce
                                         angulo = 0;                                                                                                   //El ángulo actual es 0
                                     }
                                     break;
                             }
                             break;

                         case 5:
                         //"I"
                             switch(angulo)                                                                                                             //Verificamos el ángulo actual
                             {
                                 case 0:
                                 // . o . .        . . . .
                                 // . o . .  --->  o o o o
                                 // . o . .        . . . .
                                 // . o . .        . . . .
                                     if((fromLeft > 0)                                                                                                 //Agregaremos una columna a la izquierda, así que verificamos si estamos en el borde
                                     && (fromRight > 1)                                                                                                //Agregaremos 2 columnas a la derecha, así que verificamos si estamos en el borde o justo a su lado
                                                                                                                                                       //¿Están vacías las casillas 5, 7 y 8?
                                     && ((((((active[fromLeft] >> 1) & (active[fromLeft] << 2)) & screen[fromLeft - 1]) & screen[fromLeft + 1]) & screen[fromLeft + 2]) == 0))
                                     {
                                         active[fromLeft - 1] = ((active[fromLeft] >> 1) & (active[fromLeft] << 2));                                   //Habilitamos el campo 5
                                         actualizarColumna(fromLeft - 1);                                                                              //Actualización de columna
                                         active[fromLeft] = active[fromLeft - 1];                                                                      //Desactivar los campos 2, 10 y 14
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         active[fromLeft + 1] = active[fromLeft];                                                                      //Activamos el campo 7
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                         active[fromLeft + 2] = active[fromLeft];                                                                      //Activamos el campo 8
                                         actualizarColumna(fromLeft + 2);                                                                              //Actualización de columna
                                         fromLeft--;                                                                                                   //La distancia desde el borde izquierdo se reduce
                                         fromRight -= 2;                                                                                               //La distancia desde el borde derecho se reduce
                                         pasos -= 2;                                                                                                   //No hay más campos habilitados en las dos filas inferiores, por lo que estamos reduciendo el paso
                                         angulo = 1;                                                                                                   //El ángulo actual es 1
                                     }
                                     break;

                                 case 1:
                                 // . . . .        . . o .
                                 // o o o o  --->  . . o .
                                 // . . . .        . . o .
                                 // . . . .        . . o .
                                     if((pasos < (nFil - 1))                                                                                           //¿No es este el penúltimo paso? Porque agregaremos 2 filas en la parte inferior
                                     && (((active[fromLeft] << 1) | (active[fromLeft] >> 1) | (active[fromLeft] >> 2)) & screen[fromLeft + 2]) == 0)   //¿Están vacías las casillas 3, 11 y 15?
                                     {
                                         active[fromLeft] = 0;                                                                                         //Apagamos toda la primera columna
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         active[fromLeft + 1] = 0;                                                                                     //Apagamos toda la segunda columna
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                                                                                                                                       //Permitimos los campos 3, 11 y 15
                                         active[fromLeft + 2] = (active[fromLeft + 2] | (active[fromLeft + 2] << 1) | (active[fromLeft + 2] >> 1) | (active[fromLeft + 2] >> 2));
                                         actualizarColumna(fromLeft + 2);                                                                              //Actualización de columna
                                         active[fromLeft + 3] = 0;                                                                                     //Apagamos toda la última columna
                                         actualizarColumna(fromLeft + 3);                                                                              //Actualización de columna
                                         fromLeft += 2;                                                                                                //La distancia desde el borde izquierdo aumenta
                                         fromRight++;                                                                                                  //La distancia desde el borde derecho aumenta
                                         pasos += 2;                                                                                                   //Hemos agregado 2 filas en la parte inferior, por lo que debe aumentar el paso
                                         angulo = 2;                                                                                                   //El ángulo actual es 2
                                     }
                                     break;

                                 case 2:
                                 // . . o .        . . . .
                                 // . . o .  --->  . . . .
                                 // . . o .        o o o o
                                 // . . o .        . . . .
                                     if((fromLeft > 1)                                                                                                 //Agregaremos 2 columnas a la izquierda, así que verificamos si estamos en el borde o junto a él
                                     && (fromRight > 0)                                                                                                //Agregaremos una columna a la derecha, así que verificamos si estamos en el borde
                                                                                                                                                       //¿Están vacías las casillas 9, 10 y 12?
                                     && ((((((active[fromLeft] << 1) & (active[fromLeft] >> 2)) & screen[fromLeft - 2]) & screen[fromLeft - 1]) & screen[fromLeft + 1]) == 0))
                                     {
                                         active[fromLeft - 2] = ((active[fromLeft] << 1) & (active[fromLeft] >> 2));                                   //Habilitamos el campo 9
                                         actualizarColumna(fromLeft - 2);                                                                              //Actualización de columna
                                         active[fromLeft - 1] = active[fromLeft - 2];                                                                  //Habilitamos el campo 10
                                         actualizarColumna(fromLeft - 1);                                                                              //Actualización de columna
                                         active[fromLeft] = active[fromLeft - 1];                                                                      //Apagamos los campos 3, 7 y 15
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         active[fromLeft + 1] = active[fromLeft];                                                                      //Permitimos el campo 12
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                         fromLeft -= 2;                                                                                                //La distancia desde el borde izquierdo se reduce
                                         fromRight--;                                                                                                  //La distancia desde el borde derecho se reduce
                                         pasos--;                                                                                                      //No hay más campos habilitados en la fila inferior, por lo que reducimos el paso
                                         angulo = 3;                                                                                                   //El ángulo actual es 3
                                     }
                                     break;

                                 case 3:
                                 // . . . .        . o . .
                                 // . . . .  --->  . o . .
                                 // o o o o        . o . .
                                 // . . . .        . o . .
                                     if((pasos < (nFil))                                                                                               //¿No es este el último paso? Porque agregaremos un poema en la parte inferior
                                     && (((active[fromLeft] >> 1) | (active[fromLeft] << 1) | (active[fromLeft] << 2)) & screen[fromLeft + 1]) == 0)   //¿Están vacías las casillas 2, 6 y 14?
                                     {
                                         active[fromLeft] = 0;                                                                                         //Apagamos toda la primera columna
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                                                                                                                                       //Permitimos los campos 2, 6 y 14
                                         active[fromLeft + 1] = (active[fromLeft + 1] | (active[fromLeft + 1] >> 1) | (active[fromLeft + 1] << 1) | (active[fromLeft + 1] << 2));
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                         active[fromLeft + 2] = 0;                                                                                     //Desactiva toda la penúltima columna
                                         actualizarColumna(fromLeft + 2);                                                                              //Actualización de columna
                                         active[fromLeft + 3] = 0;                                                                                     //Apagamos toda la última columna
                                         actualizarColumna(fromLeft + 3);                                                                              //Actualización de columna
                                         fromLeft++;                                                                                                   //La distancia desde el borde izquierdo aumenta
                                         fromRight += 2;                                                                                               //La distancia desde el borde derecho aumenta
                                         pasos++;                                                                                                      //Hemos agregado una linea al final, por lo que debe aumentar el paso
                                         angulo = 0;                                                                                                   //El ángulo actual es 0
                                     }
                                     break;
                             }
                             break;

                         case 6:
                         //"Z"
                             switch(angulo)                                                                                                            //Verificamos el ángulo actual
                             {
                                 case 0:
                                 // . . .        . o .
                                 // o o .  --->  o o .
                                 // . o o        o . .
                                     if(((active[fromLeft + 1] & screen[fromLeft]) == 0)                                                               //¿Está vacío el campo 7?
                                     && (((active[fromLeft + 1] << 1) & screen[fromLeft + 1]) == 0))                                                   //¿Está vacío el campo 2?
                                     {
                                         active[fromLeft] = active[fromLeft + 1];                                                                      //Activamos el campo 7
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         active[fromLeft + 1] = (active[fromLeft + 1] << 1);                                                           //Encienda el campo 2, deshabilite 8
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                         active[fromLeft + 2] = 0;                                                                                     //Apagamos toda la última columna
                                         actualizarColumna(fromLeft + 2);                                                                              //Actualización de columna
                                         fromRight++;                                                                                                  //La distancia desde el borde derecho aumenta
                                         angulo = 1;                                                                                                   //El ángulo actual es 1
                                     }
                                     break;

                                 case 1:
                                 // . o .        o o .
                                 // o o .  --->  . o o
                                 // o . .        . . .
                                     if((fromRight > 0)                                                                                                //Agregaremos una columna a la derecha, así que verificamos si estamos en el borde
                                     && ((((active[fromLeft] << 2) & (active[fromLeft] << 1)) & screen[fromLeft]) == 0)                                //Está el campo 1 vacío
                                     && (((active[fromLeft] & active[fromLeft + 1]) & screen[fromLeft + 2]) == 0))                                     //El campo 6 está vacío
                                     {
                                         active[fromLeft] = ((active[fromLeft] << 2) & (active[fromLeft] << 1));                                       //Encienda el campo 1, apague 4 y 7
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         active[fromLeft + 2] = (active[fromLeft] >> 1);                                                               //Permitimos el campo 6
                                         actualizarColumna(fromLeft + 2);                                                                              //Actualización de columna
                                         fromRight--;                                                                                                  //La distancia desde el borde derecho se reduce
                                         pasos--;                                                                                                      //No hay más campos habilitados en la fila inferior, por lo que reducimos el paso
                                         angulo = 2;                                                                                                   //El ángulo actual es 2
                                     }
                                     break;

                                 case 2:
                                 // o o .        . . o
                                 // . o o  --->  . o o
                                 // . . .        . o .
                                     if((pasos < nFil)                                                                                                 //¿No es este el último paso? Porque agregaremos un poema en la parte inferior
                                     && (((active[fromLeft + 1] >> 1) & screen[fromLeft + 1]) == 0)                                                    //¿Está vacía la casilla 8?
                                     && (((active[fromLeft + 2] << 1) & screen[fromLeft + 2]) == 0))                                                   //¿Está vacía la casilla 7?
                                     {
                                         active[fromLeft] = 0;                                                                                         //Apagamos toda la primera columna
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         active[fromLeft + 1] = (active[fromLeft + 1] >> 1);                                                           //Desactivar el campo 2, encender 8
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                         active[fromLeft + 2] = (active[fromLeft + 2] | (active[fromLeft + 2] << 1));                                  //Permitimos el campo 3
                                         actualizarColumna(fromLeft + 2);                                                                              //Actualización de columna
                                         fromLeft++;                                                                                                   //La distancia desde el borde izquierdo aumenta
                                         pasos++;                                                                                                      //Hemos agregado una linea al final, por lo que debe aumentar el paso
                                         angulo = 3;                                                                                                   //El ángulo actual es 3
                                     }
                                     break;

                                 case 3:
                                 // . . o        . . .
                                 // . o o  --->  o o .
                                 // . o .        . o o
                                     if((fromLeft > 0)                                                                                                 //Agregaremos una columna a la izquierda, así que verificamos si estamos en el borde
                                     && (((active[fromLeft] & active[fromLeft + 1]) & screen[fromLeft - 1]) == 0)                                      //El campo 4 está vacío
                                     && (((active[fromLeft + 1] >> 1) & screen[fromLeft + 1]) == 0))                                                   //El campo 9 está vacío
                                     {
                                         active[fromLeft - 1] = (active[fromLeft] & active[fromLeft + 1]);                                             //Permitimos el campo 4
                                         actualizarColumna(fromLeft - 1);                                                                              //Actualización de columna
                                         active[fromLeft + 1] = (active[fromLeft - 1] >> 1);                                                           //Encienda el campo 9, apague 3 y 6
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                         fromLeft--;                                                                                                   //La distancia desde el borde izquierdo se reduce
                                         angulo = 0;                                                                                                   //El ángulo actual es 0
                                     }
                                     break;
                             }
                             break;

                         case 7:
                         //"S"
                             switch(angulo)                                                                                                            //Verificamos el ángulo actual
                             {
                                 case 0:
                                 // . . .        o . .
                                 // . o o  --->  o o .
                                 // o o .        . o .
                                     if(((active[fromLeft + 1] << 1) & screen[fromLeft]) == 0)                                                         //¿Están vacías las casillas 1 y 4?
                                     {
                                         active[fromLeft] = (active[fromLeft + 1] << 1);                                                               //Encienda el campo 1, desactive 7
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         active[fromLeft + 2] = 0;                                                                                     //Apagamos toda la última columna
                                         actualizarColumna(fromLeft + 2);                                                                              //Actualización de columna
                                         fromRight++;                                                                                                  //La distancia desde el borde derecho aumenta
                                         angulo = 1;                                                                                                   //El ángulo actual es 1
                                     }
                                     break;

                                 case 1:
                                 // o . .        . o o
                                 // o o .  --->  o o .
                                 // . o .        . . .
                                     if((fromRight > 0)                                                                                                //Agregaremos una columna a la derecha, así que verificamos si estamos en el borde
                                     && (((active[fromLeft + 1] << 1) & screen[fromLeft + 1]) == 0)                                                    //Está el campo 2 vacío
                                     && (((active[fromLeft] & (active[fromLeft] << 1)) & screen[fromLeft + 2]) == 0))                                  //El campo 3 está vacío
                                     {
                                         active[fromLeft] = (active[fromLeft] & active[fromLeft + 1]);                                                 //Desactivar el campo 1
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         active[fromLeft + 1] = (active[fromLeft + 1] << 1);                                                           //Encienda el campo 2, deshabilite 8
                                         actualizarColumna(fromLeft + 1);                                                                              //Actualización de columna
                                         active[fromLeft + 2] = (active[fromLeft] << 1);                                                               //Permitimos el campo 3
                                         actualizarColumna(fromLeft + 2);                                                                              //Actualización de columna
                                         fromRight--;                                                                                                  //La distancia desde el borde derecho se reduce
                                         pasos--;                                                                                                      //No hay más campos habilitados en la fila inferior, por lo que reducimos el paso
                                         angulo = 2;                                                                                                   //El ángulo actual es 2
                                     }
                                     break;

                                 case 2:
                                 // . o o        . o .
                                 // o o .  --->  . o o
                                 // . . .        . . o
                                     if((pasos < nFil)                                                                                                 //¿No es este el último paso? Porque agregaremos una linea en la parte inferior
                                     && (((active[fromLeft + 1] >> 1) & screen[fromLeft + 2]) == 0))                                                   //¿Están vacías las casillas 6 y 9?
                                     {
                                         active[fromLeft] = 0;                                                                                         //Apagamos toda la última columna
                                         actualizarColumna(fromLeft);                                                                                  //Actualización de columna
                                         active[fromLeft + 2] = (active[fromLeft + 1] >> 1);                                                           //Permitimos los campos 6 y 9
                                         actualizarColumna(fromLeft + 2);                                                                              //Actualización de columna
                                         fromLeft++;                                                                                                   //La distancia desde el borde izquierdo aumenta
                                         pasos++;                                                                                                      //Hemos agregado una linea al final, por lo que debe aumentar el paso
                                         angulo = 3;                                                                                                   //El ángulo actual es 3
                                     }
                                     break;

                                 case 3:
                                 // . o .        . . .
                                 // . o o  --->  . o o
                                 // . . o        o o .
                                     if((fromLeft > 0)                                                                                                //Agregaremos una columna a la izquierda, así que verificamos si estamos en el borde
                                     && ((active[fromLeft + 1] & ((active[fromLeft + 1] >> 1)) & screen[fromLeft - 1]) == 0)                          //¿Está vacío el campo 7?
                                     && ((active[fromLeft + 1] & screen[fromLeft]) == 0))                                                             //¿Está vacío el campo 8?
                                     {
                                         active[fromLeft - 1] = (active[fromLeft + 1] & (active[fromLeft + 1] >> 1));                                 //Activamos el campo 7
                                         actualizarColumna(fromLeft - 1);                                                                             //Actualización de columna
                                         active[fromLeft] = active[fromLeft + 1];                                                                     //Desactivar el campo 2, encender 8
                                         actualizarColumna(fromLeft);                                                                                 //Actualización de columna
                                         active[fromLeft + 1] = (active[fromLeft - 1] << 1);                                                          //Desactivar el campo 9
                                         actualizarColumna(fromLeft + 1);                                                                             //Actualización de columna
                                         fromLeft--;                                                                                                  //La distancia desde el borde izquierdo se reduce
                                         angulo = 0;                                                                                                  //El ángulo actual es 0
                                     }
                                     break;
                             }
                             break;
                     }
                     playNote('E', 10);
                     retrasoBoton(200);                                                                                                                //Retraso en no rotar demasiado con solo presionar el botón
                 }
             }



  
             //Reiniciar
             if(digitalRead(empezar) == HIGH)                       //Si se presiona el botón "Comenzar / reiniciar"
             {
                 memset(lc0, 0, sizeof(lc0));                       //Restablecer la matriz superior
                 memset(lc1, 0, sizeof(lc1));                       //Restablecer la matriz inferior
                 memset(active, 0, sizeof(active));                 //Restablecer la imagen activa
                 memset(screen, 0, sizeof(screen));                 //Borrando la imagen de fondo
                 puntuacion = 0;                                    //Puntuación puesta a 0
                 cLineas = 0;                                       //Contador de líneas eliminadas puesto a 0
                 figura = 0;                                        //Restablecer la figura dibujada
                 break;
             }



             //Verificando la posibilidad de bajar
             for(int col = 0; col < nCol; col++)                      //Pasa por todas las columnas
             {
                 //Si después del cambio no manejamos en otros bloques
                 if((screen[col] & (active[col] >> 1)) == 0)               //Si no hay colisión, el producto bit de dos imágenes dará 0
                 {
                     cCol++;
                 }
                 else
                 {
                     cCol = 0;
                     if(pasos == 0)                                               //Si es imposible moverlo, y esta es solo la primera vuelta del ciclo, significa que el tablero está lleno -> ¡Se acabó el juego!
                     {
                         iniciar = 0;
                     }
                 }
             }

             //Si se puede mover
             if((cCol == nCol) && (pasos < nFil))           //Si cCol es igual a nCol, cada columna ha pasado la verificación y puede mover toda la imagen hacia abajo por una línea. El paso debe ser más pequeño que el número de filas
             {
                 for(int col = 0; col < nCol; col++)        //Pasa por todas las columnas
                 {
                     active[col] = active[col] >> 1;        //El desplazamiento en bit a la derecha moverá la columna hacia abajo
                     actualizarColumna(col);
                 }
             }
             else
             {
                 break;                                     //Si no puede moverse, nos detenemos para ir más rápido al siguiente bloque
             }             
             delay(velCaida);                               //Retrasar caída
         }


//Actualización del fondo
         for(int col = 0; col < nCol; col++)                         //Bucle por columnas en cada matriz
         {
             screen[col] = screen[col] | (lc0[col] << (nFil / 2));     //Adición de contenido de matriz al fondo
             screen[col] = screen[col] | lc1[col];                     //Adición de contenido de matriz al fondo
             lc0[col] = 0;                                             //Restablecer
             lc1[col] = 0;                                             //Restablecer
             active[col] = 0;                                          //Restablecer
         }


// == Eliminación de líneas y puntos ========================================== ==========

                  // Eliminar líneas completas y aumentar el resultado
         pLineas = 0;                                                       // Contador de líneas eliminadas al mismo tiempo en
         for(int fil = 0; fil < nFil; fil++)                                //Pętla po wszystkich wierszach
         {
             cCol = 0;                                                      // Loop después de todas las filas
             for(int col = 0; col < nCol; col++)                            // Restablecer el corrector de columna
             {    
    if(((screen[col] >> fil) & 1) == 1)                                     // Si después de mover la columna por el número de la fila actual y el producto lógico de 1 obtenemos 1, significa que el LED está encendido en este lugar
                 {    
      cCol++;                                                               // Aumenta el verificador
                 }
             }
             if(cCol == nCol)                                               // Si el verificador == el número de todas las columnas, significa que todas están iluminadas -> esta es la línea completa
             {   
                     // Eliminar animación
                 for(int col = 0; col < nCol; col++)                        // Loop después de todas las columnas
                 {
                     tmpCol = ~((int) round(pow(2, fil)));                  // Máscara de bit -> 0 en la posición correspondiente a la línea a eliminar, el resto de los bits 1
                     screen[col] = screen[col] & tmpCol;                    // Usando una máscara de bits en una columna
                     actualizarColumna(col);                                // Actualiza la pantalla
                     
                     switch(pLineas)                                        // Seleccione la nota para jugar al eliminar. Cuantas más líneas, el siguiente sonido es más alto -> es típico para el tramo;)
                        {
       case 0:
                             playNote('b', 20);
                             break;
                         case 1:
                             playNote('D', 20);
                             break;
                         case 2:
                             playNote('F', 20);
                             break;
                         case 3:
                             playNote('A', 20);
                             break;
                     }
                     delay(30);                                                              // Demora para que puedas ver el efecto en absoluto
                      // Eliminación real de la línea
                     tmpCol = (int) (round(pow(2, fil)) - 1);                                // Máscara de bit -> "abajo" de la columna actual que está debajo de la línea que se eliminará
                     tmpCol = screen[col] & tmpCol;                                          // La columna temporal debe contener solo este "fondo"
                     screen[col] = (screen[col] >> (fil + 1));                               // Mueva la columna actual hacia abajo para eliminar la línea y lo que está debajo
                     screen[col] = (screen[col] << fil);                                     // Mueve la columna actual hacia arriba, pero 1 menos (porque la línea fue eliminada)
                     screen[col] = screen[col] | tmpCol;                                     // Agregamos lo que estaba debajo de la línea eliminada a la columna actual
                 }
                 
                 // El efecto de bloques "cayendo" después de quitar la línea
                 for(int col = 0; col < nCol; col++)                                         //Loop después de todas las columnas
                 {
                     actualizarColumna(col);                                                 //Actualiza la pantalla
                 }
                 fil--;                                                                      //Reduce el contador en el bucle, porque la numeración de las líneas ha cambiado y tienes que volver a verificar (tal vez la siguiente línea)
                 pLineas++;                                                                  //Incrementa el contador de las líneas actuales
                 cLineas++;                                                                  //Aumenta el contador de todas las líneas
             }
         }
         
         if(pLineas > 0)                                                                     // Si hubiera alguna línea eliminada
         {
             puntuacion += (int) round(pow(4, pLineas-1));                                   //Puntuación
         }
    }    
 }
}
