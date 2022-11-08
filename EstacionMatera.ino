

#include <WiFi.h>
#include <FirebaseESP32.h>

#include "DHT11.h"
#include "RelojRTC.h"

/**** Datos de la Placa ****/
#define ID_PLACA 0001;
#define UBICACION_PLACA "Av.SiempreViva.712";

/**** Datos de Firebase y WiFI ****/
#define FIREBASE_HOST "termosolaresp-default-rtdb.firebaseio.com" //Sin http:// o https:// 
#define FIREBASE_AUTH "aVAbd08k2Bpjrx8kto7zY41bUbBRFEMAJzUtzhyr"
#define WIFI_SSID "TermoSolar"
#define WIFI_PASSWORD ""

/**** Firebase ****/
FirebaseData firebaseData;
String path = "/EstacionMatera/ID_PLACA" ;

/*** Constantes ***/
const byte btn = 13; // Boton para carga de termo
const byte bombaOut = 12; //Salida Bomba Agua

/**** Variables ****/
bool flag = true;
bool flagBtn = false;
bool flagWiFi = false;
String dato = "";
int contador = 0;
int retardo =0;

/*** Interrupcion por boton ***/
void isr(){
  flagBtn = true;
  }

/***Configuracion de interrupcion  (timer)***/
volatile int interruptCounter;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
 
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  Serial.println(interruptCounter);
  portEXIT_CRITICAL_ISR(&timerMux);
 
}

void setup() {
    /*** Configuracion GPIO ***/
  pinMode(btn, INPUT_PULLUP);
  attachInterrupt(btn, isr,LOW); // Interrupcion boton de carga de agua
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
              }else{
                Serial.println ("No se pudo conectar a la red Wi-Fi");
                }
   timer = timerBegin(0, 800, false);
   timerAttachInterrupt(timer, &onTimer, false);
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
/*
if(interruptCounter==20){
    timer = timerBegin(0, 800, false);
   timerAttachInterrupt(timer, &onTimer, false);
   //Serial.println("se freno el contador");
  }
*/
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
if(flagBtn == true){
        while (digitalRead(btn) == LOW){
       digitalWrite( bombaOut, HIGH);
        }

        digitalWrite(bombaOut, LOW);
  }


  flagBtn = false;
 

}
