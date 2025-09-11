#ifndef GAME_CONTROLLER_HPP
#define GAME_CONTROLLER_HPP

#include "../common/snake_types.h"
#include "../model/snake_model.h"

class GameController {
public:
    GameController();

    // Public API according to specification
    void userInput(UserAction action, bool hold);
    GameInfo updateCurrentState();

    // Additional control methods
    void initializeGame();
    void cleanup();

private:
    GameModel model_;
};

#endif // GAME_CONTROLLER_HPP
