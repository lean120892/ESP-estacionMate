#include "RTClib.h"
RTC_DS1307 rtc;

int   mes = 0;                  // Variables RTC
 int   dia = 0;
 int   hora = 0;
  int   minuto = 0;
  


  void ObtenerTiempoyFecha(){
  
   DateTime now = rtc.now();

    mes = now.month();
   // Serial.print(now.month(), DEC);
    //Serial.print('/');
    dia = now.day();
   // Serial.print(now.day(), DEC);
   // Serial.print(" (");
    hora = now.hour();
   // Serial.print(now.hour(), DEC);
   // Serial.print(':');
    minuto = now.minute();
    //Serial.print(now.minute(), DEC);
   // Serial.print(':');
  
  
  }

int ObtenerMinutos(){
     
    DateTime now = rtc.now();
    
    minuto = now.minute();
    return minuto;
   
    }


void ConfigRTC(){
   // ------- Inicializar Reloj ---------
    if (! rtc.begin()) {
    Serial.println(" RTC No encontrado");
    Serial.flush();
     while (1) delay(10);

  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
 
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  DateTime now = rtc.now();
  
   ObtenerTiempoyFecha();


  }

  int CalcularRetardo (){
     DateTime now = rtc.now();
     DateTime future (now + TimeSpan(0,0,10,0));
     Serial.println("MINUTO");
     Serial.println(now.minute());
     Serial.println("RETARDOOO");
     Serial.println(future.minute(),DEC);
     return future.minute();
    
    }
