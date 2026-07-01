CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Isrc -Iraylib/src
LDFLAGS = -Lraylib/build/raylib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lXcursor -lXinerama -lXrandr -lXi

SRC = main.cpp src/debug.cpp src/game.cpp src/input.cpp src/objects.cpp src/physics.cpp src/player.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
