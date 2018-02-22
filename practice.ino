//Declaración de variables
int frecuencia;
int unidades;
int decenas;
int turno;
boolean asterisco;  
boolean seleccion; //variable para seleccionar los displays

//Inicialización - Setup
void setup(){
  DDRL = B11110000;
  DDRC = B00000001; //Instrucción que pone los pin del puerto C como de entrada salvo PC0 (Speaker)
  PORTC = B11111000; //Activación de las línes de pull-up internas
  
  frecuencia = 200;
  turno = 0;
  asterisco = false;
  tone(3,100); //petición de interrupción
  attachInterrupt(0,tarea2,FALLING); //ejecutar la tarea2 cuando interrumpa
  seleccion = true;
}

//Procedimiento que cambia la frecuencia del altavoz
void altavoz(int frecuencia){
  switch(frecuencia){
    case 0:
      frecuencia = 200;
      break;
    case 1:
      frecuencia = 400;
      break;
    case 2:
      frecuencia = 600;
      break;
    case 3:
      frecuencia = 800;
      break;
    case 4:
      frecuencia = 1000;
      break;
    case 5:
      frecuencia = 1200;
      break;
    case 6:
      frecuencia = 1400;
      break;
    case 7:
      frecuencia = 1600;
      break;
    case 8:
      frecuencia = 1800;
      break;
    case 9:
      frecuencia = 2000;
      break;
  }
  asterisco = false;
}

//Procedimiento que codifica la tecla
int pantalla(int p){
  int dato;
  switch(p){
    case 0:
      dato = 63;
      break;
    case 1:
      dato = 6;
      break;
    case 2:
      dato = 91;
      break;
    case 3:
      dato = 79;
      break;
    case 4:
      dato = 102;
      break;
    case 5:
      dato = 109;
      break;
    case 6:
      dato = 125;
      break;
    case 7:
      dato = 7;
      break;
    case 8:
      dato = 127;
      break;
    case 9:
      dato = 103;
      break;
  }
  return dato;
}

//Procedimiento que descompone turno en unidades y decenas
void descomponer(){
  unidades = turno / 10;
  decenas = unidades % 10;
  unidades = turno % 10;
}

//Procedimiento que imprime los números en los display, además de hacer el barrido del teclado
void imprimir(){
  descomponer();
  if (seleccion == true){
    PORTA = pantalla(unidades);
    if (unidades != 0 && unidades != 1 && unidades != 7)
      digitalWrite(5,HIGH);
    else
      digitalWrite(5,LOW);
    digitalWrite(6,LOW); //unidades
    digitalWrite(7,HIGH);
    columna1();
    seleccion = false;
  }
  else
  {
    PORTA = pantalla(decenas);
    if (decenas != 0 && decenas != 1 && decenas != 7)
      digitalWrite(5,HIGH);
    else
      digitalWrite(5,LOW);
    digitalWrite(7,LOW); //decenas
    digitalWrite(6,HIGH);
    columna2();
    seleccion = true;
  }
}

//Procedimiento que hace el barrido de la columna 1 del teclado
void columna1(){
    if (asterisco == false){
      int ta = digitalRead(14);
      if (ta == 0)
        asterisco = true;
    }
    if (asterisco == true){
      int t1 = digitalRead(17);
      int t4 = digitalRead(16);
      int t7 = digitalRead(15);
      if (t1 == 0)
        altavoz(1);
      else if(t4 == 0)
        altavoz(3);
      else if(t7 == 0)
        altavoz(5);
    }
}

//Procedimiento que hace el barrido de la columna 2 del teclado
void columna2(){
    if (asterisco == true){
      int t0 = digitalRead(14);
      int t2 = digitalRead(17);
      int t5 = digitalRead(16);
      int t8 = digitalRead(15);
      if (t0 == 0)
        altavoz(0);
      else if(t2 == 0)
        altavoz(2);
      else if(t5 == 0)
        altavoz(4);
      else if(t8 == 0)
        altavoz(6);
    }
}

//Procedimiento que incrementa o decremente el turno según el pulsador presionado
void pulsadores(){
  int u = digitalRead(19);
  delay(50);
  int d = digitalRead(18);
  delay(50);
  if (u == 0){
    sonido();
    if (turno == 99)
      turno = 0;
    else
      turno++;
  }
  else if (d == 0){
    sonido();
    if (turno == 0)
      turno = 99;
    else
      turno--;
  }
}
void playNote(int noteInt, long length){
  noTone(3);
  tone(4,noteInt,length);
  delay(length);
  noTone(4);
  tone(3,100); //petición de interrupción
}

//Procedimiento que emite un sonido por el altavoz
void sonido(){
  noTone(3);
  tone(4,frecuencia,30);
  delay(30);
  noTone(4);
  tone(3,100); //petición de interrupción
}

//Procedimiento que se ejecuta cuando se produce una interrupción
void tarea2(){
    imprimir();
}

//Bucle indefinido - Loop
void loop(){
   pulsadores();
}

