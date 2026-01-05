#include "wifi_config.h"
#include <lilka.h>
#include <WiFi.h>
#include <Preferences.h>

// Hash SSID to create storage key (matches Keira's implementation)
String hashSSID(String ssid) {
    uint64_t hash = 0;
    for (int i = 0; i < ssid.length(); i++) {
        hash = (hash << 5) - hash + ssid[i];
    }
    char buffer[9];
    snprintf(buffer, sizeof(buffer), "%08x", (unsigned int)hash);
    return String(buffer);
}

// Load WiFi credentials from Keira's NVS storage
// Returns false if no credentials are configured in Keira
bool loadWiFiCredentials(String& ssid, String& password) {
    Preferences prefs;
    if (!prefs.begin(WIFI_NAMESPACE, true)) {
        // Namespace doesn't exist (no WiFi configured in Keira)
        return false;
    }
    
    if (!prefs.isKey("last_ssid")) {
        prefs.end();
        return false;
    }
    
    ssid = prefs.getString("last_ssid", "");
    if (ssid.isEmpty()) {
        prefs.end();
        return false;
    }
    
    String ssidHash = hashSSID(ssid);
    password = prefs.getString((ssidHash + "_pw").c_str(), "");
    prefs.end();
    
    return true;
}

bool connectToWiFi(String ssid, String password) {
    Serial.printf("Connecting to WiFi: %s\n", ssid.c_str());
    
    lilka::Canvas canvas;
    canvas.fillScreen(0);
    
    int16_t x1, y1;
    uint16_t w, h;
    
    // Title
    canvas.setTextSize(2);
    canvas.setTextColor(canvas.color565(100, 200, 255));
    canvas.getTextBounds("WiFi", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 60);
    canvas.print("WiFi");
    
    // Connecting message
    canvas.setTextSize(1);
    canvas.setTextColor(canvas.color565(200, 200, 200));
    canvas.getTextBounds("Connecting to...", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 100);
    canvas.print("Connecting to...");
    
    // SSID
    canvas.setTextColor(canvas.color565(255, 255, 255));
    canvas.getTextBounds(ssid.c_str(), 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 120);
    canvas.print(ssid);
    
    lilka::display.drawCanvas(&canvas);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    int attempts = 0;
    while (WiFi.status() == WL_DISCONNECTED && attempts < 30) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        attempts++;
    }
    
    bool success = (WiFi.status() == WL_CONNECTED);
    
    canvas.fillScreen(0);
    
    if (success) {
        // Success screen
        canvas.setTextSize(2);
        canvas.setTextColor(canvas.color565(100, 255, 100));
        canvas.getTextBounds("Connected!", 0, 0, &x1, &y1, &w, &h);
        canvas.setCursor((canvas.width() - w) / 2, 60);
        canvas.print("Connected!");
        
        canvas.setTextSize(1);
        canvas.setTextColor(canvas.color565(200, 200, 200));
        canvas.getTextBounds("Network:", 0, 0, &x1, &y1, &w, &h);
        canvas.setCursor((canvas.width() - w) / 2, 100);
        canvas.print("Network:");
        
        canvas.setTextColor(canvas.color565(255, 255, 255));
        canvas.getTextBounds(ssid.c_str(), 0, 0, &x1, &y1, &w, &h);
        canvas.setCursor((canvas.width() - w) / 2, 120);
        canvas.print(ssid);
        
        canvas.setTextColor(canvas.color565(200, 200, 200));
        canvas.getTextBounds("IP Address:", 0, 0, &x1, &y1, &w, &h);
        canvas.setCursor((canvas.width() - w) / 2, 150);
        canvas.print("IP Address:");
        
        String ipAddr = WiFi.localIP().toString();
        canvas.setTextColor(canvas.color565(255, 255, 100));
        canvas.getTextBounds(ipAddr.c_str(), 0, 0, &x1, &y1, &w, &h);
        canvas.setCursor((canvas.width() - w) / 2, 170);
        canvas.print(ipAddr);
        
        lilka::display.drawCanvas(&canvas);
        
        Serial.println("WiFi connected!");
        Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
        
        delay(2000);
    } else {
        // Failure screen
        canvas.setTextSize(2);
        canvas.setTextColor(canvas.color565(255, 100, 100));
        canvas.getTextBounds("Failed!", 0, 0, &x1, &y1, &w, &h);
        canvas.setCursor((canvas.width() - w) / 2, 80);
        canvas.print("Failed!");
        
        canvas.setTextSize(1);
        canvas.setTextColor(canvas.color565(200, 200, 200));
        canvas.getTextBounds("Check credentials", 0, 0, &x1, &y1, &w, &h);
        canvas.setCursor((canvas.width() - w) / 2, 130);
        canvas.print("Check credentials");
        
        lilka::display.drawCanvas(&canvas);
        
        Serial.println("WiFi connection failed!");
        
        delay(3000);
    }
    
    return success;
}
