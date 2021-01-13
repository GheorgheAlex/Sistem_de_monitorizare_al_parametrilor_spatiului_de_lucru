// Sisteme distribuite si de timp real
// Nume proiect - Sistem de monitorizare al parametrilor spațiului de lucru
// Descriere - Codul sursa pentru uC slave 
// Rol - Citirea datelor de la senzori si transmiterea datelor pe magistrala SPI la cererea uC master
// Student - Gheorghe Alexandru-Marius

#include <Arduino_FreeRTOS.h>
#include <semphr.h> 
#include <DHT.h>

// Parametrii de configurare pentru senzorul de temperatura DHT11
#define DHTTYPE DHT11 // defineste tipul senzorului din libraria DHT.h
#define DHTPIN 3 // defineste pinul la care este conectat senzorul

DHT dht(DHTPIN, DHTTYPE); // initializeaza senzorul cu parametrii dati


// Declare a mutex Semaphore Handle which we will use to manage the Serial Port.
// It will be used to ensure only only one Task is accessing this resource at any time.
SemaphoreHandle_t xSerialSemaphore;


void TaskDataTransmission( void *pvParameters ); // Transmitere date de la senzori la cerere
void TaskProcesareLumina( void *pvParameters ); // Citire valori senzor de lumina ambientala
void TaskProcesareTemperatura( void *pvParameters ); // Citire valori senzor de temperatura

// Globals
char comanda; // Valoarea in care este stocata comanda primita pe seriala
uint8_t temperatureData; // Variabila in care se stocheaza date despre temperatura
uint8_t lightData; // Variabila in care se stocheaza date despre lumina
int temt6000Pin = 0; // Pinul la care este conectat senzorul este A0

void setup() {

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  dht.begin();
  
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }

  // Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
  // because it is sharing a resource, such as the Serial port.
  // Semaphores should only be used whilst the scheduler is running, but we can set it up here.
  if ( xSerialSemaphore == NULL )  // Check to confirm that the Serial Semaphore has not already been created.
  {
    xSerialSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  }

  // Now set up two Tasks to run independently.
  xTaskCreate(
    TaskDataTransmission
    ,  "DataTransmission"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
  xTaskCreate(
    TaskProcesareLumina
    ,  "ProcesareLumina"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );
  xTaskCreate(
    TaskProcesareTemperatura
    ,  "ProcesareTemperatura"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );
  // Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.
}

void loop() {}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskDataTransmission( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
      Serial.println("Sunt in task-ul de trimitere al datelor");
  for (;;)
  {
    if(Serial.available()) {
      comanda = Serial.read();   // citeste un byte din buffer si il memoreaza
    }    

    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
      if(comanda == '1') {
        Serial.write(temperatureData);
      } else if(comanda == '0') {
          Serial.write(lightData);
        }
        Serial.println("Sunt in task-ul de trimitere al datelor");
      
      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}

void TaskProcesareLumina( void *pvParameters __attribute__((unused)) )  // Procesare date lumina si transmisiea pe UART
{
  
  for (;;)
  {
          
      Serial.print("TaskProcesareLumina - valoare lumina -  ");
      Serial.print(lightData);
      Serial.println(" ");
    lightData = analogRead(temt6000Pin);

    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}

void TaskProcesareTemperatura( void *pvParameters __attribute__((unused)) )  // Procesare date temperatura si transmisiea pe UART
{

  for (;;)
  {
    //delay(2000);
    temperatureData = dht.readTemperature(); // citeste valoarea de temperatura de pe senzor
  }
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      Serial.print("TaskProcesareTemperatura - valoare temperatura -  ");
      Serial.print(temperatureData);
      Serial.println(" ");
      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }
    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
