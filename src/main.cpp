#include <Arduino.h>
#include "Wire.h"
#include "HTTPClient.h"
#include "WiFiClient.h"
// #include "BluetoothSerial.h"
// #include "uRTCLib.h"

// uRTCLib rtc(0x68);

// BluetoothSerial serialBT;
#define SSID_HOME "S_Hosainy"
#define PASS_HOME "8280@5745"
#define URL "http://worldtimeapi.org/api/ip"

// void tHTTP(void *arg)
// {
//   uint8_t TWBuffer[100];
//   Wire.begin(0b1001001, SDA, SCL, 100000);
//   while (1)
//   {
//     while (!(Wire.available()))
//       vTaskDelay(1);
//     ;
//     Wire.readBytesUntil('&', TWBuffer, sizeof(TWBuffer));
//     for (uint8_t i = 0; i < 100; i++)
//     {
//       if (TWBuffer[i] == '&')
//         break; // vTaskDelete(tHTTP);
//       else
//         Serial.print(TWBuffer[i]);
//     }
//   }
//   // size + http... + &
//   String URL;
//   for (uint8_t i = 0; i < TWBuffer[0]; i++)
//   {
//     Serial.println(TWBuffer[i]);
//   }
//   WiFi.begin(SSID_HOME, PASS_HOME);
//   Serial.println("Connecting");
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     vTaskDelay(500);
//     Serial.print(".");
//   }
//   Serial.println("");
//   Serial.print("Connected to WiFi network with IP Address: ");
//   Serial.println(WiFi.localIP());

//   HTTPClient http;
//   uint32_t lastTime = millis();
//   uint32_t timerDelay = 1000;
//   while (1)
//   {
//     // http.begin(URL);
//     if ((millis() - lastTime) > timerDelay)
//     {
//       // Check WiFi connection status
//       if (WiFi.status() == WL_CONNECTED)
//       {
//         HTTPClient http;

//         // Your Domain name with URL path or IP address with path
//         http.begin(URL);

//         // Send HTTP GET request
//         int httpResponseCode = http.GET();

//         if (httpResponseCode > 0)
//         {
//           Serial.print("HTTP Response code: ");
//           Serial.println(httpResponseCode);
//           String payload = http.getString();
//           Serial.println(payload);
//         }
//         else
//         {
//           Serial.print("Error code: ");
//           Serial.println(httpResponseCode);
//         }
//         // Free resources
//         http.end();
//       }
//       else
//       {
//         Serial.println("WiFi Disconnected");
//       }
//       lastTime = millis();
//     }
//   }
// }
// void tTWI(void *arg)
// {
//   Wire.begin(0b1001001, SDA, SCL, 100000);
//   char *arr = NULL;
//   arr = (char *)malloc(100 * sizeof(char));
//   while (1)
//   {
//     if (Wire.available())
//     {
//       Wire.readBytes(arr, 10);
//       Serial.printf(arr);
//       vTaskDelay(10);
//     }
//   }
// }
void I2C_receiveCallback(int num)
{
  while (1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    switch (c)
    {
    case 'A':
      break;

    default:
      break;
    }

    Serial.print(c); // print the character
  }
  // int x = Wire.read();     // receive byte as an integer
  // Serial.println(x); // print the integer
}
String payload;
void I2C_requestCallback()
{
  if (payload.length())
    Wire.slaveWrite((uint8_t *)payload.c_str(), payload.length());
  else
    Wire.slaveWrite((uint8_t *)"no data", 8);
}
void setup()
{
  Serial.begin(115200);
  delay(10000);
  WiFi.begin(SSID_HOME, PASS_HOME);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Wire.begin(0b1001001);

  Wire.onReceive(I2C_receiveCallback);
  Wire.onRequest(I2C_requestCallback);
  // Wire.begin();
  // i2cInit(0, SDA, SCL, 100000);
  // while (serialBT.connected())
  //   ;
  // xTaskCreate(tTWI, "TWI task", 0x800, NULL, 10, NULL);
  // uint8_t TWBuffer[100];
  // while (1)
  // {
  //   while (!(Wire.available()))
  //   {
  //     Serial.println("nope");
  //     vTaskDelay(10);
  //   }
  //   Wire.readBytesUntil('&', TWBuffer, sizeof(TWBuffer));
  //   for (uint8_t i = 0; i < 100; i++)
  //   {
  //     if (TWBuffer[i] == '&')
  //       break; // vTaskDelete(tHTTP);
  //     else
  //       Serial.print(TWBuffer[i]);
  //   }
  // }
  // size + http... + &
  // String URL;
}
char arr[100];
HTTPClient http;

void loop()
{
  uint32_t lastTime = millis();
  uint32_t timerDelay = 1000;
  // http.begin(URL);
  if ((millis() - lastTime) > timerDelay)
  {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {
      // Your Domain name with URL path or IP address with path
      http.begin(URL);

      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0)
      {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
      }
      else
      {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  delay(1000);
}
