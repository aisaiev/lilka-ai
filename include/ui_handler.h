#ifndef UI_HANDLER_H
#define UI_HANDLER_H

#include <lilka.h>

class UIHandler {
public:
    UIHandler();
    bool getInput(String& input);
    void displayResponse(const String& response);
    void showError(const String& error);
    void showWaitingScreen();
    
private:
    const int SCREEN_WIDTH = 280;
    const int SCREEN_HEIGHT = 240;
    const int MARGIN = 25;
    const int TOP_MARGIN = 24;
};

#endif // UI_HANDLER_H
