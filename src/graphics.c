#include "graphics.h"
#include <raylib.h>

void DrawSlider(int x, int y, int width, int height, float level, const char *label) {
    // Clamp level between 0 and 1
    if (level < 0.0f) level = 0.0f;
    if (level > 1.0f) level = 1.0f;
    
    // Draw slider outline
    DrawRectangleLines(x, y, width, height, BLACK);
    
    // Compute filled height based on level
    int fillHeight = (int)(level * height);
    DrawRectangle(x, y + (height - fillHeight), width, fillHeight, GREEN);
    
    // Draw label underneath the slider and display the level value above the slider
    DrawText(label, x, y + height, 20, DARKGRAY);
    
    char levelText[16];
    // sprintf(levelText, "%.1f", level);
    // DrawText(levelText, x, y - 20, 20, DARKGRAY);
}
