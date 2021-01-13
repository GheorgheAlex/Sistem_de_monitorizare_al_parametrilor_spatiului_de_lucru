// Sisteme distribuite si de timp real
// Nume proiect - Sistem de monitorizare al parametrilor spațiului de lucru
// Descriere - Codul sursa pentru uC master
// Rol - Procesarea datelor de la uC Slave, trimiterea alertelor -> buzzer si LED-uri
// Student - Gheorghe Alexandru-Marius

// Incercare de a folosi UART in loc de SPI

#include <Arduino_FreeRTOS.h>
#include <semphr.h>  
//#include <SPI.h>

int lightLED = 4; 

int lightBUZZ = 3;
int tempLED = 7; 

int tempBUZZ = 6;

// Declare a mutex Semaphore Handle which we will use to manage the Serial Port.
// It will be used to ensure only only one Task is accessing this resource at any time.
SemaphoreHandle_t xSerialSemaphore;


void TaskDataRequestLight( void *pvParameters ); // Cere date de la senzorul de lumina ambientala 
void TaskDataRequestTemperature( void *pvParameters ); // Cere date de la senzorul de temperatura 
void TaskDataInterpretation( void *pvParameters ); // Interpretarea datelor si alertare



void setup() {

  // Initializare magistrala SPI
   Serial.begin(115200); //set baud rate to 115200 for usart
   pinMode(tempLED, OUTPUT);
   pinMode(lightLED, OUTPUT);
  
  // Asteapta conectarea magistralei serial
  while (!Serial) {
    ; 
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

  // Setare task-uri
  xTaskCreate(
    TaskDataRequestLight
    ,  "DataRequestLight"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskDataRequestTemperature
    ,  "DataRequestTemperature"
    ,  128  // Stack size 
    ,  NULL
    ,  1  // Priority
    ,  NULL );

    xTaskCreate(
    TaskDataInterpretation
    ,  "DataInterpretation"
    ,  128  // Stack size 
    ,  NULL
    ,  1  // Priority
    ,  NULL ); 
    
  // Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.
  // Task Scheduler va relua intreg procesul de programare a rularii task-urilor
}

void loop()
{
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
// Comenzile pe care fiecare task trebuie sa il faca 

// Task-ul de cerere al datelor de la senzorul de lumina
void TaskDataRequestLight( void *pvParameters __attribute__((unused)) )  
{
   char comandaLight = '0';
   float lightData;

  for (;;) {
    if(Serial.available()) {
      Serial.write(comandaLight);
    }

    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      // We were able to obtain or "Take" the semaphore and can now access the shared resource.
      // We want to have the Serial Port for us alone, as it takes some time to print,
      // so we don't want it getting stolen during the middle of a conversion.
      // print out the state of the button:
      lightData = Serial.read();

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }
    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}

// Task-ul de cerere al datelor de la senzorul de temperatura
void TaskDataRequestTemperature( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  char comandaTemperature = '1';
  float temperatureData;
  
  for (;;)
  {
    Serial.write(comandaTemperature);
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      // We were able to obtain or "Take" the semaphore and can now access the shared resource.
      // We want to have the Serial Port for us alone, as it takes some time to print,
      // so we don't want it getting stolen during the middle of a conversion.
      // print out the value you read:
      temperatureData = Serial.read();
      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}

// Task-ul de interpretare al datelor si trimitere al alertelor
void TaskDataInterpretation( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  float temp = temperatureData;
  float light = lightData;
  for (;;){
    // Interpretarea datelor de temperatura
       if (temp < 20){
          tone(tempBUZZ, 1000, 500);
          digitalWrite(tempLED, HIGH);
       }

    // Interpretarea datelor de lumina ambientala
       if (light < 20){
          tone(lightBUZZ, 800, 500);
          digitalWrite(lightLED, HIGH);
       }
    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}
