//1. librerias.
  //********************************************************
  #include <SPI.h>
  #include <LoRa.h>
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
  #define RFM95_INIT 5
  #define RFM95_RST 0

  //********************************************************
  //    - 2.4. Constantes
  #define RFM95_FREQ 915E6
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
    int packetSize = 0;
//4. Intancias.
  //********************************************************
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
    //1.1 Configuracion de Salidas:
      pinMode(RFM95_INIT,OUTPUT);
      pinMode(RFM95_RST, OUTPUT);
      pinMode(LED_azul, OUTPUT);
    //1.2 Configuracion de Entradas
    
  //2. Condiciones Iniciales.
    //****************************
    //    2.1 Estado de Salidas.
      digitalWrite(LED_azul,HIGH);
      digitalWrite(RFM95_RST, HIGH);
    //    2.2 Valores y Espacios de Variables
  //3. Configuracion de Perifericos:
    //****************************
    //-3.1 Initialize serial communication at 9600 bits per second:
      Serial.begin(9600);
      delay(10);
    //-3.2 Interrupciones Habilitadas.
      //****************************
      //interrupts ();
  //4. Prueba de Sitema Minimo Configurado.
    //****************************
    Serial.println("Sistema Minimo Configurado");
  //5. Configuracion de DEVICE externos.
    //****************************
    //-5.1 RFM95 Configuracion.
      LoRa.setPins(RFM95_CS, RFM95_RST, RFM95_INIT);

    //-5.2 RFM95 Iniciar.
      //****************************
      if (!LoRa.begin(RFM95_FREQ)) {
        Serial.println("Starting LoRa failed!");
        while (1);
      }
      Serial.println("LoRa radio init OK! FREQ= 915 Mhz");
      //****************************
}
void loop(){
  //1. Bienvenida Funcion 
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    while (inicio){
      welcome();        // Comprobamos el Sistema minimo de Funcionamiento.
      led_Monitor(2);
    }
  //2. Decodificar funcion serial
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    if(stringComplete){
      decodificar_solicitud();
    }
  //3. Ejecutar Funcion
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    if(codified_funtion){
      ejecutar_solicitud();
      // 3.1 Desactivar Banderas.
      codified_funtion=false;
    }
  //4. RFM95 Funciones.
    //-4.1 RFM95 RUN.
      packetSize = LoRa.parsePacket();
      if (packetSize) {
        // received a packet
        Serial.print("Received packet '");
  
        // read packet
        while (LoRa.available()) {
          Serial.print((char)LoRa.read());
        }
        // print RSSI of packet
        Serial.print("' with RSSI ");
        Serial.println(LoRa.packetRssi());
      }
}
//    Funciones de Logic interna del Micro.
  void welcome(){
    // Deshabilitamos Banderas
      inicio=false;
      Serial.println("Comenzamos el Programa");
      Serial.println("Esperamos recibir un Dato");
      Serial.println("ESP8266 ESCLAVO CONFIGURADO");
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
  void f2_serial_Enviar(int direccion, int buffer){
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