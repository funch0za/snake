#include "controller/GameController.h"
#include "view/GameView.h"
#include <unistd.h>
#include <chrono>
#include <thread>
#include <iostream>

#ifdef CLI_UI

void tetris_loop(void) {
    GameController controller;
    GameView view;

    if (!view.initialize()) {
        return 1;
    }

    bool running = true;
    bool game_started = false;

    GameInfo info = controller.updateCurrentState();
    view.render(info);

    mvprintw(22, 2, "Controls: R-Start, Arrows-Move, P-Pause, Q-Quit");
    mvprintw(23, 2, "Press 'R' to start the game");
    refresh();

    while (running) {
        halfdelay(info.speed);
        int ch = getch();
        bool needs_update = false;

        switch (ch) {
            case 'w': case KEY_UP:
                if (game_started) {
                    controller.userInput(UserAction::Up, true);
                    needs_update = true;
                }
                break;
            case 's': case KEY_DOWN:
                if (game_started) {
                    controller.userInput(UserAction::Down, true);
                    needs_update = true;
                }
                break;
            case 'a': case KEY_LEFT:
                if (game_started) {
                    controller.userInput(UserAction::Left, true);
                    needs_update = true;
                }
                break;
            case 'd': case KEY_RIGHT:
                if (game_started) {
                    controller.userInput(UserAction::Right, true);
                    needs_update = true;
                }
                break;
            case ' ': // Space for action - move forward
                if (game_started) {
                    controller.userInput(UserAction::Action, true);
                    needs_update = true;
                }
                break;
            case 'p': // P for pause
                if (game_started) {
                    controller.userInput(UserAction::Pause, true);
                    needs_update = true;
                }
                break;
            case 'r': // R for start/restart
                controller.userInput(UserAction::Start, true);
                game_started = true;
                needs_update = true;
                break;
            case 'q': // Q for terminate
                controller.userInput(UserAction::Terminate, true);
                running = false;
                break;
            default:
                if (game_started) {
                    controller.userInput(UserAction::Action, false);
                    needs_update = true;
                }
        }

        if (1) {
            info = controller.updateCurrentState();
            view.render(info);

            if (info.pause) {
                mvprintw(22, 2, "Game PAUSED - Press P to resume");
            } else if (game_started) {
                mvprintw(22, 2, "Score: %d - Use arrows to turn, space to move", info.score);
            }
            refresh();
        }
    }

    view.cleanup();
}

#endif

#ifdef DESKTOP_UI

void tetris_loop(void) {
}

#endif

int main() {
  tetris_loop();  
  return 0;
}
