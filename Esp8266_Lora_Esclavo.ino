//1. librerias.

  //- 1.1 Librerias
    //****************************
    #include <SPI.h>
    #include <LoRa.h>
  
//2. Definicion de Pinout.
  //  Las Etiquetas para los pinout son los que comienzan con GPIO
  //  Es decir, si queremos activar la salida 1, tenemos que buscar la referencia GPIO 1, Pero solomante Escribir 1 sin "GPIO"
  //  NO tomar como referencia las etiquetas D1, D2,D3, ....Dx.
  
  //-2.1 Definicion de etiquetas para las Entradas.
    //********************************************************
    #define PB_ENTER 0
  //-2.2 Definicion de etiquetas para las Salidas.
    //********************************************************
    #define LED_azul 2

  //-2.3 RFM95 Modulo de comunicacion
    #define RFM95_RST 16
    #define RFM95_CS 15
    #define RFM95_INIT 5

  //-2.4. Constantes
    //********************************************************
    #define RFM95_FREQ 915E6
  
//3. Variables Globales.
  //********************************************************

  //-3.1 Variables para las Interrupciones
    String inputString;                     // Buffer recepcion Serial.
    volatile  bool stringComplete= false;   // Flag: mensaje Serial Recibido completo.
  
  //-3.2 Variables Globales para Las Funciones.
    //********************************************************
    bool inicio=true;             // Habilitar mensaje de inicio por unica vez
    String funtion_Mode;          // Tipo de funcion para ejecutar.
    String funtion_Number;        // Numero de funcion a EJECUTAR.
    String funtion_Parmeter1;     // Parametro 1 de la Funcion.
    String funtion_Parmeter2;     // Parametro 2 de la Funcion.
    String funtion_Parmeter3;     // Parametro 3 de la Funcion.
    bool codified_funtion=false;  // Notifica que la funcion ha sido codificada.
    // Variables Para la Placa.
    int Zona_1 = 4;
    int Zona_2 = 10;
    int Zona_3 = 9;
    int Aceptar= 0;
    int Zonas=0;
    String Compañeros="0";
    volatile int x1=0;
    volatile int x2=0;
    volatile int x3=0;

    String Nodo ="1";
    bool responder=false;
    int Nodos = 2;         // Establece Cuantos Nodos Conforman La Red a6
  //-3.3 RFM95 Variables.
    //********************************************************
    int16_t packetnum = 0;  // packet counter, we increment per xmission
    unsigned int placa; // placa en el perimetro.
    unsigned int zona;  // Zona del perimetro.
    char radiopacket[32] = "012345 23456789 1   ";

    // Variables para enviar.
    int packetSize = 0;
    String outgoing;              // outgoing message
    byte msgCount = 0;            // count of outgoing messages
    byte localAddress = 0x01;     // address of this device           a3
    byte destination = 0xFF;      // destination to send to           a4
    long lastSendTime = 0;        // last send time
    int interval = 2000;          // interval between sends.

    // Variables para recibir.
    int recipient;          // recipient address
    byte sender;            // sender address
    byte incomingMsgId;     // incoming msg ID
    byte incomingLength;    // incoming msg length
    String incoming = "";
//4. Intancias.
  //********************************************************

//5. Funciones ISR.
  //-5.1 Serial Function.
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
  //-5.2 Extern Function
void setup(){
  //1. Configuracion de Puertos.
    //****************************
    //1.1 Configuracion de Salidas:
      pinMode(RFM95_INIT,OUTPUT);
      pinMode(RFM95_RST, OUTPUT);
      pinMode(LED_azul, OUTPUT);
    //1.2 Configuracion de Entradas
      pinMode(Zona_1, INPUT_PULLUP);
      pinMode(Zona_2, INPUT_PULLUP);
      pinMode(Zona_3, INPUT_PULLUP);
      pinMode(Aceptar, INPUT_PULLUP);

  //2. Condiciones Iniciales.
    //-2.1 Estado de Salidas.
      digitalWrite(LED_azul,HIGH);
      digitalWrite(RFM95_RST, HIGH);
    //-2.2 Valores y Espacios de Variables
      //sender=String(Nodo).toInt();
  //3. Configuracion de Perifericos:
    //-3.1 Initialize serial communication at 9600 bits per second:
      Serial.begin(9600);
      delay(10);
    //-3.2 Interrupciones Habilitadas.
      //****************************
      //attachInterrupt (digitalPinToInterrupt (Aceptar), pinChange, CHANGE);  // attach interrupt handler for D2
      //interrupts ();
  //4. Prueba de Sitema Minimo Configurado.
    //****************************
    Serial.println("Sistema Minimo Configurado");
  //5. Configuracion de DEVICE externos.
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
      led_Monitor(5);
    }
  //2. Decodificar funcion serial
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    if(stringComplete){
      decodificar_solicitud();
    }
  //3. Ejecutar Funcion
    //-3.1 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      if(codified_funtion){
        ejecutar_solicitud();
        // 3.1 Desactivar Banderas.
        codified_funtion=false;
      }
    //-3.2 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      if(!digitalRead(Aceptar)){
        inicio=true;
      }
    //-3.3 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      reviso();
  //5. RFM95 Funciones.
    //-5.1 RFM95 RUN.
      RFM95_recibir(LoRa.parsePacket());

    //-5.2 Responsde si el mensaje es para él.
      if(responder){
        destination=sender;                           // add destination address.
        localAddress=String(Nodo).toInt();            // add sender address.
        // msgCount=1;                                   // add message ID.
        RFM95_enviar(Compañeros+char(Zonas));
      }
      
}
//1. Funciones de Logic interna del Micro.
  void welcome(){
    // Deshabilitamos Banderas
      inicio=false;
      Serial.println("Sistema Iniciado");
      Serial.println("Nodo: "+Nodo+" Ready");
      
  }
  void led_Monitor(int repeticiones){
    // Deshabilitamos Banderas
    int repetir=repeticiones;
    for (int encender=0; encender<repetir; ++encender){
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
    Serial.println("Parametro2: " + funtion_Parmeter2);
    Serial.println("Parametro3: " + funtion_Parmeter3+ "\n");
  }
//2. Funciones Seleccionadas para Ejecutar.
  //-2.1 Funciones Tipo A.
    void a1_Nodo_Destellos (int repeticiones, int tiempo){
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
    void a2_Serial_Enviar(int direccion, int buffer){
      // Deshabilitamos Banderas
      Serial.println("hola");         // Pureba de Comunicacion Serial.
    }
    void a3_Nodo_Direccion(){
      // Deshabilitamos Banderas
      Nodo=funtion_Parmeter1+funtion_Parmeter2;
      Serial.println("Nodo:"+ Nodo + "Configurado Exitosamente");         // Pureba de Comunicacion Serial.
    void a2_Nodo_Serial_Enviar(int direccion, int buffer){
      // Deshabilitamos Banderas
      Serial.println("hola");         // Pureba de Comunicacion Serial.
    }
    void a3_Nodo_Direccion_Local(int paramatro_1){
      localAddress = paramatro_1;

    }
    void a4_Nodo_Direccion_Destino(int direccion_aux){
      destination = direccion_aux;
    }
    void a5_Nodo_Mensaje_ID(){      
      msgCount++;                           // increment message ID.
    }
    void a6_Nodo_Numeros(int parametro_1){
      Nodos=parametro_1;
    }

  //-2.2 Funciones tipo B.
    void b1_estados(){
      int a=0;
    }
    void b2 (int a1, int a2){
      int aa=a1;
      int aa2=a2;
    }
    void b3 (int a1, int a2){
      int aa=a1;
      int aa2=a2;
    }
    void b4 (int a1, int a2){
      int aa=a1;
      int aa2=a2;
    }
    void b5 (int a1, int a2){
      int aa=a1;
      int aa2=a2;
    }
    void b6 (int a1, int a2){
      int aa=a1;
      int aa2=a2;
    }
//3. Gestiona las funciones a Ejecutar.
  void ejecutar_solicitud(){
    // Deshabilitamos Banderas
      x1=funtion_Parmeter1.toInt();
      x2=funtion_Parmeter2.toInt();
      x3=funtion_Parmeter3.toInt();
    // Function Tipo A
      if (funtion_Mode=="A" && funtion_Number=="1"){
        Serial.println("funion A Nº1");
        a1_Nodo_Destellos(x1,x2);
      }
      if (funtion_Mode=="A" && funtion_Number=="2"){
        Serial.println("funion A Nº2");
        Serial.println("Hola Funcion 2");
      }
      if (funtion_Mode=="A" && funtion_Number=="3"){
        Serial.println("funion A Nº3");
        String Nodo_direccion_aux = funtion_Parmeter1+funtion_Parmeter2;
        int Nodo_direccion = Nodo_direccion_aux.toInt();
        a3_Nodo_Direccion_Local(Nodo_direccion);
      }
      if (funtion_Mode=="A" && funtion_Number=="4"){
        Serial.println("funion A Nº4");
      }
      if (funtion_Mode=="A" && funtion_Number=="5"){
        Serial.println("funion A Nº5");
        RFM95_enviar("Maestro");
      }
      if (funtion_Mode=="A" && funtion_Number=="6"){
        Serial.println("funion A Nº6: Numero de Nodos");
        String Nodos_numeros_aux = funtion_Parmeter1+funtion_Parmeter2;
        int Nodos_numeros = Nodos_numeros_aux.toInt();
        a6_Nodo_Numeros(Nodos_numeros);
      }
      if (funtion_Mode=="A" && funtion_Number=="7"){
        Serial.println("funion A Nº7");
      }
      if (funtion_Mode=="A" && funtion_Number=="8"){
        Serial.println("funion A Nº8");
      }
      if (funtion_Mode=="A" && funtion_Number=="9"){
        Serial.println("funion A Nº9");
      }
      if (funtion_Mode=="A" && funtion_Number=="0"){
        Serial.println("funion A Nº0");
      }
    // Function Tipo B
      if (funtion_Mode=="B" && funtion_Number=="1"){
        Serial.println("funion B Nº1");
        fb1_estados();
      }
      if (funtion_Mode=="B" && funtion_Number=="2"){
        Serial.println("funion B Nº2");
        Serial.println("Hola Funcion 2");
      }
      if (funtion_Mode=="B" && funtion_Number=="3"){
        Serial.println("funion B Nº3");
      }
      if (funtion_Mode=="B" && funtion_Number=="4"){
        Serial.println("funion B Nº4");
      }
      if (funtion_Mode=="B" && funtion_Number=="5"){
        Serial.println("funion B Nº5");
        RFM95_enviar("Maestro");
      }
      if (funtion_Mode=="B" && funtion_Number=="6"){
        Serial.println("funion B Nº6");
      }        
      if (funtion_Mode=="B" && funtion_Number=="7"){
        Serial.println("funion B Nº7");
      }
      if (funtion_Mode=="B" && funtion_Number=="8"){
        Serial.println("funion B Nº8");
      }     
      if (funtion_Mode=="B" && funtion_Number=="9"){
        Serial.println("funion B Nº9");
      }     
      if (funtion_Mode=="B" && funtion_Number=="0"){
        Serial.println("funion B Nº0");
      }                            
    else{
      Serial.println("Ninguna Funcion");
    }
      
  }
//4. Funcion que Revisa estados a ser enviados.
  //-4.1 Estados de Zonas.
    void reviso(){
      Zonas  = (char)digitalRead(Zona_1);
      Zonas += (char)digitalRead(Zona_2);
      
    }

//5. Funciones de Dispositivos Externos.
  //-5.1 RFM95 RECIBIR.
    void RFM95_recibir(int packetSize){
      if (packetSize == 0) return;          // if there's no packet, returnº1
      // read packet header bytes:
      recipient = LoRa.read();         // recipient address
      sender = LoRa.read();            // sender address
      incomingMsgId = LoRa.read();     // incoming msg ID
      incomingLength = LoRa.read();    // incoming msg length
      incoming = "";

      while (LoRa.available()){
        incoming += (char)LoRa.read();
      }
      if (incomingLength != incoming.length()) {   // check length for error
        Serial.println("error: message length does not match length");
        return;                             // skip rest of function
      }
      // if the recipient isn't this device or broadcast,
      if (recipient != localAddress && recipient != 0xFF) {
        Serial.println("Sent to: 0x" + String(recipient, HEX));
        Serial.println("This message is not for me.");
        return;                             // skip rest of function
      }
      // if message is for this device, or broadcast, print details:
      Serial.println("Received from: 0x" + String(sender, HEX));
      Serial.println("Sent to: 0x" + String(recipient, HEX));
      Serial.println("Message ID: " + String(incomingMsgId));
      Serial.println("Message length: " + String(incomingLength));
      Serial.println("Message: " + incoming);
      Serial.println("RSSI: " + String(LoRa.packetRssi()));
      Serial.println("Snr: " + String(LoRa.packetSnr()));
      Serial.println();

      //Verificamos que el mensaje que nos llega sea para nosotros
      if(String(recipient)==Nodo){
        inputString=incoming;
        stringComplete=true;
        responder=true;
      }

    }
  //-5.2 RFM95 ENVIAR.
    void RFM95_enviar(String outgoing){
      LoRa.beginPacket();                   // start packet
      LoRa.write(destination);              // add destination address
      LoRa.write(localAddress);             // add sender address
      LoRa.write(msgCount);                 // add message ID
      LoRa.write(outgoing.length());        // add payload length
      LoRa.print(outgoing);                 // add payload
      LoRa.endPacket();                     // finish packet and send it
      responder=false;
    }    