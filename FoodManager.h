#ifndef FOODMANAGER_H
#define FOODMANAGER_H

#include <QPainter>
#include "Maze.h"

// Define the dimensions of the maze (or pass them as parameters if needed)

class FoodManager {
private:
    bool foodGrid[mazeWidth][mazeHeight]; // 2D grid to store food positions
    int score; // Score counter

public:
    FoodManager();

    // Initialize food positions
    void initializeFoodGrid();

    // Check if Pacman is on food and update score
    void checkForFood(int pacmanX, int pacmanY);

    // Draw food particles on the screen
    void drawFood(QPainter& painter, int gridSize);

    // Get the current score
    int getScore() const;
};

#endif // FOODMANAGER_H
