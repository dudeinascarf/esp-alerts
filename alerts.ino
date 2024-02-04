#include "Arduino.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "WebServer.h"
#include "Arduino_JSON.h"
#include "Audio.h"
#include "SD.h"
#include "FS.h"
#include "NTPClient.h"

// Digital I/O used

// SD Card
#define SD_CS 5
#define SPI_MOSI 2
#define SPI_MISO 19
#define SPI_SCK 18

// Speaker
#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26

// Pin Definitions
WebServer server(3333);

Audio audio;

// WIFI credentials
String ssid = "name";
String password = "pass";

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;
String jsonBuffer;

String ipifyUrl = "http://api.ipify.org";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

int randomNumber;

void setup()
{
    // WIFI CONFIGS START------------------------------------------//
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    while (WiFi.status() != WL_CONNECTED)
        Serial.println("WiFi connected");

    Serial.begin(115200);

    // Obtain local ip
    IPAddress ipAddress = WiFi.localIP();
    String localip = ipAddress.toString();
    Serial.println("IP address:");
    Serial.print("Local: ");
    Serial.println(WiFi.localIP());

    WiFiClient client;
    HTTPClient http;
    String publicip;

    // Obtain public ip
    http.begin(client, ipifyUrl);
    int httpCode_GET = http.GET();
    if (httpCode_GET > 0)
    {
        if (httpCode_GET == HTTP_CODE_OK)
        {
            publicip = http.getString();
            Serial.println("Public: " + publicip);
        }
    }
    else
    {
        Serial.println("Error fetching IP address");
    }
    http.end();

    //   This is optional, if you want to send obtained ips to your server endpoint (it's usefull if you need to debug w/o arduino IDE or some other monitor)
    //   http.begin(client, "<url:endpoint>");
    //   http.addHeader("Content-Type", "application/json");
    //   Serial.print("[HTTP] POST...\n");
    //   String jsonPayload = "{\"localIP\":\"" + localip + "\"," + "\"" + "publicIP\":\"" + publicip + "\"}";
    //   int httpCode_POST = http.POST(jsonPayload);
    //   http.end();

    // Starts listening to '/deploy_alert' endpoint
    server.on("/deploy_alert", handleBody);
    server.begin();
    Serial.println("Server listening");
    // WIFI CONFIGS END----------------------------------------------//

    delay(1500);

    // SD CARD AND AUDIO CONFIGS START-------------------------------//
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

    if (!SD.begin(SD_CS))
    {
        Serial.println("Error talking to SD card!");
        while (true)
            ; // end program
    }

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(21); // 0...21
    // SD CARD AND AUDIO CONFIGS END--------------------------------//
    delay(500);

    // Initialize the NTP client
    timeClient.begin();

    delay(1500);
}

void loop()
{
    server.handleClient(); // Handling of incoming requests
    audio.loop();
}

void handleBody()
{ // Handler for the body path
    String prfx = "/";
    String pofx = ".mp3";

    if (server.hasArg("plain") == false)
    { // Check if body received
        server.send(200, "text/plain", "Body not received");
        return;
    }

    String message = "Body received:\n";
    message += server.arg("plain");
    message += "\n";

    server.send(200, "text/plain", message);

    jsonBuffer = server.arg("plain");
    JSONVar myObject = JSON.parse(jsonBuffer);

    String project = JSON.stringify(myObject["env"]);

    // Update the NTP client and get the day of the week
    timeClient.update();
    int dayOfWeek = timeClient.getDay(); // 0 = Sunday

    if (dayOfWeek == 5)
    {
        randomNumber = random(3);
        delay(500);
        Serial.print("Random NUM: ");
        Serial.println(randomNumber);

        if (randomNumber == 0)
            audio.connecttoFS(SD, "/circus.mp3");
        else if (randomNumber == 1)
            audio.connecttoFS(SD, "/again.mp3");
        else
            audio.connecttoFS(SD, "/again-and-again.mp3");

        return;
    }

    // Check either request came from front or back
    if (myObject.hasOwnProperty("front"))
    {
        String brand = JSON.stringify(myObject["brand"]);
        String path = prfx + brand.substring(1, brand.length() - 1) + pofx;
        const char *clip = path.c_str();

        delay(500);
        audio.connecttoFS(SD, clip);
    }
    else if (myObject.hasOwnProperty("back"))
    {
        String product = JSON.stringify(myObject["product"]);
        String path = prfx + product.substring(1, product.length() - 1) + pofx;
        const char *clip = path.c_str();

        delay(500);
        audio.connecttoFS(SD, clip);
    }
}