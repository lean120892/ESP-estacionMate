
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "ThingsBoard.h"

#include "DHT11.h"
#include "RelojRTC.h"

// to understand how to obtain an access token
#define TOKEN               "IgiDFYXb6dSbbgZtTB4Y"
#define THINGSBOARD_SERVER  "thingsboard.cloud"
// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);

/**** Datos de la Placa ****/
#define ID_PLACA 0001;
#define UBICACION_PLACA "Av.SiempreViva.712";

/**** Datos de Firebase y WiFI ****/
#define FIREBASE_HOST "termosolaresp-default-rtdb.firebaseio.com" //Sin http:// o https:// 
#define FIREBASE_AUTH "aVAbd08k2Bpjrx8kto7zY41bUbBRFEMAJzUtzhyr"
#define WIFI_SSID "Casa"
#define WIFI_PASSWORD "algaleem64659296"

/**** Firebase ****/
FirebaseData firebaseData;
String path = "/EstacionMatera/ID_PLACA" ;

/*** Constantes ***/
const byte btn = 27; // Boton para carga de termo
const byte bombaOut = 12; //Salida Bomba Agua

/**** Variables ****/
bool flag = true;
bool flagBtn_enable = false;
bool flagWiFi = false;
bool flagBtn_presionado = false;
bool flagCargaTermos = false;
unsigned char tiempoDeCarga =0;

String dato = "";
int contador = 0;
int retardo =0;
int cantidadDeTermos=0;

volatile int interruptCounter;

/*** Funcion de interrupcion pora boton carga de agua ***/

 void IRAM_ATTR isr_btn_agua(){
if(flagBtn_enable){
  
  if(flagBtn_presionado){
    flagBtn_presionado = false;
    Serial.println("SE SOLTO");

   Serial.println(interruptCounter);
   tiempoDeCarga = interruptCounter;
   flagCargaTermos = true;
    }else{
      flagBtn_presionado = true;
      interruptCounter = 0;
        Serial.println("SE PRESIONO");
        
    }
}

  }

/***Configuracion de interrupcion  (timer)***/

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
 
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  //Serial.println(interruptCounter);
  portEXIT_CRITICAL_ISR(&timerMux);
 
}

void setup() {
    /*** Configuracion GPIO ***/
  pinMode(btn, INPUT_PULLUP);
  attachInterrupt(btn, isr_btn_agua,CHANGE); // Interrupcion boton de carga de agua
  pinMode(bombaOut, OUTPUT);
  Serial.begin(115200);

   /***Inicializar las interrupciones ***/

  timer = timerBegin(0, 800, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100000, true);
  timerAlarmEnable(timer);
  
 /*** Inicializar Wi-Fi  ***/

  if(flagWiFi == false){
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      Serial.print("Conectando a ....");
      interruptCounter =0;
        while (WiFi.status() != WL_CONNECTED && interruptCounter < 50)
            {
              Serial.print(".");
              delay(300);
            }
            if(WiFi.status() == WL_CONNECTED){
              flagWiFi = true;
                Serial.println();
                Serial.print("Conectado con la IP: ");
                Serial.println(WiFi.localIP());
                Serial.println();
                   flagBtn_enable =true;
              }else{
                Serial.println ("No se pudo conectar a la red Wi-Fi");
                }
  // timer = timerBegin(0, 800, false);
   //timerAttachInterrupt(timer, &onTimer, false);

   flagBtn_enable =true;
    }



/*** Configuracion de Firebase ***/
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Establezca el tiempo de espera de lectura de la base de datos en 1 minuto (máximo 15 minutos)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  
  //Tamaño y  tiempo de espera de escritura, tiny (1s), small (10s), medium (30s) and large (60s).
  //tiny, small, medium, large and unlimited.
  dato += contador;
  Firebase.setwriteSizeLimit(firebaseData, "tiny");


  /*** Configuracion de sensores ***/
  configuracionDHT11(); // Inicializar sensor DHT11
  ConfigRTC();

}

void loop() {

  if (flag == true){
    MedTempyHum();
    ObtenerTiempoyFecha();
    
    Firebase.setInt(firebaseData, path + "/Mediciones/" + dato + "/Fecha/mes", mes);
     Firebase.setInt(firebaseData, path + "/Mediciones/" + dato + "/Fecha/dia", dia);
      Firebase.setInt(firebaseData, path + "/Mediciones/" + dato + "/Fecha/hora", hora);
       Firebase.setInt(firebaseData, path + "/Mediciones/" + dato + "/Fecha/minuto", minuto);
        Firebase.setFloat(firebaseData, path + "/Mediciones/" + dato + "/Temperatura", temp);
         Firebase.setFloat(firebaseData, path + "/Mediciones/" + dato + "/Humedad", hum);
         retardo = CalcularRetardo();
         flag = false;
  
    
    }
int nuevosMinutos = ObtenerMinutos();
    if(nuevosMinutos == retardo){
      contador++;
      if(contador == 100){
        contador =0;
        }
        dato = "";
        dato += contador;
        flag = true;

        
      }

 

        while (digitalRead(btn) == LOW){
       digitalWrite( bombaOut, HIGH);
        }

        digitalWrite(bombaOut, LOW);
        if(   tiempoDeCarga >10){
          Serial.println("Subiendo Datos");
          
          tiempoDeCarga =0;
          flagCargaTermos = false;
          cantidadDeTermos++;
          Firebase.setInt(firebaseData, path + "/Cargas/",cantidadDeTermos );
          }

  if (!tb.connected()) {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }
  MedTempyHum();
    tb.sendTelemetryInt("temperature", temp);
  tb.sendTelemetryFloat("humidity", hum);

  tb.loop();

          
  }
