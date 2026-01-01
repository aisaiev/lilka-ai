#include "config_server.h"

const char* AP_SSID = "LilkaAI-AP";
const char* AP_PASSWORD = "lilka2026";

ConfigServer::ConfigServer(Settings* settings) {
    this->settings = settings;
    this->done = false;
    this->server = new WebServer(80);
}

void ConfigServer::start() {
    // Start Access Point
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    IPAddress IP = WiFi.softAPIP();
    
    // Setup web server routes
    server->on("/", [this]() { this->handleRoot(); });
    server->on("/save", HTTP_POST, [this]() { this->handleSave(); });
    
    server->begin();
}

void ConfigServer::loop() {
    server->handleClient();
}

bool ConfigServer::isDone() {
    return done;
}

void ConfigServer::handleRoot() {
    server->send(200, "text/html", getConfigPage());
}

void ConfigServer::handleSave() {
    if (server->hasArg("wifi_ssid") && server->hasArg("wifi_pass") && server->hasArg("openai_key") && server->hasArg("openai_model")) {
        String ssid = server->arg("wifi_ssid");
        String password = server->arg("wifi_pass");
        String apiKey = server->arg("openai_key");
        String model = server->arg("openai_model");
        
        settings->setWifiSSID(ssid);
        settings->setWifiPassword(password);
        settings->setOpenAIKey(apiKey);
        settings->setOpenAIModel(model);
        settings->setConfigured(true);
        
        String response = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
        response += "<style>body{font-family:Arial;margin:40px;background:#f5f5f5;color:#333;}";
        response += ".success{background:#fff;padding:20px;border-radius:10px;text-align:center;box-shadow:0 2px 8px rgba(0,0,0,0.1);}";
        response += "h1{color:#4CAF50;}</style></head><body>";
        response += "<div class='success'><h1>Settings Saved!</h1>";
        response += "<p>Configuration has been saved successfully.</p>";
        response += "<p>Device will restart in 3 seconds...</p></div>";
        response += "<script>setTimeout(function(){window.location='/restart';},3000);</script>";
        response += "</body></html>";
        
        server->send(200, "text/html", response);
        
        done = true;
    } else {
        server->send(400, "text/html", "<h1>Error: Missing parameters</h1>");
    }
}

String ConfigServer::getConfigPage() {
    // Get current settings
    String currentSSID = settings->getWifiSSID();
    String currentPassword = settings->getWifiPassword();
    String currentAPIKey = settings->getOpenAIKey();
    String currentModel = settings->getOpenAIModel();
    
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>Lilka AI Configuration</title>";
    html += "<style>";
    html += "body{font-family:Arial,sans-serif;background:#f0f2f5;color:#333;margin:0;padding:20px;}";
    html += ".container{max-width:500px;margin:0 auto;background:#fff;padding:30px;border-radius:15px;box-shadow:0 2px 10px rgba(0,0,0,0.1);}";
    html += "h1{color:#1976d2;text-align:center;margin-bottom:30px;}";
    html += ".form-group{margin-bottom:20px;}";
    html += "label{display:block;margin-bottom:8px;color:#555;font-weight:bold;}";
    html += "input{width:100%;padding:12px;border:2px solid #ddd;border-radius:8px;";
    html += "background:#f9f9f9;color:#333;font-size:16px;box-sizing:border-box;}";
    html += "input:focus{outline:none;border-color:#1976d2;background:#fff;}";
    html += "button{width:100%;padding:15px;background:#1976d2;color:#fff;border:none;";
    html += "border-radius:8px;font-size:18px;font-weight:bold;cursor:pointer;margin-top:10px;}";
    html += "button:hover{background:#1565c0;}";
    html += ".info{background:#e3f2fd;padding:15px;border-radius:8px;margin-bottom:20px;font-size:14px;color:#1976d2;}";
    html += "</style></head><body>";
    html += "<div class='container'>";
    html += "<h1>🤖 Лілка AI Config</h1>";
    html += "<div class='info'>Configure your WiFi and OpenAI credentials</div>";
    html += "<form action='/save' method='POST'>";
    html += "<div class='form-group'>";
    html += "<label>WiFi SSID:</label>";
    html += "<input type='text' name='wifi_ssid' value='" + currentSSID + "' placeholder='Your WiFi network name' required>";
    html += "</div>";
    html += "<div class='form-group'>";
    html += "<label>WiFi Password:</label>";
    html += "<input type='password' name='wifi_pass' value='" + currentPassword + "' placeholder='Your WiFi password' required>";
    html += "</div>";
    html += "<div class='form-group'>";
    html += "<label>OpenAI API Key:</label>";
    html += "<input type='text' name='openai_key' value='" + currentAPIKey + "' placeholder='sk-...' required>";
    html += "</div>";
    html += "<div class='form-group'>";
    html += "<label>OpenAI Model:</label>";
    html += "<input type='text' name='openai_model' value='" + currentModel + "' placeholder='gpt-4.1-nano' required>";
    html += "</div>";
    html += "<button type='submit'>Save Configuration</button>";
    html += "</form></div></body></html>";
    return html;
}
