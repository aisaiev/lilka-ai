#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <Preferences.h>

class Settings {
public:
    Settings();
    void begin();
    
    // Getters
    String getWifiSSID();
    String getWifiPassword();
    String getOpenAIKey();
    String getOpenAIModel();
    bool isConfigured();
    
    // Setters
    void setWifiSSID(const String& ssid);
    void setWifiPassword(const String& password);
    void setOpenAIKey(const String& key);
    void setOpenAIModel(const String& model);
    void setConfigured(bool configured);
    
    // Clear all settings
    void clear();
    
private:
    Preferences preferences;
};

#endif // SETTINGS_H
