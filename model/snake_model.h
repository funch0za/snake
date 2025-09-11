#ifndef GAME_MODEL_HPP
#define GAME_MODEL_HPP

#include "../common/snake_types.h"
#include "../common/snake_consts.h"
#include <vector>

class GameModel {
public:
    GameModel();

    // Game state management
    void initialize();
    void update();
    void processInput(UserAction action, bool hold);
    void manualMove();
    void moveForward(); // Для ручного движения змейки вперед
    // Getters
    GameInfo getGameInfo() const;
    bool isGameOver() const;
    bool isPaused() const;
    bool isRunning() const;

private:
    // Game state
    std::vector<std::pair<int, int>> snake_;
    std::pair<int, int> food_;
    int direction_; // 0: up, 1: right, 2: down, 3: left
    int next_direction_;

    // Game status
    bool game_over_;
    bool paused_;
    bool running_;

    // Game metrics
    int score_;
    int high_score_;
    int level_;
    int speed_;
    int tick_counter_;

    // Field representation
    std::vector<std::vector<int>> field_;
    std::vector<std::vector<int>> next_;

    // Internal methods
    void spawnFood();
    void moveSnake();
    void updateField();
    bool checkCollision(int x, int y) const;
    void increaseScore();
    void updateLevel();

};

#endif // GAME_MODEL_HPP
