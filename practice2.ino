const int LEE_SDA = 2;
const int LEE_SCL = 3;
const int ESC_SDA = 4;
const int ESC_SCL = 5;
int pos;
int valor;

void setup() {

Serial.begin(9600);  

  // Terminales de entrada
  pinMode(LEE_SDA, INPUT);
  pinMode(LEE_SCL, INPUT);

  // Terminales de salida
  pinMode(ESC_SDA, OUTPUT);
  pinMode(ESC_SCL, OUTPUT);

  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);  

}

void loop() {
  Serial.println("Bienvenido al gestor de memoria. Por favor, indique la operacion a realizar");
  Serial.println("1. Guardar valor.");
  Serial.println("2. Leer posicion.");
  Serial.println("3. Inicializar memoria.");
  Serial.println("4. Mostrar el contenido");
  
  while (Serial) {
  switch((int)Serial.parseInt()) {
    case 1:  Serial.println("\nIndique el valor:\n");
             while(Serial.available() <= 0);
             valor = Serial.parseInt();
             if (valor < 0 || valor > 255){
                Serial.println("\nEl valor introducido esta fuera del rango permitido. \nVuelva a seleccionar una accion");
                break;
             }
             
             Serial.println("\nIndique la posicion:\n");
             while(Serial.available() <= 0);
             pos = Serial.parseInt();
             if (pos < 0 || pos > 127){
                Serial.println("\nLa direccion introducido esta fuera del rango permitido. \nVuelva a seleccionar una accion.");
                break;
             }
             
             writeMem(valor, pos);
             break;

    case 2:  Serial.println("\nIndique la posicion:\n");
             while(Serial.available() <= 0);
             pos = Serial.parseInt();
             if (0 <= pos < 128) readMem(pos);
             break;
             
    case 3:  Serial.println("\nIndique el valor que se establecera en toda la memoria:\n");
             while(Serial.available() <= 0);
             valor = Serial.parseInt();
             if (valor < 0 || valor > 255){
                Serial.println("\nEl valor introducido esta fuera del rango permitido. \nVuelva a seleccionar una accion.");
                break;
             }
             
             inicializar(valor);
             break;  
             
    case 4:  Serial.println("\nEl contenido de los 128 bytes del M24CO1 es:\n");
             mostrarContenido();
             break;

     case 5: Serial.println("\nInicializar toda la memoria del M24C01 a este valor\n");
             while(Serial.available() <= 0);
             valor = Serial.parseInt();
             if (valor < 0 || valor > 255){
                Serial.println("\nEl valor introducido esta fuera del rango permitido. \nVuelva a seleccionar una accion.");
                break;
             }
             
             inicializarPage(valor);
             break;  
    }
  }
}

void writeMem(int dato, int direccion) {
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
  
  // BYTE ADDR
  for (int i = 7; i >= 0; i--) {
    if (bitRead(direccion,i) == 0) e_Bit0();
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
  
  // STOP
  stopCom();
  
  // SHOW MESSAGE
  Serial.println();
  Serial.print("En la posicion ");
  Serial.print(direccion);
  Serial.print(" se ha introducido el dato ");
  Serial.print(dato);

}

void readMem(int direccion) {  
  volatile byte dato;
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
  
  // SHOW MESSAGE
  Serial.println();
  Serial.print("En la posicion ");
  Serial.print(direccion);
  Serial.print(" se encuentra el dato: ");
  Serial.print(dato);

  
}

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

void inicializar(int dato) {
  for (int i = 0; i < 128; i++) writeMem(dato,i);
}

void inicilizarPage(int dato) {
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
  
  // BYTE ADDR
  for (int i = 7; i >= 0; i--) {
    if (bitRead(direccion,i) == 0) e_Bit0();
    else e_Bit1();
  }
  
  // ACK
  if (r_Bit() == 1) goto Write;
  
  // DATA IN
  for (int i = 60; i >= 0; i--) {
    if (bitRead(dato,i) == 0) e_Bit0();
    else e_Bit1();
    if (r_Bit() == 1) goto Write;
  }
  
  // ACK
  if (r_Bit() == 1) goto Write;
  
  // STOP
  stopCom();
  
  // SHOW MESSAGE
  Serial.println();
  Serial.print("En toda la memoria se ha introducido el dato ");
  Serial.print(dato);
}


void mostrarContenido() {
  for (int i = 0; i < 128; i++) readMem(i);
}
