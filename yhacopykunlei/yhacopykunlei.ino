/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com
*********/

// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include "DHT.h"

// Uncomment one of the lines below for whatever DHT sensor type you're using!
#define DHTTYPE DHT11  // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Replace with your network details
const char* ssid = ">3";
const char* password = "12345678";

// Web Server on port 80
WiFiServer server(80);

// DHT Sensor
const int DHTPin = 5;
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

// Temporary variables
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];

// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);

  dht.begin();

  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);

  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
}

// runs over and over again
void loop() {
  // Listenning for new clients
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New client");
    // bolean to locate when the http request ends
    boolean blank_line = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        if (c == '\n' && blank_line) {
          // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
          float h = dht.readHumidity();
          // Read temperature as Celsius (the default)
          float t = dht.readTemperature();
          // Read temperature as Fahrenheit (isFahrenheit = true)
          float f = dht.readTemperature(true);
          // Check if any reads failed and exit early (to try again).
          if (isnan(h) || isnan(t) || isnan(f)) {
            Serial.println("Failed to read from DHT sensor!");
            strcpy(celsiusTemp, "Failed");
            strcpy(fahrenheitTemp, "Failed");
            strcpy(humidityTemp, "Failed");
          } else {
            // Computes temperature values in Celsius + Fahrenheit and Humidity
            float hic = dht.computeHeatIndex(t, h, false);
            dtostrf(hic, 6, 2, celsiusTemp);
            float hif = dht.computeHeatIndex(f, h);
            dtostrf(hif, 6, 2, fahrenheitTemp);
            dtostrf(h, 6, 2, humidityTemp);
            // You can delete the following Serial.print's, it's just for debugging purposes
            Serial.print("Humidity: ");
            Serial.print(h);
            Serial.print(" %\t Temperature: ");
            Serial.print(t);
            Serial.print(" *C ");
            Serial.print(f);
            Serial.print(" *F\t Heat index: ");
            Serial.print(hic);
            Serial.print(" *C ");
            Serial.print(hif);
            Serial.print(" *F");
            Serial.print("Humidity: ");
            Serial.print(h);
            Serial.print(" %\t Temperature: ");
            Serial.print(t);
            Serial.print(" *C ");
            Serial.print(f);
            Serial.print(" *F\t Heat index: ");
            Serial.print(hic);
            Serial.print(" *C ");
            Serial.print(hif);
            Serial.println(" *F");
          }
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          // your actual web page that displays temperature and humidity
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head>");
          client.println("<style>");
          client.println("body { font-family: Arial, sans-serif; text-align: center; background-color: #f0f8ff; color: #333; margin: 0; padding: 0; }");
          client.println("h1 { color: #0066cc; margin-top: 20px; }");
          client.println(".card { max-width: 500px; margin: 20px auto; padding: 30px; background: #fff; box-shadow: 0 4px 8px rgba(0,0,0,0.2); border-radius: 8px; }");
          client.println(".value { font-size: 2.5em; color: #333; margin: 10px 0; }");
          client.println(".label { font-size: 1.5em; color: #555; }");
          client.println(".icon { font-size: 3em; margin-bottom: 10px; }");
          client.println(".bar-container { width: 100%; background-color: #ddd; border-radius: 20px; overflow: hidden; height: 25px; margin-top: 10px; }");
          client.println(".bar { height: 25px; border-radius: 20px; }");
          client.println(".celsius-bar { background-color: #00bfff; }");
          client.println(".fahrenheit-bar { background-color: #ff6347; }");
          client.println(".humidity-bar { background-color: #32cd32; }");
          client.println("</style>");
          client.println("</head>");
          client.println("<body>");
          client.println("<h1>ESP8266 - Temperature and Humidity</h1>");

          // Celsius Temperature
          client.println("<div class='card'>");
          client.println("<div class='icon'><img src='https://cdn-icons-png.flaticon.com/512/1959/1959295.png'alt='Celsius Icon' style='width: 50px; height: 50px;'/></div>");  // Icon for Celsius
          client.println("<div class='value'>" + String(celsiusTemp) + "°C</div>");
          client.println("<div class='label'>Temperature (Celsius)</div>");
          client.println("<div class='bar-container'>");
          client.println("<div class='bar celsius-bar' style='width: " + String((atof(celsiusTemp) / 50.0) * 100) + "%;'></div>");
          client.println("</div>");
          client.println("</div>");

          // Fahrenheit Temperature
          client.println("<div class='card'>");
          client.println("<div class='icon'><img src='https://cdn-icons-png.flaticon.com/512/1959/1959296.png' alt='Fahrenheit Icon' style='width: 50px; height: 50px;'/></div>");  // Icon for Fahrenheit
          client.println("<div class='value'>" + String(fahrenheitTemp) + "°F</div>");
          client.println("<div class='label'>Temperature (Fahrenheit)</div>");
          client.println("<div class='bar-container'>");
          client.println("<div class='bar fahrenheit-bar' style='width: " + String((atof(fahrenheitTemp) / 120.0) * 100) + "%;'></div>");
          client.println("</div>");
          client.println("</div>");

          // Humidity
          client.println("<div class='card'>");
          client.println("<div class='icon'><img src='https://static.vecteezy.com/system/resources/previews/022/152/992/original/pure-water-icon-isolated-png.png' alt='Humidity Icon' style='width: 50px; height: 50px;'/></div>");

          client.println("<div class='value'>" + String(humidityTemp) + "%</div>");
          client.println("<div class='label'>Humidity</div>");
          client.println("<div class='bar-container'>");
          client.println("<div class='bar humidity-bar' style='width: " + String(atof(humidityTemp)) + "%;'></div>");
          client.println("</div>");
          client.println("</div>");

          client.println("</body>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // when starts reading a new line
          blank_line = true;
        } else if (c != '\r') {
          // when finds a character on the current line
          blank_line = false;
        }
      }
    }
    // closing the client connection
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}