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
char comanda; // Valoarea in care este stocata comanda primita pe seriala
float temperatureData; // Variabila in care se stocheaza date despre temperatura
float lightData; // Variabila in care se stocheaza date despre lumina
int temt6000Pin = 5; // Pinul la care este conectat senzorul este A0

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
    comanda = Serial.read(); 
    if(comanda == '1') {
        Serial.write((char)temperatureData);
      } else if(comanda == '0') {
        Serial.write((char)lightData);
      }
        
        // Doar de test - ar trebui sters cand toate sunt functionale
        Serial.println("Sunt in task-ul de trimitere al datelor");
      
        Serial.print("TaskProcesareLumina - valoare lumina -  ");
        Serial.print(lightData);
        Serial.println(" ");
        
        Serial.print("TaskProcesareTemperatura - valoare temperatura -  ");
        Serial.print(temperatureData);
        Serial.println(" ");

        vTaskDelay(500 / portTICK_PERIOD_MS ); 
  }
}
void TaskCitireLumina(void *pvParameters)  {
  while(1)
  {
    lightData = analogRead(TEMT6000_PIN);
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
  }
}
void TaskCitireTemperatura(void *pvParameters)  {
  while(1)
  {
    delay(1500);
    int chk = DHT.read11(DHT11_PIN);
    temperatureData = DHT.temperature; // citeste valoarea de temperatura de pe senzor

    
    // temperatureData = DHT11.read(DHT11PIN); // Citire folosind biblioteca dht11.h
    //delay(2000);
    //vTaskDelay(500 / portTICK_PERIOD_MS);    
  }
  
}
