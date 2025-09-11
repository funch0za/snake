#ifndef GAME_VIEW_HPP
#define GAME_VIEW_HPP

#include "../../common/snake_types.h"
#include <ncurses.h>

class GameView {
public:
    GameView();
    ~GameView();

    bool initialize();
    void render(const GameInfo& game_info);
    void showGameOver();
    void showPause();
    void cleanup();

private:
    void drawField(const std::vector<std::vector<int>>& field);
    void drawSidebar(const GameInfo& game_info);
    void drawBorders();
};

#endif // GAME_VIEW_HPP
