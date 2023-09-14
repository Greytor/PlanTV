// SENSOR DE HUMEDAD DEL SUELO
#define AOUT 7 // Pin analógico "A0" para conectar la salida del sensor de humedad capacitivo
const int Valor_Sensor_Aire = 630; // Valor calculado con el programa de calibración con el sensor al aire
const int Valor_Sensor_Agua = 345; // Valor calculado con el programa de calibración con el sensor sumergido en agua
int valor_sensor = 0; // Variable que almacena el valor de salida del sensor de humedad capacitivo
int porcentaje = 0; // Variable que almacena el porcentaje de humedad relativa del terreno

// SENSOR DE TEMPERATURA DEL SUELO
#include <OneWire.h>
#include <DallasTemperature.h>
const byte pinData = 6;
OneWire outWire(pinData);
DallasTemperature sensor(&outWire);

// SENSOR BAROMETRICO Y DE TEMPERATURA AMBIENTAL
#include <SFE_BMP180.h>
#include <Wire.h>
SFE_BMP180 pressure;
#define ALTITUDE 4.0

// SENSOR DE LUMINOSIDAD
int LDR_pin1 = 6;  // Leeremos el pin 6
int LDR_val1 = 0;  // Variable para leer datos del LDR

// SENSOR DE HUMEDAD AMBIENTAL
#include "DHT.h"
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE); 


// LORA
#include <SPI.h>
#include <LoRa.h>

void setup() {
  //Serial.begin(9600);
  LoRa.begin(915E6);
  //if (!LoRa.begin(915E6)) {
    //Serial.println("Starting LoRa failed!");
    //while (1);
  //}
// SENSOR DE TEMPERATURA DEL SUELO
  sensor.begin();
// SENSOR BAROMETRICO Y DE TEMPERATURA AMBIENTAL
  pressure.begin();

// SENSOR DE HUMEDAD DEL SUELO
  dht.begin();
}

void loop() {
  // ENTRADA - SENSOR DE HUMEDAD DEL SUELO
  // Leemos el valor de la salida analógica del sensor capacitivo, conectada al pin analógico "A0"
  valor_sensor = analogRead(AOUT);
  // Se calcula el porcentaje de humedad relativa teniendo en cuenta los dos límites
  porcentaje = map(valor_sensor, Valor_Sensor_Agua, Valor_Sensor_Aire, 100, 0);
  if(porcentaje < 0) porcentaje = 0; // Evita porcentajes negativos en la medida del sensor
  if(porcentaje > 100) porcentaje = 100; // Evita porcentajes negativos en la medida del sensor
  // Se presentan los mensajes, a través del monitor serie, dependiendo del porcentaje de humedad relativa

  // ENTRADA - SENSOR DE TEMPERATURA DEL SUELO
  sensor.requestTemperatures();

  // ENTRADA - SENSOR DE HUMEDAD DEL AMBIENTE
  float h = dht.readHumidity();
  float t = dht.readTemperature();


  // LORA 
  LoRa.beginPacket();
  // ENTRADA - SENSOR DE LUMINOSIDAD
  LDR_val1 = analogRead(LDR_pin1);
  // ENTRADA - SENSOR BAROMETRICO Y DE TEMPERATURA
  char status;
  double T,P,p0,a;
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
     if (status != 0)
    {
      // Print out the measurement:

      LoRa.print(T,2);

  // SALIDA - SENSOR DE TEMPERATURA DEL SUELO
  LoRa.print(";");
  LoRa.print(sensor.getTempCByIndex(0));
  status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);
        status = pressure.getPressure(P,T);
        {
          // Print out the measurement:
          LoRa.print(";");
          LoRa.print(P,2);
          
        }
      }
    }
  }
  
  // SALIDA - SENSOR DE HUMEDAD DEL SUELO
  LoRa.print(";");
  LoRa.print(porcentaje);
  
  // SALIDA - SENSOR DE HUMEDAD DEL AMBIENTE
  LoRa.print(";");
  LoRa.print(h);

  // SALIDA - SENSOR DE LUMINOSIDAD
   LoRa.print(";");
   LoRa.print(map(LDR_val1,1024,0,0,100));
  

  





  LoRa.endPacket();
  Serial.println("Paquete enviado");
  delay(10000);
}
