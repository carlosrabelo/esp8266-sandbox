// Bitcoin Ticker
//
// Author: Carlos Rabelo <developer@carlosrabelo.com.br>

#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include "config.h"

const String url = "http://api.coindesk.com/v1/bpi/currentprice.json";

const int buttonPin = 2;

int state = 0;

float usd_rate = 0;
float eur_rate = 0;

Adafruit_PCD8544 display = Adafruit_PCD8544(12, 5, 4);

HTTPClient http;

void setup() {

  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(115200);

  display.begin();
  display.setContrast(30);
  // display.clearDisplay();
  // display.display();

  Serial.println();
  Serial.print("Connecting");

  // EN: Try to connect wireless network
  // PT: Tenta conectar na rede wireless

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempt = 50;

  while (WiFi.status() != WL_CONNECTED && attempt > 0) {

    Serial.print(".");

    delay(250);

    attempt--;

  }

  // EN: If has success, show address ip
  // PT: Se tiver sucesso, mostra o numero de ip

  if (attempt > 0) {

    Serial.println("Success!");
    Serial.print("IP Address is: ");
    Serial.println(WiFi.localIP());

    state = 2;

  } else {

    Serial.println("Unsuccess!");
    Serial.println("Press reset to retry");

  }

}

void loop() {

  String usd_value;
  String eur_value;

  switch (state) {

    // EN: Check the refresh button state
    // PT: Verifica o estado do botão de refresh

    case 1:
    
      int buttonState;

      buttonState = digitalRead(buttonPin);

      // EN: If pressed, go to next state
      // PT: Se pressionado, vai para o próximo estado

      if (buttonState == LOW) {
        state++;
      }

      break;

    // EN: Show "Updating" message
    // PT: Mostra a mensagem "Updating" (Atualizando)

    case 2:

      Serial.println("Updating");

      display.clearDisplay();
      display.setCursor(0,0);
      display.println("Updating");
      display.display();

      delay(200);

      state++;

      break;

    // EN: Get data from remote server
    // PT: Obtenha dados do servidor remoto

    case 3:

      int code;
 
      http.begin(url);
      
      code = http.GET();

      if (code == 200) {

        String load = http.getString();

        DynamicJsonBuffer jsonBuffer(1100);

        JsonObject& root = jsonBuffer.parseObject(load);
        JsonObject& bpi = root["bpi"];
        JsonObject& bpi_USD = bpi["USD"];
        JsonObject& bpi_EUR = bpi["EUR"];

        usd_rate = bpi_USD["rate_float"];
        eur_rate = bpi_EUR["rate_float"];

      } else {

        usd_rate = -1;
        eur_rate = -1;

      }

      http.end();

      state++;

      break;

    // EN: Show Bitcoin rate on lcd display
    // PT: Mostra o valor do Bitcoin no display LCD

    case 4:

      if (usd_rate != -1 || eur_rate != -1 ) {

        usd_value = "USD: ";
        usd_value += usd_rate;

        eur_value = "EUR: ";
        eur_value += eur_rate;

      } else {

        usd_value = "?";
        eur_value = "?";

      }

      Serial.print("1 Bitcoin = ");
      Serial.print(usd_value);
      Serial.print(", ");
      Serial.println(eur_value);

      display.clearDisplay();
      display.setCursor(0,0);
      display.println("1 Bitcoin");
      display.setCursor(0,10);
      display.println(usd_value);
      display.setCursor(0,20);
      display.println(eur_value);
      display.setCursor(0,30);
      display.println("source:");
      display.setCursor(0,40);
      display.println("coindesk.com");
      display.display();

      state++;

      break;

    // EN: Go to state 1
    // PT: Vai para o estado 1

    default:

      state = 1;

      break;

  }

}
