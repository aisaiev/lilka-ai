#include "ui_handler.h"
#include "config.h"

UIHandler::UIHandler() {}

void UIHandler::showWaitingScreen() {
    lilka::display.fillScreen(lilka::display.color565(0, 0, 0));
    
    int16_t x1, y1;
    uint16_t w, h;
    
    lilka::display.setTextSize(2);
    lilka::display.setTextColor(lilka::display.color565(100, 200, 255));
    lilka::display.getTextBounds("Thinking...", 0, 0, &x1, &y1, &w, &h);
    lilka::display.setCursor((lilka::display.width() - w) / 2, SCREEN_HEIGHT / 2);
    lilka::display.println("Thinking...");
}

void UIHandler::showError(const String& error) {
    lilka::Alert alert("Error", error);
    alert.draw(&lilka::display);
    while (!alert.isFinished()) {
        alert.update();
    }
}

bool UIHandler::getInput(String& input) {
    // Use Lilka's built-in input dialog with on-screen keyboard
    // Use Canvas to prevent flickering
    lilka::Canvas canvas;
    lilka::InputDialog dialog("Enter your message:");
    
    while (!dialog.isFinished()) {
        dialog.update();
        dialog.draw(&canvas);
        lilka::display.drawCanvas(&canvas);
    }
    
    String result = dialog.getValue();
    
    if (result.length() > 0) {
        input = result;
        return true;
    }
    
    return false;
}

void UIHandler::displayResponse(const String& response) {
    int scrollY = 0; // Scroll offset

    while (true) {
        // Handle input
        lilka::State state = lilka::controller.getState();
        if (state.a.justPressed) break;
        if (state.up.justPressed && scrollY < 0) scrollY += 10; // Scroll up
        if (state.down.justPressed) scrollY -= 10; // Scroll down
        
        // Render
        lilka::Canvas canvas;
        canvas.fillScreen(0);
        canvas.setTextSize(1);
        canvas.setTextWrap(true);
        
        // Middle: Response text with scrollY offset
        int contentY = TOP_MARGIN + 25;
        
        canvas.setTextColor(canvas.color565(255, 255, 255));
        canvas.setCursor(0, contentY + scrollY);
        canvas.print(response);

        // Top: Header with background
        canvas.fillRect(0, 0, SCREEN_WIDTH, TOP_MARGIN + 8, canvas.color565(30, 30, 60));
        canvas.setTextColor(canvas.color565(150, 150, 255));
        canvas.setCursor(MARGIN, TOP_MARGIN);
        canvas.print("Response:");
        
        // Bottom: Draw footer as overlay with dark green background
        int footerY = SCREEN_HEIGHT - 20;
        
        canvas.fillRect(0, footerY, SCREEN_WIDTH, 20, canvas.color565(0, 40, 0));
        
        // Footer text
        int16_t x1, y1;
        uint16_t w, h;
        canvas.setTextColor(canvas.color565(100, 255, 100));
        canvas.getTextBounds("A - continue", 0, 0, &x1, &y1, &w, &h);
        canvas.setCursor((canvas.width() - w) / 2, footerY + 15);
        canvas.print("A - continue");
        
        lilka::display.drawCanvas(&canvas);
        delay(50);
    }
}
