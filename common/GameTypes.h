#ifndef GAME_TYPES_HPP
#define GAME_TYPES_HPP

#include <vector>

enum class UserAction {
    Start,
    Pause,
    Terminate,
    Left,
    Right,
    Up,
    Down,
    Action
};

struct GameInfo {
    std::vector<std::vector<int>> field;
    std::vector<std::vector<int>> next;
    int score;
    int high_score;
    int level;
    int speed;
    int pause;
};

#endif // GAME_TYPES_HPP