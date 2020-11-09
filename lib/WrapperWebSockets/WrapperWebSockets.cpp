#include "WrapperWebSockets.h"

String getHeapFree(void ){
  return String(GET_FREE_HEAP);
};

void WrapperWebSockets::init(void) {
  Serial.println("Starting Websocket server...\n");

  // start webSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // listObjets[0] = "heap_free";
  // listObjetFunctions[0] = getHeapFree;
  // listObjetsIndex++;
  this->addObjectToPublish("heap_free", getHeapFree);

};



void WrapperWebSockets::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip =  webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "{\"Connected\": true}");
        }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);

            if(payload[0] == '#') {
                // we get RGB data

                // decode rgb data
                // uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);

            //     analogWrite(LED_RED, ((rgb >> 16) & 0xFF));
            //     analogWrite(LED_GREEN, ((rgb >> 8) & 0xFF));
            //     analogWrite(LED_BLUE, ((rgb >> 0) & 0xFF));
            // }

            break;
    }
  }

};


void WrapperWebSockets::publishClients(void) {

  numClients++;
  bool ping = (numClients % 2);
  int i = webSocket.connectedClients(ping);
  // Serial.printf("%d Connected websocket clients ping: %d\n", i, ping);
  // To send msg to all connected clients:
  // webSocket.broadcastTXT("message here");
  // To send msg to specific client id:
  // webSocket.sendTXT(i-1, msg_ws.c_str());

  String msg_ws = "{";

  for (int i = 0; i <= this->listObjetsIndex-1; i++) {
    msg_ws +="\""+ this->listObjets[i] + "\": " + (*this->listObjetFunctions[i])();
    if (this->listObjetsIndex-1 != i) msg_ws += " , ";
    else msg_ws += "}";

  };

  // Serial.println(msg_ws.c_str());
  webSocket.sendTXT(i-1, msg_ws.c_str());

};


void WrapperWebSockets::handle(void) {
  webSocket.loop();
};


bool WrapperWebSockets::addObjectToPublish(String key, String (*valueFunction)()) {

  if (this->listObjetsIndex+1 <= MAX_LIST_OBJECT_FUNCTIONS){
    this->listObjets[listObjetsIndex] = key;
    this->listObjetFunctions[listObjetsIndex] = valueFunction;
    this->listObjetsIndex++;
    Serial.print("Added WebSocket msg object[");
    Serial.print(this->listObjetsIndex);
    Serial.println("]: " + key);

    return true;
  } else {
    Serial.print("NOT added WebSocket msg object[");
    Serial.print(this->listObjetsIndex);
    Serial.println("]: " + key);

    return false;
  }
};
