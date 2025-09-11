# Compiler and linker
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

# Source and header files
SRCS_CLI = main.cpp \
       model/GameModel.cpp \
       controller/GameController.cpp \
       view/GameView.cpp

HEADERS_CLI = common/GameTypes.h \
          common/GameConstants.h \
          model/GameModel.h \
          controller/GameController.h \
          view/GameView.h

# Output executable
CLI = cli
DESKTOP = desktop

# Curses library flags
CURSES_LIBS = -lcurses
CURSES_INCLUDES = 

# Object files
OBJS_CLI = $(SRCS_CLI:.cpp=.o)

# Default target
all: $(CLI) $(DESKTOP)

# Link the executable
$(CLI): $(OBJS_CLI)
	$(CXX) $(CXXFLAGS) -DCLI_UI -o $@ $^ $(CURSES_LIBS)

# Compile source files
%.o: %.cpp $(HEADERS_CLI)
	$(CXX) $(CXXFLAGS) -DCLI_UI $(CURSES_INCLUDES) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS_CLI) $(TARGET)

.PHONY: all clean

