//1. librerias.

  //- 1.1 Librerias
    //****************************
    #include <SPI.h>
    #include <LoRa.h>
    #include <Ticker.h>
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
  //-2.5 timer


//3. Variables Globales.
  //********************************************************

  //-3.1 Variables para las Interrupciones
    String inputString;                     // Buffer recepcion Serial.
    volatile  bool stringComplete= false;   // Flag: mensaje Serial Recibido completo.
    volatile  bool flag_ISR_prueba=false;   // Flag: prueba para interrupcion serial.
    volatile  int contador=0;
    volatile  bool flag_temporizador_1=false;
  //-3.2 Variables Globales para Las Funciones.
    //********************************************************
    bool inicio=true;             // Habilitar mensaje de inicio por unica vez
    String funtion_Mode;          // Tipo de funcion para ejecutar.
    String funtion_Number;        // Numero de funcion a EJECUTAR.
    String funtion_Parmeter1;     // Parametro 1 de la Funcion.
    String funtion_Parmeter2;     // Parametro 2 de la Funcion.
    String funtion_Parmeter3;     // Parametro 3 de la Funcion.
    
    volatile int x1=0;
    volatile int x2=0;
    volatile int x3=0;

    bool codified_funtion=false;  // Notifica que la funcion ha sido codificada.
    
    bool flag_Un_segundo=false;
    // Variables Para Hardware.
      //Entradas
        int Zona_1 = 4;         // Entrada de Zona 1
        int Zona_2 = 10;        // Entrada de Zona 2
        int Zona_3 = 9;         // Entrada de Zona 3
        int Aceptar= 0;         // Entrada de Pulsador Aceptar


      unsigned long tiempo1;
      unsigned long tiempo2;
      int Zonas=0;
      String Compañeros="0";


      String Nodo ="1";
      bool responder=false;
      int Nodos = 2;         // Establece Cuantos Nodos Conforman La Red a6.
      bool modo_Continuo;
      // Variables para Logica interna
      byte compañeros1;
      byte compañeros2;
      String Nodo_info="";
      String letras="";
      String Nodo_cercano=""; // Guardo la direccion del Nodo que escribe
    // Alarmas
      int Alarma_Zona_1=0;
      int Alarma_Zona_2=0;
      int Alarma_Zona_3=0;
    // Eventos
      bool Nodo_Reconocido=0;
  //-3.3 RFM95 Variables.
    //********************************************************
    int16_t packetnum = 0;  // packet counter, we increment per xmission
    unsigned int placa; // placa en el perimetro.
    unsigned int zona;  // Zona del perimetro.
    byte master=0xFF;
    byte siguiente=2; // Direccion del Nodo que sigue para enviar mensaje
    // Variables para enviar.
      int packetSize = 0;
      String outgoing;              // outgoing message
      byte msg1_Write = 0;            // Habilito bandera del Nodo que envia 
      byte msg2_Write = 0;            // Habilito bandera del Nodo que envia
      byte localAddress = 0x01;     // address of this device           a3
      byte destination = 0xFF;      // destination to send to           a4
      long lastSendTime = 0;        // last send time
      int interval = 2000;
      byte msg_ID;    // en modo continuo este numero incrementa automaticamente.          // interval between sends.
    // Variables para recibir.
      int recipient;          // recipient address
      byte sender;            // sender address
      byte incomingMsgId1;     // incoming msg ID
      byte incomingMsgId2;     // incoming msg ID
      byte nodo_informa;       // informacion particular que envia el nodo
      byte incomingLength;    // incoming msg length
      String incoming = "";
//4. Intancias.
  //********************************************************
  Ticker temporizador_1;
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
    ICACHE_RAM_ATTR void ISR_1(){
      flag_ISR_prueba=true;
    }
    //-5.3 Interrupciones por Timer 1.
    void ISR_temporizador_1(){
      if(modo_Continuo){
        flag_temporizador_1=true;
        responder=true;
      }
      
    }
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
  //3. Configuracion de Perifericos:
    //-3.1 Initialize serial communication at 9600 bits per second:
      Serial.begin(9600);
      delay(10);
    //-3.2 Temporizador.
    //-3.2 Interrupciones Habilitadas.
      //****************************
      attachInterrupt (digitalPinToInterrupt (PB_ENTER), ISR_1, FALLING);  // attach interrupt handler for D2
      temporizador_1.attach(3, ISR_temporizador_1);
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
      led_Monitor(3);
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
        inputString="";
      }
    //-3.2 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      reviso();
  //4. Atender Las fucniones desde activas desde ISR.
    if(flag_ISR_prueba){
      flag_ISR_prueba=false;
      a1_Nodo_Destellos(1,3);
    }

  //5. RFM95 Funciones.
    //-5.2 Responsde si el mensaje es para él.
      if(responder){
        if(sender==master){
          b1();
          RFM95_enviar(Nodo_info+letras);
        }
        else if(sender==siguiente){
          b2();
          RFM95_enviar(Nodo_info+letras);
        }
        else if(modo_Continuo && flag_temporizador_1){
          flag_temporizador_1=false;
          a5_Nodo_Mensaje_ID();
          b3();
          RFM95_enviar(Nodo_info+letras);
        }
        else if(localAddress){
          RFM95_enviar(letras);
        }
      }
    //-5.2 RFM95 RECIBIR.
      RFM95_recibir(LoRa.parsePacket());
}
//1. Funciones de Logic interna del Micro.
  void welcome(){
    // Deshabilitamos Banderas
      inicio=false;
      Serial.println("Sistema Iniciado");
      Serial.println("Direccion: ");
      Serial.println(localAddress);
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
    funtion_Mode=inputString.substring(0,1);
    funtion_Number=inputString.substring(1,2);
    funtion_Parmeter1=inputString.substring(2,3);
    funtion_Parmeter2=inputString.substring(3,4);
    funtion_Parmeter3=inputString.substring(4,5);


    Serial.println(inputString);         // Pureba de Comunicacion Serial.
    Serial.println("funcion: " + funtion_Mode);
    Serial.println("Numero: " + funtion_Number);
    Serial.println("Parametro1: " + funtion_Parmeter1);
    Serial.println("Parametro2: " + funtion_Parmeter2);
    Serial.println("Parametro3: " + funtion_Parmeter3+ "\n");
  }
//2. Funciones Seleccionadas para Ejecutar.
  //-2.1 Funciones Tipo A.
    void a1_Nodo_Destellos (int repeticiones, int tiempo){
      // FUNCION PROBADA CORRECTAMENTE
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
    void a2(){
      // FUNCION PROBADA CORRECTAMENTE
      letras=inputString.substring(2);
      Serial.println("letras:" + letras);         // Pureba de Comunicacion Serial.
    }
    void a3_Nodo_Direccion_Local(int paramatro_1){
      Serial.println("Ejecutando F3.. \n");
      localAddress = paramatro_1;
      Serial.print("Drireccion Local: ");
      Serial.println(localAddress);
    }
    void a4_Nodo_Direccion_Destino(int direccion_aux){
      Serial.println("Ejecutando F4.. \n");
      destination = direccion_aux;
      Serial.print("Drireccion Destino: ");
      Serial.println(destination);
      
    }
    void a5_Nodo_Mensaje_ID(){      
      msg_ID++;                           // increment message ID.
    }
    void a6_Nodo_Numeros(int parametro_1){
      Nodos=parametro_1;  
    }
    void a7(int tipo_Modo){
      int a=tipo_Modo;
      if(a==1){
        modo_Continuo=true;
        responder=true;
        temporizador_1.attach(1, ISR_temporizador_1);
      }
      if(a==0){
        modo_Continuo=false;
        responder=false;
        temporizador_1.detach();
      }
    }
  //-2.2 Funciones tipo B.
    // Identifico quien Envia el Mensaje Byte
    void b1 (){
      // 1. Destinatario.
      destination=sender;                           // Respondo a quien me escribe.
      // 2. Remitente.
      //localAddress=String(Nodo).toInt();            // Establecer direccion Local.
      // 3. Nodos Leidos 1.
      msg1_Write=incomingMsgId1;
      // 4. Nodos Leidos 2.
      msg2_Write=incomingMsgId2;
      // 5. Longitud de Bytes de la Cadena incoming.
      // Este byte lo escribe antes de Enviar el mensaje.
      // 6. Este byte contiene Informacion del Nodo.
      Nodo_info=String(Alarma_Zona_1+Alarma_Zona_2, HEX);
      // 7. Byte Escrito desde recepcion Serial o Predefinido.
      // 7. Byte Escrito desde recepcion Serial o Predefinido.

    }
    void b2 (){
      // Decodifico quien envia el mensaje.
      // Establecer la proxima direccion del Nodo
      // Si es el Ultimo Nodo Enviar al Maestro.
     // Nodo_cercano=
    }
    void b3 (){
      // Informacion Acerca de los nodos que pude LEER.
      // Si el mensaje viene del Maestro, preparar el mesaje para responder al Maestro
      destination=master;                           // Respondo a quien me escribe.
      // 2. Remitente.
      //localAddress=String(Nodo).toInt();            // Establecer direccion Local.
      // 3. Nodos Leidos 1.
      msg1_Write=incomingMsgId1;
      // 4. Nodos Leidos 2.
      msg2_Write=incomingMsgId2;
      // 5. Longitud de Bytes de la Cadena incoming
        // Este byte lo escribe antes de Enviar el mensaje
      // 6. Este byte contiene Informacion del Nodo
      Nodo_info=String(msg_ID, HEX);
      // 7. Byte Escrito desde recepcion Serial o Predefinido.
      // 7. Byte Escrito desde recepcion Serial o Predefinido.
      
    }
    void b4 (int a1, int a2){
      int aa=a1;
      int aa2=a2;
      bitSet(msg1_Write, sender);
      msg2_Write=0;

    }
    void b5 (int a1, int a2){
      int aa=a1;
      int aa2=a2;
    }
    void b6 (int a1, int a2){
      int aa=a1;
      int aa2=a2;
    }
    void b7 (int a1, int a2){
      int aa=a1;
      int aa2=a2;
    }
    void b8 (int a1, int a2){
      int aa=a1;
      int aa2=a2;
    }
    void b9 (int a1, int a2){
      int aa=a1;
      int aa2=a2;
    }
    void b0 (){
      int aa=1;
    }
//3. Gestiona las funciones a Ejecutar.
  void ejecutar_solicitud(){
    // Deshabilitamos Banderas
      x1=funtion_Parmeter1.toInt();
      x2=funtion_Parmeter2.toInt();
      x3=funtion_Parmeter3.toInt();
    // Function Tipo A
      if (funtion_Mode=="A" && funtion_Number=="1"){
        Serial.println("funion A Nº001");
        a1_Nodo_Destellos(x1,x2);
      }
      else if (funtion_Mode=="A" && funtion_Number=="2"){
        Serial.println("funion A Nº2");
        a2();
      }
      else if (funtion_Mode=="A" && funtion_Number=="3"){
        // FUNCIONO A MEDIAS SOLO DIRECIONES BAJAS Y 255 falta acomodar un poco mas
        Serial.println("funion A Nº3");
        String Nodo_direccion_aux = "";
        Nodo_direccion_aux = funtion_Parmeter1 + funtion_Parmeter2 + funtion_Parmeter3;
        //Serial.println(Nodo_direccion_aux);
        int Nodo_direccion = Nodo_direccion_aux.toInt();
        //Serial.println(Nodo_direccion);
        a3_Nodo_Direccion_Local(Nodo_direccion);
      }
      else if (funtion_Mode=="A" && funtion_Number=="4"){
        Serial.println("funion A Nº4");
        String Nodo_destino_aux = "";
        Nodo_destino_aux = funtion_Parmeter1+funtion_Parmeter2+funtion_Parmeter3;
        int Nodo_destino = Nodo_destino_aux.toInt();
        a4_Nodo_Direccion_Destino(Nodo_destino);
      }
      else if (funtion_Mode=="A" && funtion_Number=="5"){
        Serial.println("funion A Nº5");
      }
      else if (funtion_Mode=="A" && funtion_Number=="6"){
        Serial.println("funion A Nº6: Numero de Nodos");
        String Nodos_numeros_aux = funtion_Parmeter1+funtion_Parmeter2;
        int Nodos_numeros = Nodos_numeros_aux.toInt();
        a6_Nodo_Numeros(Nodos_numeros);
      }
      else if (funtion_Mode=="A" && funtion_Number=="7"){
        Serial.println("funion A Nº7");
        a7(x1);
      }
      else if (funtion_Mode=="A" && funtion_Number=="8"){
        Serial.println("funion A Nº8 Status");
        //1.
        Serial.print("Direccion Local: ");
        Serial.println(localAddress);
        //2.
        Serial.print("Direccion Destino: ");
        Serial.println(destination);
        //3.
        Serial.print("Modo Continuo: ");
        Serial.println(modo_Continuo);
        //4.
        // Serial.print(": ");
        // Serial.println();
        // //5.
        // Serial.print(": ");
        // Serial.println();
        // //6.
        // Serial.print(": ");
        // Serial.println();
      }
      else if (funtion_Mode=="A" && funtion_Number=="9"){
        Serial.println("funion A Nº9");
        responder=true;
        letras=inputString.substring(2);
        // Serial.println(letras);
      }
      else if (funtion_Mode=="A" && funtion_Number=="0"){
        Serial.println("funion A Nº0");
        RFM95_enviar("Maestro");
      }
    // Function Tipo B
      //
      else if (funtion_Mode=="B" && funtion_Number=="1"){
        Serial.println("funion B Nº1: Quien envia?");
        b1();
      }
      else if (funtion_Mode=="B" && funtion_Number=="2"){
        Serial.println("funion B Nº2: Preparo informacion propia");
        b2();
      }
      else if (funtion_Mode=="B" && funtion_Number=="3"){
        Serial.println("funion B Nº3:  info recibida ");
        b3();
      }
      else if (funtion_Mode=="B" && funtion_Number=="4"){
        Serial.println("funion B Nº4");
        b4(1,1);
      }
      else if (funtion_Mode=="B" && funtion_Number=="5"){
        Serial.println("funion B Nº5");
        b5(1,1);
      }
      else if (funtion_Mode=="B" && funtion_Number=="6"){
        Serial.println("funion B Nº6");
        b6(1,1);
      }        
      else if (funtion_Mode=="B" && funtion_Number=="7"){
        Serial.println("funion B Nº7");
        b7(1,1);
      }
      else if (funtion_Mode=="B" && funtion_Number=="8"){
        Serial.println("funion B Nº8");
        b8(1,1);
      }     
      else if (funtion_Mode=="B" && funtion_Number=="9"){
        Serial.println("funion B Nº9");
        b9(1,1);
      }     
      else if (funtion_Mode=="B" && funtion_Number=="0"){
        Serial.println("funion B Nº0");
        b0();
      }                            
      else{
      Serial.println("Ninguna Funcion");
      }
      
  }
//4. Funcion que Revisa estados a ser enviados.
  //-4.1 Estados de Zonas.
    void reviso(){
      if(!digitalRead(Zona_1)){
        Alarma_Zona_1=1;
      }
      if(!digitalRead(Zona_1));{
        Alarma_Zona_2=1;
      }
      if(!digitalRead(Zona_1));{
        Alarma_Zona_3=1;
      }
      if(!digitalRead(Aceptar)){
        Alarma_Zona_1=0;
        Alarma_Zona_2=0;
        Alarma_Zona_3=0;
      }
    }
//5. Funciones de Dispositivos Externos.
  //-5.1 RFM95 RECIBIR.
    void RFM95_recibir(int packetSize){
      if (packetSize == 0) return;          // if there's no packet, returnº1
      // read packet header bytes:
      recipient = LoRa.read();         // recipient address
      sender = LoRa.read();            // sender address
      incomingMsgId1 = LoRa.read();     // incoming msg ID
      incomingMsgId2 = LoRa.read();     // incoming msg ID
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
      Serial.println("Message ID1: " + String(incomingMsgId1));
      Serial.println("Message ID2: " + String(incomingMsgId2));
      Serial.println("Message length: " + String(incomingLength));
      Serial.println("Message: " + incoming);
      Serial.println("RSSI: " + String(LoRa.packetRssi()));
      Serial.println("Snr: " + String(LoRa.packetSnr()));
      Serial.println();

      //Verificamos que el mensaje que nos llega sea para nosotros
      if(recipient==localAddress){
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
      LoRa.write(msg1_Write);               // add message ID
      LoRa.write(msg2_Write);               // add message ID
      LoRa.write(outgoing.length());        // add payload length
      LoRa.print(outgoing);                 // add payload
      LoRa.endPacket();                     // finish packet and send it
      responder=false;
    }    