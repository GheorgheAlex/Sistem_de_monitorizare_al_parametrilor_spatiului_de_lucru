#include <Arduino_FreeRTOS.h>

// Declarare Task-uri
void TaskDataReceived( void *pvParameters );
void TaskDataInterpretation( void *pvParameters );
void TaskTempConversion( void *pvParameters );
void TaskLightConversion( void *pvParameters );

// Declarare pini conectati
int lightLED = 4; 
int BUZZ = 3;
int tempLED = 7; 
int tempBUZZ = 6;

// Variables
float temp;
float light;

float lightSensorData;
float temperatureSensorData;

// int value;
int i;

char received[20];
char tempChar[5];
char lightChar[3];

String lightString;
String tempString;
String s;

int valueTemp = 0; 
int valueLight = 0;


  
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  xTaskCreate(
    TaskDataReceived
    ,  "RequestData"   
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

     xTaskCreate(
    TaskTempConversion
    ,  "TempConversion"
    ,  128  
    ,  NULL
    ,  1  
    ,  NULL );

    xTaskCreate(
    TaskLightConversion
    ,  "LightConversion"
    ,  128  
    ,  NULL
    ,  1  
    ,  NULL );
    
vTaskStartScheduler();
}

void loop()
{
}

void TaskDataReceived(void *pvParameters)  
{ 
  while(1)
  {
    if(Serial.available() > 0)
    {
      i = 0;
      while(0 < Serial.available())
      {
        received[i] = Serial.read();
        i++;
      }
    }   
    
    vTaskDelay(250 / portTICK_PERIOD_MS ); //500
  }
}

void TaskTempConversion(void *pvParameters)  
{
  while(1)
  {    
    sprintf(tempChar,"%c%c",received[0],received[1]);
    String tempString = String(tempChar);
    valueTemp = tempString.toInt();
    vTaskDelay(400  / portTICK_PERIOD_MS); //1000
  }
  
}

void TaskLightConversion(void *pvParameters)  
{
  while(1)
  {    
    sprintf(lightChar,"%c%c%c",s[3],s[4],s[5]);
    lightString = String(lightChar);
    valueLight = lightString.toInt();
    vTaskDelay(400  / portTICK_PERIOD_MS);  //1000
  }
}

void TaskDataInterpretation(void *pvParameters)  {
  
  while(1)
  { 
    s = String (received);
    Serial.println(s);

    Serial.print("Temp - ");
    Serial.println(valueTemp);

    Serial.print("Light - ");
    Serial.println(valueLight);
       
    if((valueLight == 0) && (valueTemp == 0))
          {
            digitalWrite(lightLED, LOW);
            digitalWrite(tempLED, LOW);
          }else if (valueTemp >= 25) // Daca doar temperatura depaseste pragul, declanseaza alerta de temperatura
            {
              tone(BUZZ, 1000, 500);
              digitalWrite(tempLED, HIGH);
              Serial.println("Alerta Temperatura!");
            } else if (valueLight < 25) // Daca doar lumina depaseste pragul, declanseaza alerta de lumina
              {
                tone(BUZZ, 800, 500);
                digitalWrite(lightLED, HIGH);
                Serial.println("Alerta Lumina!");
              } else if ((valueLight < 25) && (valueTemp >= 25)) // Daca ambele depasesc pragul, declanseaza ambele alerte
                {
                  tone(BUZZ, 1500, 250);
                  digitalWrite(lightLED, HIGH);
                  digitalWrite(tempLED, HIGH);
                  Serial.println("Alerta AMBELE!!!!");
                } else if((valueLight =! 0) && (valueTemp =! 0))
                  {
                    digitalWrite(lightLED, LOW);
                    digitalWrite(tempLED, LOW);
                  } 
    vTaskDelay(400  / portTICK_PERIOD_MS);  //1000   
  }
}
