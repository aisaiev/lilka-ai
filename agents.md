## Project overview
This project provides OpenAI GPT integration for Lilka ESP32 dev board, enabling AI-powered conversations through multiple interface modes with web-based configuration.

## Features
- **OpenAI Integration**: Full integration with OpenAI's Chat Completions API supporting GPT-4, GPT-3.5-turbo, and other models
- **Conversation History**: Maintains context across multiple turns for natural dialogue flow
- **Dual Interface Modes**: 
  - On-device chat mode with virtual keyboard and scrollable response display
  - RS-232 serial mode for terminal-based interaction
- **Keira WiFi Integration**: Automatically reads WiFi credentials from Keira OS NVS storage
- **Web Configuration Portal**: WiFi access point with web interface for OpenAI API configuration
- **Persistent Settings**: NVS storage for OpenAI API keys and model preferences
- **Quick Settings**: On-device model switching without full reconfiguration
- **Menu System**: Multi-level navigation with proper back handling following Keira OS patterns
- **Error Handling**: Comprehensive error messages parsed from OpenAI API responses
- **Lightweight Design**: Optimized for ESP32 resource constraints with efficient memory management
- **Scrollable UI**: Long responses can be scrolled using directional buttons

## Languages and Tools
- **Programming Language**: C++
- **SDK**: Lilka SDK for ESP32
- **Docs**: [Lilka SDK Documentation](https://docs.lilka.dev/projects/sdk/uk/latest/)
- **APIs**: OpenAI Chat Completions API
- **Build System**: PlatformIO

## Architecture
- **main.cpp**: Application entry, menu system, mode selection, Keira WiFi credential loading
- **wifi_config**: Reads WiFi credentials from Keira's NVS storage (namespace "kwifi")
- **openai_client**: HTTP client for OpenAI API with conversation history tracking
- **ui_handler**: Display management with canvas-based rendering for flicker-free updates
- **serial_handler**: RS-232 communication handler for terminal mode
- **settings**: NVS-based persistent configuration storage (OpenAI API key and model only)
- **config_server**: Async web server for OpenAI configuration portal

## Requirements
- Lilka ESP32 dev board (v2)
- Keira OS with configured WiFi credentials
- OpenAI API Key with active credits
- (Optional) RS-232 Serial Interface for serial mode
- (Optional) USB to Serial Adapter for development/debugging

## Configuration
- WiFi: Automatically loaded from Keira OS NVS storage (namespace "kwifi")
- OpenAI: Web-based setup via WiFi
- Settings stored in separate NVS namespace: OpenAI API Key, AI Model
- Quick settings for runtime model changes
- All configuration persists across reboots

## Technical Details
- **Display**: Full-screen canvas rendering with centered text and custom UI elements
- **Input**: Lilka's built-in InputDialog with on-screen keyboard
- **Serial**: GPIO 43 (TX), GPIO 44 (RX), 9600 baud, 8N1
- **HTTP**: HTTPS support for secure OpenAI API communication
- **Memory**: JSON documents with proper cleanup, conversation history management
- **Navigation**: Keira OS-inspired menu patterns with activation buttons and cursor tracking