#include "controller/snake_controller.h"

#ifdef CLI_UI
#include "view/cli/snake_cli.h"
#endif

#ifdef DESKTOP_UI
#include "view/desktop/snake_gtk.h"
#endif

#include <unistd.h>
#include <chrono>
#include <thread>
#include <iostream>

#ifdef CLI_UI
void tetris_loop(void) {
    GameController controller;
    GameView view;

    if (!view.initialize()) {
        return;
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

        switch (ch) {
            case 'w': case KEY_UP:
                if (game_started) {
                    controller.userInput(UserAction::Up, true);
                }
                break;
            case 's': case KEY_DOWN:
                if (game_started) {
                    controller.userInput(UserAction::Down, true);
                }
                break;
            case 'a': case KEY_LEFT:
                if (game_started) {
                    controller.userInput(UserAction::Left, true);
                }
                break;
            case 'd': case KEY_RIGHT:
                if (game_started) {
                    controller.userInput(UserAction::Right, true);
                }
                break;
            case ' ': // Space for action - move forward
                if (game_started) {
                    controller.userInput(UserAction::Action, true);
                }
                break;
            case 'p': // P for pause
                if (game_started) {
                    controller.userInput(UserAction::Pause, true);
                }
                break;
            case 'r': // R for start/restart
                controller.userInput(UserAction::Start, true);
                game_started = true;
                break;
            case 'q': // Q for terminate
                controller.userInput(UserAction::Terminate, true);
                running = false;
                break;
            default:
                if (game_started) {
                    controller.userInput(UserAction::Action, false);
                }
        }

        info = controller.updateCurrentState();
        view.render(info);

        if (info.pause) {
            mvprintw(22, 2, "Game PAUSED - Press P to resume");
        } else if (game_started) {
            mvprintw(22, 2, "Score: %d - Use arrows to turn, space to move", info.score);
        }
        refresh();
    }

    view.cleanup();
}
#endif

#ifdef DESKTOP_UI

// Global variables for GTK callbacks
GameController* g_controller = nullptr;
GameViewGTK* g_view = nullptr;
bool g_game_started = false;
guint g_timer_id = 0;

gboolean game_update_callback(gpointer data) {
    if (g_controller && g_view && g_game_started) {
        GameInfo info = g_controller->updateCurrentState();
        g_view->render(info);
        


       g_controller->userInput(UserAction::Action, false);
        if (g_view->game_over) {
            g_view->showGameOver();
            g_timer_id = 0; // Stop timer
            return FALSE;
        }
    }
    return TRUE; // Continue timer
}

gboolean action_callback_(gpointer data) {
    if (g_controller && g_view && g_game_started) {
        GameInfo info = g_controller->updateCurrentState();
        g_view->render(info);
        
        if (g_view->game_over) {
            g_view->showGameOver();
            g_timer_id = 0; // Stop timer
            return FALSE;
        }
    }
    return TRUE; // Continue timer
}

gboolean on_key_press(GtkWidget* widget, GdkEventKey* event, gpointer data) {
    if (!g_controller) return FALSE;

    auto create_timer = [](guint speed_) -> guint {
      return g_timeout_add(speed_ * 20, game_update_callback, NULL);
    };

    switch (event->keyval) {
        case GDK_KEY_Up:
        case GDK_KEY_w:
        case GDK_KEY_W:
            if (g_game_started) {
                g_controller->userInput(UserAction::Up, true);
            }
            break;
            
        case GDK_KEY_Down:
        case GDK_KEY_s:
        case GDK_KEY_S:
            if (g_game_started) {
                g_controller->userInput(UserAction::Down, true);
            }
            break;
            
        case GDK_KEY_Left:
        case GDK_KEY_a:
        case GDK_KEY_A:
            if (g_game_started) {
                g_controller->userInput(UserAction::Left, true);
            }
            break;
            
        case GDK_KEY_Right:
        case GDK_KEY_d:
        case GDK_KEY_D:
            if (g_game_started) {
                g_controller->userInput(UserAction::Right, true);
            }
            break;
            
        case GDK_KEY_space:
            if (g_game_started) {
                g_controller->userInput(UserAction::Action, true);
                GameInfo info = g_controller->updateCurrentState();    
                if (g_timer_id) {
                  g_source_remove(g_timer_id);
                  g_timer_id = create_timer(info.speed);
                }
            }
            break;
            
        case GDK_KEY_p:
        case GDK_KEY_P:
            if (g_game_started) {
                g_controller->userInput(UserAction::Pause, true);
                GameInfo info = g_controller->updateCurrentState();
                if (info.pause) {
                    g_view->showPause();
                } else {
                    if (!g_timer_id) {
                        g_timer_id = create_timer(info.speed);
                    }
                }
            }
            break;
            
        case GDK_KEY_r:
        case GDK_KEY_R:
            g_controller->userInput(UserAction::Start, true);
            g_game_started = true;
            // TODO: useless?
            if (!g_timer_id) {
                GameInfo info = g_controller->updateCurrentState();
                g_timer_id = create_timer(info.speed);
            }
            break;
            
        case GDK_KEY_q:
        case GDK_KEY_Q:
            g_controller->userInput(UserAction::Terminate, true);
            if (g_timer_id) {
                g_source_remove(g_timer_id);
                g_timer_id = 0;
            }
            gtk_main_quit();
            break;
            
        default:
            std::cerr << "action\n";
            if (g_game_started) {
                g_controller->userInput(UserAction::Action, false);
            }
    }
    
    return TRUE;
}

void tetris_loop(void) {
    g_controller = new GameController();
    g_view = new GameViewGTK();
    
    int argc = 0;
    char** argv = nullptr;
    if (!g_view->initialize(argc, argv)) {
        std::cerr << "Failed to initialize GTK view!" << std::endl;
        delete g_view;
        delete g_controller;
        return;
    }
    
    g_signal_connect(g_view->window, "key-press-event", G_CALLBACK(on_key_press), NULL);
    
    GameInfo initial_info = g_controller->updateCurrentState();
    g_view->render(initial_info);
    
    gtk_main();
    
    if (g_timer_id) {
        g_source_remove(g_timer_id);
    }
    delete g_view;
    delete g_controller;
    g_view = nullptr;
    g_controller = nullptr;
}

#endif

int main(int argc, char** argv) {
    tetris_loop();
    return 0;
}
