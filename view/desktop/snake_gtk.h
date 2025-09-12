#ifndef SNAKE_GTK_H
#define SNAKE_GTK_H

#include "../../common/snake_types.h"
#include <gtk/gtk.h>

class GameViewGTK {
public:
    GameViewGTK();
    ~GameViewGTK();

    bool initialize(int argc, char** argv);
    void render(const GameInfo& game_info);
    void showGameOver();
    void showPause();
    void cleanup();

    // Public for callback access
    GtkWidget* window;
    GtkWidget* drawing_area;
    GtkWidget* sidebar_box;

private:
    void drawField(const std::vector<std::vector<int>>& field, cairo_t* cr);
    void drawSidebar(const GameInfo& game_info);
    void drawBorders(cairo_t* cr);
    
    GameInfo current_game_info;
    bool game_over;
    bool paused;
};

// Global instance for callbacks
extern GameViewGTK* game_view_instance;

#endif // SNAKE_GTK_H
