#include "GameController.h"

GameController::GameController() {
    initializeGame();
}

void GameController::userInput(UserAction action, bool hold) {
    model_.processInput(action, hold);
}

GameInfo GameController::updateCurrentState() {
    model_.update();
    return model_.getGameInfo();
}

void GameController::initializeGame() {
    model_.initialize();
}

void GameController::cleanup() {
    // Cleanup resources if needed
}