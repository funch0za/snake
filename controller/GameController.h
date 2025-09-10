#ifndef GAME_CONTROLLER_HPP
#define GAME_CONTROLLER_HPP

#include "../common/GameTypes.h"
#include "../model/GameModel.h"

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