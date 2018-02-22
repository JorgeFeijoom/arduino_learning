//Asignación de pines
const int LEE_SDA = 2;
const int LEE_SCL = 3;
const int ESC_SDA = 4;
const int ESC_SCL = 5;
int pos;
int valor;
int pagina;

void setup() {
//Inicia canal serie velocidad bps
Serial.begin(9600);  

  // Terminales de entrada
  pinMode(LEE_SDA, INPUT);
  pinMode(LEE_SCL, INPUT);

  // Terminales de salida
  pinMode(ESC_SDA, OUTPUT);
  pinMode(ESC_SCL, OUTPUT);

  //No interv el bus
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);  

}
//Mostrar opciones del programa
void loop(){
  Serial.println("\n__________________________\n");
  Serial.println("1. Guardar valor");
  Serial.println("2. Leer posicion");
  Serial.println("3. Inicializar memoria");
  Serial.println("4. Mostrar el contenido");
  Serial.println("5. Page Write");
  Serial.println("6. Pos Read");
  Serial.println("7. Page Read");
  Serial.println("__________________________\n");
  
//Petición y tratamiento de datos según la opción
while (Serial) {
  switch((int)Serial.parseInt()) {
    case 1:  Serial.println("\nIndique el valor:\n");
             while(Serial.available() <= 0);
             valor = Serial.parseInt();
             if (valor < 0 || valor > 255){
                Serial.println("\nColoque un valor correcto [0-255]\n");
                break;
             }
             
             Serial.println("\nIndique la posicion:\n");
             while(Serial.available() <= 0);
             pos = Serial.parseInt();
             if (pos < 0 || pos > 127){
                Serial.println("\nColoque un valor correcto [0-127]\n");
                break;
             }
             
             writeMem(valor, pos);
             showMessage(valor,pos);
             loop();
             break;

    case 2:  Serial.println("\nIndique la posicion:\n");
             while(Serial.available() <= 0);
             pos = Serial.parseInt();
             if (0 <= pos < 128) readMem(pos);
             loop();
             break;
             
    case 3:  Serial.println("\nIndique el valor que se establecera en toda la memoria:\n");
             while(Serial.available() <= 0);
             valor = Serial.parseInt();
             if (valor < 0 || valor > 255){
                Serial.println("\nCColoque un valor correcto [0-127]\n");
                break;
             }
             
             inicializar(valor);
             showMessage2(valor);
             loop();
             break;  
             
    case 4:  Serial.println("\nEl contenido de los 128 bytes del M24CO1 es:\n");
             mostrarContenido();
             loop();
             break;
    
    case 5:  Serial.print("\nInserte el valor al que desea inicializar la memoria (PAGE WRITE): \n");
             while(Serial.available() <= 0);
             valor = Serial.parseInt();
             if (valor < 0 || valor > 255){
                Serial.println("\nColoque un valor correcto [0-255]\n");
                break;
             }
             Serial.println("\nIndique la pagina:\n");
             while(Serial.available() <= 0);
             pagina = Serial.parseInt();
             if (0 <= pagina < 128) writePage(valor,pagina*8);
             showMessage3(valor,pagina);
             loop();
             break;

    case 6:  Serial.println("\nIndique la posicion:\n");
             while(Serial.available() <= 0);
             pos = Serial.parseInt();
             if (0 <= pos < 128) readPos(pos);
             loop();
             break;
             
    case 7:  Serial.println("\nIndique la pagina:\n");
             while(Serial.available() <= 0);
             pagina = Serial.parseInt();
             if (0 <= pagina < 128) readPage(pagina*8);
             loop();
             break;

    }
  }
}

//Escribe un valor en la dirección de memoria asignada
void writeMem(int dato, int direccion) {
  Write:
  //START
  startCom();
  
  //DEV SEL, bus direc
  e_Bit1();
  e_Bit0();
  e_Bit1();
  e_Bit0();
  e_Bit0();
  e_Bit0();
  e_Bit0();
  
  //R/W
  e_Bit0();
  
  //ACK (1 emis SDA desb, escucha; 0 recep resp)
  if (r_Bit() == 1) goto Write;
  
  //BYTE ADDR
  for (int i = 7; i >= 0; i--) {
    if (bitRead(direccion,i) == 0) e_Bit0();
    else e_Bit1();
  }
  
  //ACK
  if (r_Bit() == 1) goto Write;
  
  //DATA IN
  for (int i = 7; i >= 0; i--) {
    if (bitRead(dato,i) == 0) e_Bit0();
    else e_Bit1();
  }
  
  //ACK
  if (r_Bit() == 1) goto Write;
  
  //STOP
  stopCom();
}

//Lee el contenido de una posición de memoria definida
void readMem(int direccion) {  
  volatile byte dato;
  // START
  startCom();
  
  // DEV SEL, bus direc
  e_Bit1();
  e_Bit0();
  e_Bit1();
  e_Bit0();
  e_Bit0();
  e_Bit0();
  e_Bit0();
  
  // R/W
  e_Bit0();
  
  // ACK
  if (r_Bit() == 1) readMem(direccion);
  
  // BYTE ADDR
  for (int i = 7; i >= 0; i--) {
    if (bitRead(direccion,i) == 0) e_Bit0();
    else e_Bit1();
    
  }
  // ACK
  if (r_Bit() == 1) readMem(direccion);
  
  // START
  startCom();
  
  // DEV SEL
  e_Bit1();
  e_Bit0();
  e_Bit1();
  e_Bit0();
  e_Bit0();
  e_Bit0();
  e_Bit0();
  
  // R/W
  e_Bit1();
  
  //ACK
  if (r_Bit() == 1) readMem(direccion);
  
  //DATA OUT
  for (int i = 7; i >= 0; i--) bitWrite(dato,i,r_Bit());
  // NO ACK

  //STOP
  stopCom();
  
  //SHOW MESSAGE
  Serial.println();
  Serial.print("En la posicion ");
  Serial.print(direccion);
  Serial.print(" se encuentra el dato: ");
  Serial.print(dato); 
}


//Inicializa toda la memoria con un dato
void inicializar(int dato) {
  for (int i = 0; i < 128; i++) writeMem(dato,i);
}


//Muestra el contenido que hay guardado en cada posición de memoria
void mostrarContenido() {
//Utilizamos un vector para alinear a la derecha los valores
char alinear[10];
    for (int i = 0; i < 128; i++){
      //Guardamos en el vector, con la alineacion, el contenido de memoria
      sprintf(alinear, "%5d", readByte(i));
      Serial.print(alinear);
      Serial.print("\t");
      if ((i+1)%8==0) Serial.println();
    }
}

//Escribe un valor en una página completa
void writePage(int dato, int pagina) {
Write:
  // START
startCom();
  
  // DEV SEL
e_Bit1();
e_Bit0();
e_Bit1();
e_Bit0();
e_Bit0();
e_Bit0();
e_Bit0();

  
  // R/W
e_Bit0();
  
  // ACK
if (r_Bit() == 1) goto Write;
  
for(int j = 0; j<8; j++){
  // BYTE ADDR
  for (int i = 0; i< 8; i++) {
    if (bitRead(pagina+j,i) == 0) e_Bit0();
    else e_Bit1();
  }
  
  // ACK
  if (r_Bit() == 1) goto Write;

  // DATA IN
  for (int i = 7; i >= 0; i--) {
    if (bitRead(dato,i) == 0) e_Bit0();
    else e_Bit1();
  }
  
  // ACK
  if (r_Bit() == 1) goto Write;
}
  // STOP
  stopCom();

}

//Lee el valor de las posiciones de memoria de una página completa
void readPage(int pagina){
int dato;
inicio:
  for(int j = 0; j<8; j++){
  // START
  startCom();
  
  // DEV SEL
  e_Bit1();
  e_Bit0();
  e_Bit1();
  e_Bit0();
  e_Bit0();
  e_Bit0();
  e_Bit0();
  
  // R/W
  e_Bit0();

  // ACK
  if (r_Bit() == 1) goto inicio;

  // BYTE ADDR
  for (int i = 7; i >= 0; i--) {
    if (bitRead(pagina+j,i) == 0) e_Bit0();
    else e_Bit1();
  }
  // ACK
  if (r_Bit() == 1) goto inicio;

// START
  startCom();
  
  // DEV SEL
  e_Bit1();
  e_Bit0();
  e_Bit1();
  e_Bit0();
  e_Bit0();
  e_Bit0();
  e_Bit0();
  
  // R/W
  e_Bit1();
  
  // ACK
  if (r_Bit() == 1) goto inicio;
  
  // DATA OUT
  for (int i = 7; i >= 0; i--) bitWrite(dato,i,r_Bit());
  // NO ACK

  Serial.print(dato);
  Serial.print("\t");
  // STOP
  stopCom();
  }
}

//Lee el contenido que se encuentra en una memoria y devuelve ese dato
int readByte(int direccion) {  
  int dato;
  // START
  startCom();
  // DEV SEL
  e_Bit1();
  e_Bit0();
  e_Bit1();
  e_Bit0();
  e_Bit0();
  e_Bit0();
  e_Bit0();
  
  // R/W
  e_Bit0();
  
  // ACK
  if (r_Bit() == 1) readMem(direccion);
  
  // BYTE ADDR
  for (int i = 7; i >= 0; i--) {
    if (bitRead(direccion,i) == 0) e_Bit0();
    else e_Bit1();
    
  }
  // ACK
  if (r_Bit() == 1) readMem(direccion);
  
  // START
  startCom();
  
  // DEV SEL
  e_Bit1();
  e_Bit0();
  e_Bit1();
  e_Bit0();
  e_Bit0();
  e_Bit0();
  e_Bit0();
  
  // R/W
  e_Bit1();
  
  // ACK
  if (r_Bit() == 1) readMem(direccion);
  
  // DATA OUT
  for (int i = 7; i >= 0; i--) bitWrite(dato,i,r_Bit());
  // NO ACK

  // STOP
  stopCom();
  
  return dato;  
}
//Muestra el contenido que hay guardadodesde una posición de memoria
void readPos(int pos) {
    for (int i = pos; i < 128; i++){
      //Mostramos el contenido
      Serial.print(readByte(i));
      Serial.print("\t");
      if ((i+1)%8==0) Serial.println();
    }
}
//Muestra el mensaje cuando se introduce algún dato en alguna posición de memoria
void showMessage(int dato, int direccion){
  Serial.println();
  Serial.print("En la posicion; ");
  Serial.print(direccion);
  Serial.print(" se ha introducido el dato: ");
  Serial.print(dato);
  Serial.println("\n");
}

//Muestra el mensaje cuando se introduce algún dato en toda la memoria
void showMessage2(int dato){
  Serial.println();
  Serial.print("Se ha introducido el dato: ");
  Serial.print(dato);
  Serial.print(" en toda la memoria");
  Serial.println("\n");
}

//Muestra el mensaje cuando se introduce algún dato en una página
void showMessage3(int dato, int pagina){
  Serial.print("Se ha introducido el dato: ");
  Serial.print(dato);
  Serial.print(" en la pagina: ");
  Serial.print(pagina);
  Serial.print("\n");
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void startCom() {
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);
  
  while (digitalRead(LEE_SDA) == HIGH && digitalRead(LEE_SCL) == HIGH){
    digitalWrite(ESC_SDA, LOW);
    digitalWrite(ESC_SCL, HIGH);
    
    digitalWrite(ESC_SDA, LOW);
    digitalWrite(ESC_SCL, LOW);    
  }
  
}

void stopCom() {
  digitalWrite(ESC_SDA, LOW);
  digitalWrite(ESC_SCL, LOW);
  
  digitalWrite(ESC_SDA, LOW);
  digitalWrite(ESC_SCL, HIGH);
    
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);    

  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);    

}

void e_Bit1() {
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, LOW);
  
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);
    
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);    

  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, LOW);   
}

void e_Bit0() {
  digitalWrite(ESC_SDA, LOW);
  digitalWrite(ESC_SCL, LOW);
  
  digitalWrite(ESC_SDA, LOW);
  digitalWrite(ESC_SCL, HIGH);
    
  digitalWrite(ESC_SDA, LOW);
  digitalWrite(ESC_SCL, HIGH);    

  digitalWrite(ESC_SDA, LOW);
  digitalWrite(ESC_SCL, LOW);   
}

byte r_Bit() {
  int valor = 0;
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, LOW);
  
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);
  
  if (digitalRead(LEE_SDA) == HIGH) valor = 1;
  
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, LOW); 
  
  return valor;
}
