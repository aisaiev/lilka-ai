#ifndef CONFIG_SERVER_H
#define CONFIG_SERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "settings.h"

class ConfigServer {
public:
    ConfigServer(Settings* settings);
    void start();
    void loop();
    bool isDone();
    
private:
    WebServer* server;
    Settings* settings;
    bool done;
    
    void handleRoot();
    void handleSave();
    String getConfigPage();
};

#endif // CONFIG_SERVER_H
