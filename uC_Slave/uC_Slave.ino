// Sisteme distribuite si de timp real
// Nume proiect - Sistem de monitorizare al parametrilor spațiului de lucru
// Cod pentru microcontroller-ul slave 
// Rol - citirea datelor de la senzori si transmiterea datelor pe magistrala SPI.
//
//

#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).
#include <SPI.h>

// Declare a mutex Semaphore Handle which we will use to manage the Serial Port.
// It will be used to ensure only only one Task is accessing this resource at any time.
SemaphoreHandle_t xSerialSemaphore;

// define two Tasks for DigitalRead & AnalogRead
void sensorRead( void *pvParameters ); // Citire parametrii de pe senzori
void SPIsend( void *pvParameters ); // Transmitere date pe magistrala SPI

// the setup function runs once when you press reset or power the board
void setup() {

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
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
    TaskSPIsend
    ,  "SPIsend"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TasksensorRead
    ,  "sensorRead"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );

  // Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.
}

void loop()
{
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TasksensorRead( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  /*
    SensorRead
    Citeste datele furnizate de senzori si le memoreaza
  */
  uint8_t sensorTemp_pin = 3;
  uint8_t sensorLight_pin = 4;

  pinMode(sensorTemp_pin, INPUT);
  pinMode(sensorLightpin, INPUT);

  for (;;) // A Task shall never return or exit.
  {
    // Citire date de pe intrari
    int sensorTemp_in = digitalRead(sensorTemp_pin);
    int sensorLight_in = analogRead(sensorLight_pin); // desi cred ca va fi tot citire digitala


    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}

void TaskSPIsend( void *pvParameters __attribute__((unused)) )  // This is a Task.
{

  for (;;)
  {
    // Transmitere date pe magistrala SPI

    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}
