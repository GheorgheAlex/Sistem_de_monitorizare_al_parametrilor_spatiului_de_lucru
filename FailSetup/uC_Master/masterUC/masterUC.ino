#include <Arduino_FreeRTOS.h>

// Declarare Task-uri
void TaskDataRequestLightSensor( void *pvParameters );
void TaskDataRequestTemperatureSensor( void *pvParameters );
void TaskDataInterpretation( void *pvParameters );

// Declarare pini conectati
int lightLED = 4; 
int lightBUZZ = 3;
int tempLED = 7; 
int tempBUZZ = 6;

// Variables
float temp;
float light;
float lightSensorData;
float temperatureSensorData;

char comandaTemperature = '1';
char comandaLight = '0';

  
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  xTaskCreate(
    TaskDataRequestLightSensor
    ,  "RequestLightSensor"   
    ,  128  
    ,  NULL
    ,  1  
    ,  NULL );
  xTaskCreate(
    TaskDataRequestTemperatureSensor
    ,  "RequestTemperatureSensor"
    ,  128  
    ,  NULL
    ,  1  
    ,  NULL );
    xTaskCreate(
    TaskDataInterpretation
    ,  "DataInterpretation"
    ,  128  
    ,  NULL
    ,  1  
    ,  NULL );
    
vTaskStartScheduler();
}

void loop()
{
}

void TaskDataRequestLightSensor(void *pvParameters)  {
  
  while(1)
  {
    
    Serial.write(comandaLight);
    
    lightSensorData = Serial.read();
    Serial.print("valoare lumina -  ");
    Serial.println(lightSensorData);
    
    vTaskDelay(1000 / portTICK_PERIOD_MS ); 
  }
}
void TaskDataRequestTemperatureSensor(void *pvParameters)  {

  while(1)
  {
     
    Serial.write(comandaTemperature);
    
    temperatureSensorData = Serial.read();
    Serial.print("valoare temperatura -  ");
    Serial.println(temperatureSensorData);
    
    vTaskDelay(1500 / portTICK_PERIOD_MS); 
  }
}
void TaskDataInterpretation(void *pvParameters)  {
  
  
  while(1)
  {
    // Interpretare datelor de temperatura
    if (temperatureSensorData < 20)
    {
      tone(tempBUZZ, 1000, 500);
      digitalWrite(tempLED, HIGH);
      //Serial.println("Alerta Temperatura!");
    }

    // Interpretarea datelor de lumina ambientala
    if (lightSensorData < 20)
    {
      tone(lightBUZZ, 800, 500);
      digitalWrite(lightLED, HIGH);
      //Serial.println("Alerta Lumina!");
    }
    
    vTaskDelay(1000  / portTICK_PERIOD_MS);    
  }
  
}
