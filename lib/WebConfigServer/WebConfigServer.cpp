#include "WebConfigServer.h"


WebConfigServer::WebConfigServer(void){
  // Serial.println("WebConfigServer loaded");
}

void WebConfigServer::begin(void){

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS Mount failed");
  } else {
    Serial.println("SPIFFS Mount succesfull");
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");

    if (SPIFFS.exists(CONFIG_FILE)) {
      Serial.print(CONFIG_FILE); Serial.println(" exists!");
      loadConfigurationFile(CONFIG_FILE, config);
    printFile(CONFIG_FILE);
    }

  }

}


// Format bytes:
String WebConfigServer::formatBytes(size_t bytes){
  if (bytes < 1024) {
    return String(bytes) + " B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + " KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + " MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
  }
}


// Saves the web configuration from a POST req to a file
void WebConfigServer::saveWebConfigurationFile(const char *filename, const JsonDocument& doc){
  // Delete existing file, otherwise the configuration is appended to the file
  SPIFFS.remove(filename);

  // Open file for writing
  // File file = SD.open(filename, FILE_WRITE);
  File file = SPIFFS.open(filename, "w");
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }

  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
}


void WebConfigServer::parseConfig(const JsonDocument& doc, Config& config){

    // serializeJsonPretty(doc, Serial);

    // Network object:
    strlcpy(config.network.ssid_name, doc["network"]["ssid_name"] | "SSID_name", sizeof(config.network.ssid_name));
    strlcpy(config.network.ssid_password, doc["network"]["ssid_password"] | "SSID_password", sizeof(config.network.ssid_password));
    config.network.connection = false;

    // MQTT object:
    strlcpy(config.mqtt.server, doc["mqtt"]["server"] | "server_address", sizeof(config.mqtt.server));
    config.mqtt.port = doc["mqtt"]["port"] | 8888;

    // Save the config file with new configuration:
    saveWebConfigurationFile(CONFIG_FILE,doc);

}


// Loads the configuration from a file
void WebConfigServer::loadConfigurationFile(const char *filename, Config& config){
  // Open file for reading
  File file = SPIFFS.open(filename, "r");

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  DynamicJsonDocument doc(JSON_CONFIG_BUFF_SIZZE);

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  // Parse file to Config struct object:
  parseConfig(doc,config);

  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
}


// Saves the Config struct configuration to a file
// void WebConfigServer::saveConfigurationFile(const char *filename, const Config &config)
void WebConfigServer::saveConfigurationFile(const char *filename, Config& config){
  // Delete existing file, otherwise the configuration is appended to the file
  SPIFFS.remove(filename);

  // Open file for writing
  // File file = SD.open(filename, FILE_WRITE);
  File file = SPIFFS.open(filename, "w");
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  DynamicJsonDocument doc(JSON_CONFIG_BUFF_SIZZE);

  // Network object:
  doc["network"]["ssid_name"] = config.network.ssid_name;
  doc["network"]["ssid_password"] = config.network.ssid_password;
  doc["network"]["connection"] = config.network.connection;

  // Network object:
  doc["mqtt"]["server"] = config.mqtt.server;
  doc["mqtt"]["port"] = config.mqtt.port;



  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
}


// Prints the content of a file to the Serial
void WebConfigServer::printFile(String filename){
  // Open file for reading
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    Serial.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }
  Serial.println();

  // Close the file
  file.close();
}


// Restore the backup of a file:
// void saveConfigurationFile(const char *filename, const Config &config){
void WebConfigServer::restoreBackupFile(String filenamechar){

      String filename = filenamechar;
      if(!filename.startsWith("/")) filename = "/"+filename;
      String filename_bak =  "/.bak"+filename;
      Serial.print("Restoring backup for: "); Serial.println(filename);
      // Delete existing file, otherwise the configuration is appended to the file
      SPIFFS.remove(filename);

      File file = SPIFFS.open(filename, "w+");
      if (!file) {
        Serial.print(F("Failed to read file: "));Serial.println(filename);
        return;
      }

      //Serial.print("Opened: "); Serial.println(filename);
      File file_bak = SPIFFS.open(filename_bak, "r");
      if (!file) {
        Serial.print(F("Failed to read backup file: "));Serial.println(filename_bak);
        return;
      }

      //Serial.print("Opened: "); Serial.println(filename_bak);

      size_t n;
      uint8_t buf[64];
      while ((n = file_bak.read(buf, sizeof(buf))) > 0) {
        file.write(buf, n);
      }
      Serial.println("Backup restored");
      file_bak.close();
      file.close();

      // Serial.println("New config:");
      //printFile(filename);
}


void WebConfigServer::updateGpio(ESP8266WebServer *server){
  String gpio = server->arg("id");
  String val = server->arg("val");
  String success = "1";

  int pin = D5;
  if ( gpio == "D5" ) {
    pin = D5;
  } else if ( gpio == "D7" ) {
     pin = D7;
   } else if ( gpio == "D8" ) {
     pin = D8;
   } else if ( gpio == "LED_BUILTIN" ) {
     pin = LED_BUILTIN;
   } else {
     // Built-in nodemcu GPI16, pin 16 led D0
     // esp12 led is 2 or D4
     pin = LED_BUILTIN;

    }

  // Reverse current LED status:
  pinMode(pin, OUTPUT);
  // digitalWrite(pin, LOW);
  Serial.println(pin);
  Serial.print("Current status:");
  Serial.println(digitalRead(pin));
  digitalWrite(pin, !digitalRead(pin));


  // if ( val == "true" ) {
  //   digitalWrite(pin, HIGH);
  // } else if ( val == "false" ) {
  //   digitalWrite(pin, LOW);
  // } else {
  //   success = "true";
  //   Serial.println("Err parsing GPIO Value");
  // }

  String json = "{\"gpio\":\"" + String(gpio) + "\",";
  json += "\"val\":\"" + String(val) + "\",";
  json += "\"success\":\"" + String(success) + "\"}";

  server->send(200, "application/json", json);
  Serial.println("GPIO updated!");

}


void WebConfigServer::configureServer(ESP8266WebServer *server){

  // Create configuration file
  //saveConfigurationFile(CONFIG_FILE, config);

  //printFile(CONFIG_FILE);

  //SERVER INIT
  //list directory
  server->serveStatic("/", SPIFFS, "/index.html");
  server->serveStatic("/css", SPIFFS, "/css");
  server->serveStatic("/js", SPIFFS, "/js");
  server->serveStatic("/certs", SPIFFS, "/certs");
  server->serveStatic("/config.json", SPIFFS, "/config.json");

  server->on("/gpio", HTTP_POST, [& ,server](){
    updateGpio(server);
  });

  server->on("/save_config", HTTP_POST, [& ,server](){

    DynamicJsonDocument doc(JSON_CONFIG_BUFF_SIZZE);
    deserializeJson(doc, server->arg("plain"));

    // JsonObject network = doc["network"];

    Serial.print("JSON POST: ");
    serializeJsonPretty(doc, Serial);
    Serial.println("");

    // Parse file to Config struct object to update internal config:
    parseConfig(doc,config);

    server->send ( 200, "text/json", "{success:true}" );

  });

  server->on("/restore_config", HTTP_POST, [& ,server](){
    // StaticJsonBuffer<200> newBuffer;
    // JsonObject& newjson = newBuffer.parseObject(server->arg("plain"));
    //
    // server->send ( 200, "text/json", "{success:true}" );
/*
    StaticJsonDocument doc(JSON_CONFIG_BUFF_SIZZE);
    //DynamicJsonDocument doc(JSON_CONFIG_BUFF_SIZZE);
    deserializeJson(doc, server->arg("plain"));

    // JsonObject network = doc["network"];

    Serial.print("JSON POST: ");
    serializeJsonPretty(doc, Serial);
    Serial.println("");

    // Restore the filename requested:
    restoreBackupFile(doc["filename"]);

    // server->send ( 200, "text/json", "{success:true}" );
*/


    Serial.print("JSON POST: "); Serial.println(server->arg("plain"));
    Serial.print("JSON POST argName: "); Serial.println(server->argName(0));
    Serial.print("JSON POST args: "); Serial.println(server->args());

      if (server->args() > 0){
         for (int i = 0; i < server->args(); i++ ) {
             // Serial.print("POST Arguments: " ); Serial.println(server->args(i));
             Serial.print("Name: "); Serial.println(server->argName(i));
             Serial.print("Value: "); Serial.println(server->arg(i));
        }
     }

    if( ! server->hasArg("filename") || server->arg("filename") == NULL){
      server->send(400, "text/plain", "400: Invalid Request");
    } else{
      Serial.print("File to restore: "); Serial.println(server->arg("filename"));
      restoreBackupFile(server->arg("filename"));
      server->send ( 200, "text/json", "{success:true}" );
    }


  });


  //called when the url is not defined here
  //use it to load content from SPIFFS
  server->onNotFound([& ,server]() {
    // if (!handleFileRead(server.uri())) {
      server->send(404, "text/plain", "FileNotFound");
    // }
  });

  server->begin();
  Serial.println ( "HTTP server started" );


}


void WebConfigServer::handle(void){

}
