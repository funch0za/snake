cmake_minimum_required(VERSION 4.0)
project(snake)

set(CMAKE_CXX_STANDARD 20)

add_executable(snake main.cpp
        common/GameTypes.h
        common/GameConstants.h
        model/GameModel.cpp
        model/GameModel.h
        controller/GameController.cpp
        controller/GameController.h
        view/GameView.cpp
        view/GameView.h
)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(snake ${CURSES_LIBRARIES})

