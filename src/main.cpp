

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <sstream>

// *********************************************************************
// constants
#define VERSION "2.0"

// *********************************************************************
// data
const int oneWireBus = D3;
OneWire onewaire(oneWireBus);
DallasTemperature DS18B20(&onewaire);

// WiFi access parameters
const char *ssid = "BELL733";
const char *password = "why133256";

DeviceAddress DS18B20Adrs;

// web server hosted internally to the IoT device.  Listen on TCP port 80,
// which is the default HTTP port.
ESP8266WebServer webServer(80);

// *********************************************************************
// function that return the device address as a string
String printAddress(DeviceAddress deviceAddress)
{
  String addr = "\"";
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 0x10)
    {
      addr += "0";
    }
    addr += String(deviceAddress[i], HEX);
  }
  return addr+"\"";
}

// *********************************************************************
// simple handler that just returns program version to client
void handleVersion()
{
  String sText;

  sText = VERSION;
  sText += "\n";
  webServer.send(200, "text/plain", "Version: " + sText);
}

// *********************************************************************
// simple handler that just returns temperature
void handleTemp()
{
  float fTemp;
  DS18B20.requestTemperatures();
  fTemp = DS18B20.getTempCByIndex(0);
  webServer.send(200, "text/plain", String(fTemp));
}

// *********************************************************************
// simple handler that just returns sensor id
void handleID()
{
  String sensorId;
  if (DS18B20.isConnected(DS18B20Adrs))
    sensorId = printAddress(DS18B20Adrs);
  webServer.send(200, "application/json", sensorId);
}

// *********************************************************************
// this function examines the URL from the client and based on the extension
// determines the type of response to send.
bool loadFromLittleFS(String path)
{
  bool bStatus;
  String contentType;

  // set bStatus to false assuming this will not work.
  bStatus = false;

  // assume this will be the content type returned, unless path extension
  // indicates something else
  contentType = "text/plain";

  // DEBUG:  print request URI to user:
  Serial.print("Requested URI: ");
  Serial.println(path.c_str());

  // if no path extension is given, assume index.html is requested.
  if (path.endsWith("/"))
    path += "index.html";

  // look at the URI extension to determine what kind of data to
  // send to client.
  if (path.endsWith(".src"))
    path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html"))
    contentType = "text/html";
  else if (path.endsWith(".htm"))
    contentType = "text/html";
  else if (path.endsWith(".css"))
    contentType = "text/css";
  else if (path.endsWith(".js"))
    contentType = "application/javascript";
  else if (path.endsWith(".png"))
    contentType = "image/png";
  else if (path.endsWith(".gif"))
    contentType = "image/gif";
  else if (path.endsWith(".jpg"))
    contentType = "image/jpeg";
  else if (path.endsWith(".ico"))
    contentType = "image/x-icon";
  else if (path.endsWith(".xml"))
    contentType = "text/xml";
  else if (path.endsWith(".pdf"))
    contentType = "application/pdf";
  else if (path.endsWith(".zip"))
    contentType = "application/zip";

  // try to open file in LittleFS filesystem
  File dataFile = LittleFS.open(path.c_str(), "r");
  // if dataFile <> 0, then it was opened successfully.
  if (dataFile)
  {
    if (webServer.hasArg("download"))
      contentType = "application/octet-stream";
    // stream the file to the client.  check that it was completely sent.
    if (webServer.streamFile(dataFile, contentType) != dataFile.size())
    {
      Serial.println("Error streaming file: " + String(path.c_str()));
    }
    // close the file
    dataFile.close();
    // indicate success
    bStatus = true;
  }

  return bStatus;
}

// *********************************************************************
void handleWebRequests()
{
  if (!loadFromLittleFS(webServer.uri()))
  {
    // file not found.  Send 404 response code to client.
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += webServer.uri();
    message += "\nMethod: ";
    message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += webServer.args();
    message += "\n";
    for (uint8_t i = 0; i < webServer.args(); i++)
    {
      message += " NAME:" + webServer.argName(i) + "\n VALUE:" + webServer.arg(i) + "\n";
    }
    webServer.send(404, "text/plain", message);
    Serial.println(message);
  }
}

// *********************************************************************
// setup()
//
void setup()
{
  String sStatus;

  // setup serial debug port
  Serial.begin(115200);

  // Start the DS18B20 sensor
  DS18B20.begin();
  DS18B20.getAddress(DS18B20Adrs, 0);

  // Initialize File System
  LittleFS.begin();
  Serial.println("File System Initialized");

  // attempt to connect to WiFi network
  Serial.printf("\nConnecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // set up webserver handlers...

  // handler for version inforamtion
  webServer.on("/version", handleVersion);

  // handler for temperature inforamtion
  webServer.on("/temp", handleTemp);

  // handler for sensor id
  webServer.on("/sensorId", handleID);

  // send all other web requests here by default.
  // The filesystem will be searched for the requested resource
  webServer.onNotFound(handleWebRequests);

  // start web server and print it's address.
  webServer.begin();
  Serial.printf("\nWeb server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
}

// *********************************************************************
// loop ()
//
void loop()
{
  webServer.handleClient();
}
