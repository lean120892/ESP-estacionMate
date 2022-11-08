#include <Arduino.h>


unsigned long TiempoInicio = 0; // Variable para el contador por millis()
float temp = 0;                 //Temperatura hambiente DHT11
float hum = 0;                  //Humedad hambiente DHT11



// ----- Sensor DHT 11 ---------
#include "DHT.h"
#define DHTPIN 32     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

void configuracionDHT11(){
  
   //------Inicializar Sensor dht 11 -------
  dht.begin();

  //-------- Inicializar variable de tiempo para millis()

  TiempoInicio = millis();
  }


 void MedTempyHum(){
  unsigned long retardo = 5000;
  //Serial.println("Midiendo temperatura");
  while(( millis() - TiempoInicio) <= retardo ){
     hum = dht.readHumidity();
     temp = dht.readTemperature();
    
    }
 TiempoInicio = millis(); 

 //Serial.print("Temperatura: ");
// Serial.println(temp);
  }
  
