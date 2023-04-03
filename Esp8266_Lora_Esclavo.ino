//1. librerias.
  //********************************************************
  #include <SPI.h>
  #include <RH_RF95.h>
  //********************************************************
//2. Definicion de Pinout.
  //      Las Etiquetas para los pinout son los que comienzan con GPIO
  //      Es decir, si queremos activar la salida 1, tenemos que buscar la referencia GPIO 1, Pero solomante Escribir 1 sin "GPIO"
  //      NO tomar como referencia las etiquetas D1.
  //********************************************************
  //- 2.1 Definicion de etiquetas para las Entradas.

  //********************************************************
  //    - 2.2 Definicion de etiquetas para las Salidas.
  #define LED_azul 2

  //********************************************************
  //      - 2.3 Modulo de comunicaion rfm95
  #define RFM95_CS 15
  #define RFM95_INT 5
  #define RFM95_RST 0

  //********************************************************
  //    - 2.4. Constantes
  #define RF95_FREQ 915.0
  //********************************************************
//3. Variables Globales.
  String inputString;                     // Buffer recepcion Serial.
  volatile  bool stringComplete= false;   // Flag: mensaje Serial Recibido completo.
  //********************************************************
  //-3.1 Variables para las Interrupciones

  //********************************************************
  //-3.2 Variables Globales para Las Funciones.
  bool inicio=true;             // Habilitar mensaje de inicio por unica vez
  String funtion_Mode;          // Tipo de funcion para ejecutar.
  String funtion_Number;        // Numero de funcion a EJECUTAR.
  String funtion_Parmeter1;     // Parametro 1 de la Funcion;
  String funtion_Parmeter2;     // Parametro 2 de la Funcion;
  bool codified_funtion=false;  // Notifica que la funcion ha sido codificada.

  //********************************************************
  //-3.3 Variables para RF95
  int16_t packetnum = 0;  // packet counter, we increment per xmission
  unsigned int placa; // placa en el perimetro.
  unsigned int zona;  // Zona del perimetro.
  char radiopacket[32] = "012345 23456789 1   ";
//4. Intancias.
  //********************************************************
  RH_RF95 rf95(RFM95_CS, RFM95_INT);
//5. Funciones ISR.
  //********************************************************
  // 5.1 funciones de interrupcion.
    void serialEvent (){
      while (Serial.available()) {
        // get the new byte:
        char inChar = (char)Serial.read();
        // add it to the inputString:
        inputString += inChar;
        // if the incoming character is a newline, set a flag so the main loop can
        // do something about it:
        if (inChar == '\n') {
          stringComplete = true;
          codified_funtion=false;
        }
      }
    }
void setup() {
  //1. Configuracion de Puertos.
    //****************************
    //    1.1 Configuracion de Salidas:
    pinMode(RFM95_INT,OUTPUT);
    pinMode(RFM95_RST, OUTPUT);
    pinMode(LED_azul, OUTPUT);
    //    1.2 Configuracion de Entradas
    //****************************
  //2. Condiciones Iniciales.
    //    2.1 Estado de Salidas.
    digitalWrite(LED_azul,HIGH);
    digitalWrite(RFM95_RST, HIGH);
    //    2.2 Valores y Espacios de Variables
  //3. Configuracion de Perifericos:
    //****************************
    //    - 3.1 Initialize serial communication at 9600 bits per second:
    Serial.begin(9600);
    delay(10);
    //    - 3.2 Interrupciones Habilitadas.
      //****************************
      //interrupts ();
  //4. Prueba de Sitema Minimo Configurado.
    //****************************
    Serial.println("Sistema Minimo Configurado");
  //5. Configuracion de DEVICE externos.
    //****************************
    //    5.1 Configuracion RF95
    //    5.1.1 RESET.
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);
  
    //    5.1.2 INIT. RF95
    //****************************
    while (!rf95.init()) {
      Serial.println("LoRa radio init failed");
      while (1);
    }
    Serial.println("LoRa radio init OK!");
    //****************************
    //    5.1.3 SET FRECUENCY
    if (!rf95.setFrequency(RF95_FREQ)) {
      Serial.println("setFrequency failed");
      while (1);
    }
    Serial.print("Set Freq to: ");
    Serial.println(RF95_FREQ);  
    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95 / 96/97/98 modules using the transmitter pin PA_BOOST, then
    // you can set transmission powers from 5 to 23 dBm:

  
    //    5.1.4 SET POWER TRANSMIT
    //****************************
    rf95.setTxPower(23, false);
}
void loop(){
  //1. Mensaje de Bienvenida 
    // Para Comprobar el Sistema minimo de Funcionamiento.
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    while (inicio){
      welcome();
      led_Monitor(2);
    }

  //2. Decodificar funcion serial
    if(stringComplete){
      decodificar_solicitud();
    }
  //3. Ejecutar Funcion
    if(codified_funtion){
      ejecutar_solicitud();
      // 3.1 Desactivar Banderas.
      codified_funtion=false;
    }
  //4. Recibir Mensjae From RFM95W
    if (rf95.available()){
      // Should be a message for us now   
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
      
      if (rf95.recv(buf, &len))
      {
        digitalWrite(LED_azul, HIGH);
        RH_RF95::printBuffer("Received: ", buf, len);
        Serial.print("Got: ");
        Serial.println((char*)buf);
        Serial.print("RSSI: ");
        Serial.println(rf95.lastRssi(), DEC);
        
        // Send a reply
        uint8_t data[] = "And hello back to you";
        rf95.send(data, sizeof(data));
        rf95.waitPacketSent();
        Serial.println("Sent a reply");
        digitalWrite(LED_azul, LOW);
      }
      else
      {
        Serial.println("Receive failed");
      }
    }    
}
//    Funciones de Logic interna del Micro.
  void welcome(){
    // Deshabilitamos Banderas
    inicio=false;
    Serial.println("Comenzamos el Programa");
    Serial.println("Esperamos recibir un Dato");
    Serial.println("ESP8266 NODO 1");
  }
  void led_Monitor(int repeticiones){
    // Deshabilitamos Banderas
    int repetir=repeticiones;
    for (int encender=0; encender<=repetir; ++encender){
      digitalWrite(LED_azul, LOW);   // Led ON.
      delay(500);                    // pausa 1 seg.
      digitalWrite(LED_azul, HIGH);    // Led OFF.
      delay(500);                    // pausa 1 seg.
    }
  }
  void decodificar_solicitud(){
    //Deshabilitamos Banderas
    stringComplete=false;
    codified_funtion=true;
    Serial.println(inputString);         // Pureba de Comunicacion Serial.
    funtion_Mode=inputString.substring(0,1);
    funtion_Number=inputString.substring(1,2);
    funtion_Parmeter1=inputString.substring(2,3);
    funtion_Parmeter2=inputString.substring(3,4);
    inputString="";
    Serial.println("funcion: " + funtion_Mode);
    Serial.println("Numero: " + funtion_Number);
    Serial.println("Parametro1: " + funtion_Parmeter1);
    Serial.println("Parametro2: " + funtion_Parmeter2+ "\n");
    //Serial.println("Numero de funcion: ")
  }
//    Funciones de dispositivo externos.
  void rf95_mensaje(){
    Serial.println("Sending to rf95_server");     // ___DEBUG
    char radiopacket[32] = "012345 23456789 1   ";
    itoa(packetnum++, radiopacket+16, 10);                // 7. Convertimos una cadena a nuemeros de base 10 en caracter.
    Serial.print("Sending ");
    Serial.println(radiopacket);
    radiopacket[31] = 0;
    Serial.println("Sending..."); 
    delay(10);
  }
  void rf95_enviar(){
    rf95.send((uint8_t *)radiopacket, 20);
    digitalWrite(LED_azul, HIGH);
    Serial.println("Waiting for packet to complete...");
    delay(10);
    rf95.waitPacketSent();
  }
  void rf95_recibir(){
    // Now wait for a reply
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    Serial.println("Waiting for reply..."); 
    delay(10);
    if (rf95.waitAvailableTimeout(1000))
    { 
      // Should be a reply message for us now   
      if (rf95.recv(buf, &len))
    {
        Serial.print("Got reply: ");
        Serial.println((char*)buf);
        Serial.print("RSSI: ");
        Serial.println(rf95.lastRssi(), DEC);    
      }
      else
      {
        Serial.println("Receive failed");
      }
    }
    else
    {
      Serial.println("No reply, is there a listener around?");
    }
    
    delay(1000);
    digitalWrite(LED_azul, LOW);
  }
//    Funciones Seleccionadas para Ejecutar.
  void f1_Destellos (int repeticiones, int tiempo){
    int veces=repeticiones;
    int retardo=tiempo*100;
    Serial.println("Ejecutando F1.. \n");
    for(int repetir=0; repetir<veces; ++repetir){
      delay(retardo);                  // pausa 1 seg.
      digitalWrite(LED_azul, LOW);     // Led ON.
      delay(retardo);                  // pausa 1 seg.
      digitalWrite(LED_azul, HIGH);    // Led OFF.
    }
  }
  void f2_serial_Enviar(){
    // Deshabilitamos Banderas
    Serial.println("hola");         // Pureba de Comunicacion Serial.
  }
//    Ultima Funcion.
  void ejecutar_solicitud(){
    // Deshabilitamos Banderas
    if (funtion_Number=="1"){
      int x1=funtion_Parmeter1.toInt();
      int x2=funtion_Parmeter2.toInt();
      Serial.println("funion Nº1");
      f1_Destellos(x1,x2);
    }
    if (funtion_Number=="2"){
      Serial.println("funion Nº2");
    }
    if (funtion_Number=="3"){
      Serial.println("funion Nº3");
    }
    else{
      Serial.println("Ninguna Funcion");
    }
      
  }