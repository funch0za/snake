#include "GameModel.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>

GameModel::GameModel() {
    std::srand(std::time(nullptr));
    initialize();
}

void GameModel::initialize() {
    // Initialize field matrices
    field_.resize(FIELD_HEIGHT, std::vector<int>(FIELD_WIDTH, 0));
    next_.resize(FIELD_HEIGHT, std::vector<int>(FIELD_WIDTH, 0));

    // Reset game state
    snake_.clear();
    direction_ = 1; // Start moving right
    game_over_ = false;
    paused_ = false;
    running_ = false;

    // Initialize snake in the middle
    int start_x = FIELD_WIDTH / 2;
    int start_y = FIELD_HEIGHT / 2;
    snake_.push_back({start_x, start_y});
    snake_.push_back({start_x - 1, start_y});
    snake_.push_back({start_x - 2, start_y});

    // Reset scores
    score_ = 0;
    high_score_ = 0;
    level_ = 1;
    speed_ = INITIAL_SPEED;

    spawnFood();
    updateField();
}

void GameModel::update() {
    // Only update the field, don't move snake automatically
    updateField();
}

void GameModel::processInput(UserAction action, bool hold) {
    if (hold) {
        switch (action) {
            case UserAction::Start:
                if (!running_ || game_over_) {
                    initialize();
                    running_ = true;
                    paused_ = false;
                    game_over_ = false;
                }
                break;

            case UserAction::Pause:
                if (running_ && !game_over_) {
                    paused_ = !paused_;
                }
                break;

            case UserAction::Terminate:
                running_ = false;
                game_over_ = true;
                break;

            case UserAction::Left:
                if (running_ && !paused_ && !game_over_ && direction_ != 1) {
                    direction_ = 3; // Set to left
                    moveSnake(); // Move immediately after direction change
                }
                break;

            case UserAction::Right:
                if (running_ && !paused_ && !game_over_ && direction_ != 3) {
                    direction_ = 1; // Set to right
                    moveSnake();
                }
                break;

            case UserAction::Up:
                if (running_ && !paused_ && !game_over_ && direction_ != 2) {
                    direction_ = 0; // Set to up
                    moveSnake();
                }
                break;

            case UserAction::Down:
                if (running_ && !paused_ && !game_over_ && direction_ != 0) {
                    direction_ = 2; // Set to down
                    moveSnake();
                }
                break;

            case UserAction::Action:
                if (running_ && !paused_ && !game_over_) {
                    speed_ = std::max(speed_ - 1, 0);
                }
                break;
        }
    } else if (action == UserAction::Action) {
        if (running_ && !paused_ && !game_over_) {
            moveSnake(); // Move forward in current direction
        }
    }
}


void GameModel::moveSnake() {
    if (snake_.empty()) return;

    auto head = snake_.front();

    // Calculate new head position based on direction
    switch (direction_) {
        case 0:
            head.second--;
            break; // Up
        case 1:
            head.first++;
            break;  // Right
        case 2:
            head.second++;
            break; // Down
        case 3:
            head.first--;
            break;  // Left
    }

    // Check collisions
    if (checkCollision(head.first, head.second)) {
        game_over_ = true;
        running_ = false;
        return;
    }

    // Move snake - add new head
    snake_.insert(snake_.begin(), head);

    // Check if food eaten
    if (head.first == food_.first && head.second == food_.second) {
        increaseScore();
        spawnFood();
    } else {
        // Remove tail if no food eaten
        if (snake_.size() > 1) {
            snake_.pop_back();
        }
    }

    updateField();
}

void GameModel::spawnFood() {
    int x, y;
    bool valid;

    do {
        x = std::rand() % FIELD_WIDTH;
        y = std::rand() % FIELD_HEIGHT;
        valid = true;

        // Check if food spawns on snake
        for (const auto &segment: snake_) {
            if (segment.first == x && segment.second == y) {
                valid = false;
                break;
            }
        }
    } while (!valid);

    food_ = {x, y};
}

bool GameModel::checkCollision(int x, int y) const {
    // Wall collision
    if (x < 0 || x >= FIELD_WIDTH || y < 0 || y >= FIELD_HEIGHT) {
        return true;
    }

    // Self collision (skip head)
    for (size_t i = 1; i < snake_.size(); i++) {
        if (snake_[i].first == x && snake_[i].second == y) {
            return true;
        }
    }

    return false;
}

void GameModel::increaseScore() {
    score_ += 10;
    if (score_ > high_score_) {
        high_score_ = score_;
    }
    updateLevel();
}

void GameModel::updateLevel() {
    level_ = (score_ / 50) + 1;
}

void GameModel::updateField() {
    // Clear field
    for (auto &row: field_) {
        std::fill(row.begin(), row.end(), 0);
    }

    // Draw snake
    for (const auto &segment: snake_) {
        if (segment.second >= 0 && segment.second < FIELD_HEIGHT &&
            segment.first >= 0 && segment.first < FIELD_WIDTH) {
            field_[segment.second][segment.first] = 1;
        }
    }

    // Draw food
    if (food_.second >= 0 && food_.second < FIELD_HEIGHT &&
        food_.first >= 0 && food_.first < FIELD_WIDTH) {
        field_[food_.second][food_.first] = 2;
    }
}

GameInfo GameModel::getGameInfo() const {
    return {
            field_,
            next_,
            score_,
            high_score_,
            level_,
            speed_,
            paused_ ? 1 : 0
    };
}

bool GameModel::isGameOver() const { return game_over_; }

bool GameModel::isPaused() const { return paused_; }

bool GameModel::isRunning() const { return running_; }