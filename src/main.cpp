#include <Arduino.h>
#include "Wire.h"
#include "HTTPClient.h"
#include "WiFi.h"
// #include "WiFiClient.h"
// #include "BluetoothSerial.h"
// #include "uRTCLib.h"

// // uRTCLib rtc(0x68);

// // BluetoothSerial serialBT;
#define SSID "S_Hosainy"
#define PASS "8280@5745"
// #define SSID "Tomahawk"
// #define PASS "12345678Mm"

#define TIME_URL_C "http://worldtimeapi.org/api/ip"
#define LOCATION_URL_C "http://ip-api.com/json"
String payload1;
String payload2;
String payload_dummy1 = "{\"abbreviation\":\"+0330\",\"client_ip\":\"217.218.49.158\",\"datetime\":\"2023-12-22T22:06:09.504615+03:30\",\"day_of_week\":5,\"day_of_year\":356,\"dst\":false,\"dst_from\":null,\"dst_offset\":0,\"dst_until\":null,\"raw_offset\":12600,\"timezone\":\"Asia/Tehran\",\"unixtime\":1703270169,\"utc_datetime\":\"2023-12-22T18:36:09.504615+00:00\",\"utc_offset\":\"+03:30\",\"week_number\":51}";
String payload_dummy2 = "{\"status\":\"success\",\"country\":\"Iran\",\"countryCode\":\"IR\",\"region\":\"23\",\"regionName\":\"Tehran\",\"city\":\"Tehran\",\"zip\":\"\",\"lat\":35.7425,\"lon\":51.5023,\"timezone\":\"Asia/Tehran\",\"isp\":\"Iran University of Science \u0026 technology\",\"org\":\"University of Science \u0026 Technology\",\"as\":\"AS41620 Iran University Of Science and Technology\",\"query\":\"194.225.233.84\"}";
bool updateActive = true;
byte command;
// uRTCLib RTC;
#define COMMAND_BT 0xB0
#define COMMAND_WIFI 0xA0
void redirectUart_task(void *arg)
{
  // Serial2.begin(38400, SERIAL_8E1, 16, 17);
  // char c;
  while (1)
  {
    if (Serial2.available())
      Serial.write(Serial2.read());
    // else
    //   vTaskDelay(10);
  }
}
void httpUpdate_task(void *arg)
{
  // WiFiClient Wifi;
  // Wifi.connect()
  Serial.printf("1");
  WiFi.begin(SSID, PASS);
  Serial.printf("1");
  while (WiFi.isConnected() != WL_CONNECTED)
  {
    Serial.printf(".");
    Serial.println(WiFi.status());
    vTaskDelay(1000);
    if (WiFi.status() == WL_CONNECTED)
      break;
  }
  Serial.println("");
  Serial.print("Connected. IP Address: ");
  Serial.println(WiFi.localIP());

  // http.begin(URL);
  uint32_t now = millis();
  while (1)
  {
    if (millis() - now > 3000 && updateActive)
    {
      if (WiFi.status() != WL_CONNECTED)
      {
        printf("not connected\n\r");
      }
      else
      {
        HTTPClient http;
        String URL = TIME_URL_C;
        http.begin(URL);
        int httpCode = http.GET();

        if (httpCode = HTTP_CODE_OK)
        {
          // payload1 = http.getString().substring(76, 76 + 8);
          payload1 = http.getString();
          Serial.print("ESP:");
          Serial.println(payload1.length());
          // Serial2.write(payload_dummy.c_str(), payload_dummy.length());
        }
        else
          Serial.printf("http error : %d\n", httpCode);
        http.end();
        URL = LOCATION_URL_C;
        http.begin(URL);
        httpCode = http.GET();

        if (httpCode = HTTP_CODE_OK)
        {
          // payload2 = http.getString().substring(100, 100 + 20);
          payload2 = http.getString();
          Serial.print("ESP:");
          Serial.println(payload2.length());
          // Serial2.write(payload_dummy.c_str(), payload_dummy.length());
        }
        else
          Serial.printf("http error : %d\n", httpCode);
        now = millis();
      }
    }
    vTaskDelay(1000);
  }
}
void I2C_receiveCallback(int num)
{
  while (Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.printf("ESP: got %x\n", c);
    // Serial.println(c);
    switch (c)
    {
    case 0xA0:
      command = COMMAND_WIFI;
      break;
    case 0xA1:
      command = 0xA1;
      break;
    case 0xA2:
      command = 0xA2;
      break;
    case 0xB0:
      command = COMMAND_BT;
      break;

    default:
      break;
    }

    // Serial.print(c); // print the character
  }
}
void I2C_requestCallback()
{
  // if (payload.length() > 2)
  // {
  byte high;
  byte low;
  byte Byte1;
  switch (command)
  {
  case 0xA1:
    // high = payload_dummy1.length() >> 8;
    // low = payload_dummy1.length();
    high = (payload1.length() & 0xF0) >> 8;
    low = payload1.length() & 0xF;
    Wire.slaveWrite(&high, 1);
    Wire.slaveWrite(&low, 1);
    Wire.slaveWrite((uint8_t *)payload1.c_str(), payload1.length());
    // Wire.slaveWrite((uint8_t *)payload_dummy1.c_str(), payload_dummy1.length());
    break;
  case 0xA2:

    // high = payload_dummy2.length() >> 8;
    // low = payload_dummy2.length();
    high = (payload2.length() & 0xF0) >> 8;
    low = payload2.length() & 0xF;
    Wire.slaveWrite(&high, 1);
    Wire.slaveWrite(&low, 1);
    Wire.slaveWrite((uint8_t *)payload2.c_str(), payload2.length());
    // Wire.slaveWrite((uint8_t *)payload_dummy2.c_str(), payload_dummy2.length());
    break;
  case 0xA0:
    Byte1 = WiFi.status();
    Wire.slaveWrite(&Byte1, 1);
    break;
  default:
    break;
  }
}
u32_t now;
void setup()
{
  Serial.begin(115200);
  // Serial2.begin(38400, SERIAL_8E1, 16, 17);
  now = millis();

  // xTaskCreate(redirectUart_task, "Redirect_Uart", 0x800, NULL, 10, NULL);
  // delay(5000);
  xTaskCreate(httpUpdate_task, "httpUpdate", 0x2000, NULL, 11, NULL);
  // WiFi.begin(SSID_HOME, PASS_HOME);
  // Serial.println("Connecting");
  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   vTaskDelay(500);
  //   Serial.print(".");
  // }
  // Serial.println("");
  // Serial.print("Connected to WiFi network with IP Address: ");
  // Serial.println(WiFi.localIP());

  // 0b1001001
  if (Wire.begin(0b1001001))
    printf("done\n");

  Wire.onReceive(I2C_receiveCallback);
  Wire.onRequest(I2C_requestCallback);
}
void loop()
{
  delay(1);
}
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
// RTC.set(2,1,1,1,1,1,1);
// void uRTCLib::set(const uint8_t second, const uint8_t minute, const uint8_t hour, const uint8_t dayOfWeek, const uint8_t dayOfMonth, const uint8_t month, const uint8_t year) {
// }
// // char arr[100];
// void loop()
// {

//   // RTC.refresh();
//   // printf("%d\n", RTC.second());

//   char c;
//   // while (1)
//   // {
//   // if (Serial2.available())
//   //   Serial.print(Serial2.readString());
//   // else
//   //   vTaskDelay(10);
//   // if (Serial.available())
//   // Serial2.write(Serial.read());
//   if (now - millis() > 10000)
//   {
//     Serial2.write("HELLO ATMEGA&\n\r");
//     now = millis();
//   }
//   if (Serial2.available())
//     Serial.write(Serial2.read());
//   // Serial.write("HELLO ATMEGA\n\r", 15);
//   // Serial2.print("HELLO ATMEGA&");
//   vTaskDelay(1);
//   // Serial2.write("ESP32&",7);
//   // Serial.write("ESP32\n\r", 8);
//   // }
//   // vTaskDelay(1000);
// }
// // void loop()
// // {
// //   uint32_t lastTime = millis();
// //   uint32_t timerDelay = 1000;
// //   // http.begin(URL);
// //   if ((millis() - lastTime) > timerDelay)
// //   {
// //     // Check WiFi connection status
// //     if (WiFi.status() == WL_CONNECTED)
// //     {
// //       // Your Domain name with URL path or IP address with path
// //       http.begin(URL);

// //       // Send HTTP GET request
// //       int httpResponseCode = http.GET();

// //       if (httpResponseCode > 0)
// //       {
// //         Serial.print("HTTP Response code: ");
// //         Serial.println(httpResponseCode);
// //         payload = http.getString();
// //       }
// //       else
// //       {
// //         Serial.print("Error code: ");
// //         Serial.println(httpResponseCode);
// //       }
// //       // Free resources
// //       http.end();
// //     }
// //     else
// //     {
// //       Serial.println("WiFi Disconnected");
// //     }
// //     lastTime = millis();
// //   }
// //   delay(1000);
// // }
// #include "Arduino.h"
// #include <WiFi.h>
// #include <HTTPClient.h>

// const char *ssid = "S_Hosainy";
// const char *password = "8280@5745";
// void alive_task(void *arg)
// {
//   while (1)
//   {
//     printf("alive\n");
//     vTaskDelay(1000);
//   }
// }
// void redirectUart_task(void *arg)
// {
//   Serial2.begin(38400);
//   char c;
//   while (1)
//   {
//     if (Serial2.available())
//       //   c = Serial2.read();
//       // if (c == '\n')
//       //   Serial.println();
//       // else
//       Serial.print(Serial2.read());
//     else
//       vTaskDelay(10);
//     // vTaskDelay(10);
//   }
// }
// void setup()
// {

//   Serial.begin(115200);
//   xTaskCreate(alive_task, "kkk", 0x800, NULL, 10, NULL);
//   xTaskCreate(redirectUart_task, "UART", 0x800, NULL, 10, NULL);
//   printf("hello");
//   delay(4000);
//   printf("hello");
//   WiFi.begin(ssid, password);
//   printf("hello");

//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(1000);
//     Serial.println("Connecting to WiFi..");
//   }

//   Serial.println("Connected to the WiFi network");
// }

// void loop()
// {

//   if ((WiFi.status() == WL_CONNECTED))
//   { // Check the current connection status

//     HTTPClient http;

//     http.begin("http://worldtimeapi.org/api/ip"); // Specify the URL
//     int httpCode = http.GET();                    // Make the request

//     if (httpCode > 0)
//     { // Check for the returning code

//       String payload = http.getString();
//       Serial.println(httpCode);
//       Serial.println(payload);
//     }

//     else
//     {
//       Serial.println("Error on HTTP request");
//     }

//     http.end(); // Free the resources
//   }

//   delay(10000);
// }