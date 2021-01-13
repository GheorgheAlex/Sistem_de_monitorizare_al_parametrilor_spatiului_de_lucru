// Componenta care citeste si transmite la cerere datele citite de la senzori

// Conexiuni:
// -> Senzor lumina ambientala TEMT6000 - Pinul A5
// -> Senzor temperatura si umiditate DHT11 - Pinul 4

#include <Arduino_FreeRTOS.h>

#include <dht.h>
dht DHT;

#define DHT11_PIN 4
#define TEMT6000_PIN A5

// Bucata semifunctionala pentru temperatura - parte declarativa initiala
//#include <dht11.h>
//#define DHT11PIN 4
//dht11 DHT11;

// Declarare Task-uri
void TaskTrimitereDate( void *pvParameters );
void TaskCitireLumina( void *pvParameters );
void TaskCitireTemperatura( void *pvParameters );

// Globals
char transmitere[20]; // Variabila care trimite datele pe seriala
int temperatureData; // Variabila in care se stocheaza date despre temperatura
int lightData; // Variabila in care se stocheaza date despre lumina


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  
  xTaskCreate(
    TaskTrimitereDate
    ,  "task1"   
    ,  128  
    ,  NULL
    ,  1  
    ,  NULL );
  xTaskCreate(
    TaskCitireLumina
    ,  "task2"
    ,  128  
    ,  NULL
    ,  1  
    ,  NULL );
    xTaskCreate(
    TaskCitireTemperatura
    ,  "task3"
    ,  128  
    ,  NULL
    ,  1  
    ,  NULL );
    
vTaskStartScheduler();
}

void loop()
{
}

void TaskTrimitereDate(void *pvParameters)  {
  while(1)
  {
    sprintf(transmitere,"%d|%d", temperatureData,lightData);
    Serial.write(transmitere);
    Serial.println();    
    vTaskDelay(500 / portTICK_PERIOD_MS ); // 500
  }
}
void TaskCitireLumina(void *pvParameters)  {
  while(1)
  {
    lightData = analogRead(TEMT6000_PIN);
    //Serial.print("Lum - ");
   // Serial.println(lightData);
    vTaskDelay(500 / portTICK_PERIOD_MS); // 1000
  }
}
void TaskCitireTemperatura(void *pvParameters)  {
  while(1)
  {
    delay(1500); // 1500
    int chk = DHT.read11(DHT11_PIN);
    temperatureData = DHT.temperature; // citeste valoarea de temperatura de pe senzor 
    //Serial.print("Temp - ");
   // Serial.println(temperatureData); 
  }
  
}
