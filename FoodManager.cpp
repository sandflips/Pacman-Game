#include "FoodManager.h"
#include "Maze.h"
FoodManager::FoodManager() : score(0) {
    initializeFoodGrid();
}

void FoodManager::initializeFoodGrid() {
    // First, clear the food grid
    for (int i = 0; i < mazeWidth; ++i) {
        for (int j = 0; j < mazeHeight; ++j) {
            foodGrid[i][j] = false; // No food initially
        }
    }

    // Assuming maze[i][j] is a valid maze structure, where 0 represents an empty space
    // and 1 represents a wall.
    for (int i = 0; i < mazeWidth; ++i) {
        for (int j = 0; j < mazeHeight; ++j) {
            // Ensure maze position is valid and not a wall (1)
            if (currentMaze[i][j] == 0 && !foodGrid[i][j]) { // Check if the cell is not a wall and not already occupied
                // Randomly decide if a food should be placed here (density control)
                if (rand() % 15 < 3) {  // Adjust the number (3) to control the density of food
                    foodGrid[i][j] = true;
                }
            }
        }
    }
    foodGrid[2][0]=true;
}



void FoodManager::checkForFood(int pacmanX, int pacmanY) {
    if (foodGrid[pacmanX][pacmanY]) {
        foodGrid[pacmanX][pacmanY] = false; // Pacman eats the food
        score++; // Increment score
    }
}

void FoodManager::drawFood(QPainter& painter, int gridSize) {
    for (int i = 0; i < mazeWidth; ++i) {
        for (int j = 0; j < mazeHeight; ++j) {
            if (foodGrid[i][j]) {
                painter.setBrush(Qt::yellow);
                painter.drawEllipse(j * gridSize+gridSize/2, i * gridSize+gridSize/2, gridSize/5, gridSize/5);
            }
        }
    }
}

int FoodManager::getScore() const {
    return score;
}
