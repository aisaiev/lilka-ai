#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <Arduino.h>

// Keira WiFi namespace in NVS storage
#define WIFI_NAMESPACE "network"

// Hash SSID to create storage key (matches Keira's implementation)
String hashSSID(String ssid);

// Load WiFi credentials from Keira's NVS storage
// Returns false if no credentials are configured in Keira
bool loadWiFiCredentials(String& ssid, String& password);

// Connect to WiFi with visual feedback on Lilka display
// Returns true if connection successful, false otherwise
bool connectToWiFi(String ssid, String password);

#endif // WIFI_CONFIG_H
