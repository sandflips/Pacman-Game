#include "mainwindow.h"
#include <QMessageBox>
#include "Maze.h"
#include <algorithm>
#include <QPainterPath>

using std::max;
using std::min;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), x(0), y(0), squareSize(20), dx(1), dy(0), pac_speed(3),ghost_speed(2), open(true), incf(2), cellSize(30), nog(5), foodmanager(),
    scoreThreshold(2),level(1),lives(3)
{
    setFixedSize(1600, 700);
    ghostX= new int[nog];
    ghostY= new int[nog];
    ghostDx= new float[nog];
    ghostDy= new float[nog];
    ghostCooldown=new int[nog];
    placeGhostsRandomly();
    timer = new QTimer(this);
    timer1 = new QTimer(this);
    connect(timer1, &QTimer::timeout, this, &MainWindow::toggleMouth);
    timer1->start(150);  // Interval for Pacman mouth opening and closing
    connect(timer, &QTimer::timeout, this, &MainWindow::move);
    timer->start(15);  // Timer triggers every 15 ms for smooth movement of both pacman and ghosts
}

MainWindow::~MainWindow()
{
    delete[] ghostX;
    delete[] ghostY;
    delete[] ghostDx;
    delete[] ghostDy;
    delete[] ghostCooldown;
}

// Define the maze as a 2D array (1 = wall, 0 = open space)

bool MainWindow::isValidGhostPosition(int x, int y) {
    int gridX = x / cellSize;
    int gridY = y / cellSize;

    // Ensure the ghost is not placed on a wall
    if (currentMaze[gridY][gridX] != 0||(gridX<=6&&gridY<=8)) { // assuming 0 is an open space
        return false;
    }
    return true;
}

void MainWindow::placeGhostsRandomly() {
    for (int i = 0; i < nog; i++) {
        int randX, randY;
        do {
            randX = (rand() % mazeHeight) * cellSize;  // Use this->width()
            randY = (rand() % mazeWidth) * cellSize; // Use this->height()
        } while (!isValidGhostPosition(randX, randY)); // Keep trying until the ghost is placed in an open space

        ghostX[i] = randX;
        ghostY[i] = randY;
        int direction = rand() % 4;
        switch (direction) {
        case 0: ghostDx[i] = 1; ghostDy[i] = 0; break;  // Move right
        case 1: ghostDx[i] = -1; ghostDy[i] = 0; break; // Move left
        case 2: ghostDx[i] = 0; ghostDy[i] = 1; break;  // Move down
        case 3: ghostDx[i] = 0; ghostDy[i] = -1; break; // Move up
        }
        ghostCooldown[i] = rand() % 600 + 100;
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setPen(Qt::yellow); // Change the text color to a more vibrant one
    painter.setFont(QFont("Arial", 24, QFont::Bold)); // Increase font size and make it bold
    painter.drawText(1250, 200, "🎮 Score: " + QString::number(foodmanager.getScore())); // Add an emoji for style
    painter.drawText(1250, 270, "⭐ Level: " + QString::number(level)); // Adjust position for better spacing
    int heartSize = 20; // Size of each heart
    for (int i = 0; i < lives; ++i) {
        int x = 1250 + i * (heartSize + 10);
        int y = 50;

        QPainterPath heartPath;

        // Draw two circles for the top of the heart
        heartPath.addEllipse(x, y, heartSize / 2, heartSize / 2); // Left circle
        heartPath.addEllipse(x + heartSize / 2, y, heartSize / 2, heartSize / 2); // Right circle

        // Draw the triangle for the bottom of the heart
        QPointF topLeft(x, y + heartSize / 4);
        QPointF topRight(x + heartSize, y + heartSize / 4);
        QPointF bottom(x + heartSize / 2, y + heartSize);

        QPolygonF triangle;
        triangle << topLeft << topRight << bottom;
        heartPath.addPolygon(triangle);

        // Fill the heart shape with red
        painter.setBrush(Qt::red);
        painter.setPen(Qt::NoPen); // Optional: Remove border
        painter.drawPath(heartPath);
    }
    painter.setBrush(Qt::white);  // Color for maze walls
    painter.setPen(Qt::NoPen);

    for (int i = 0; i < mazeWidth; ++i) {
        for (int j = 0; j < mazeHeight; ++j) {
            if (currentMaze[i][j] == 1) {
                painter.drawRect(j * cellSize, i * cellSize, cellSize, cellSize);
            }
        }
    }
    foodmanager.drawFood(painter,cellSize);
    // Draw Pacman
    int startAngle, spanAngle;
    painter.setBrush(Qt::blue);   // Set color for Pacman
    QRect pacmanRect(x, y, squareSize, squareSize);  // Bounding box for Pacman
    QRect GhostRects[nog];
    for(int i=0;i<nog;i++){
        QRect a(ghostX[i],ghostY[i],squareSize, squareSize);
        GhostRects[i]=a;
    }

    if (open) {
        if (dx == 0) {  // Moving vertically
            if (dy > 0) {
                startAngle = 300 * 16;
                spanAngle = 300 * 16;
            } else {
                startAngle = 120 * 16;
                spanAngle = 300 * 16;
            }
        } else {  // Moving horizontally
            if (dx > 0) {
                startAngle = 30 * 16;
                spanAngle = 300 * 16;
            } else {
                startAngle = 210 * 16;
                spanAngle = 300 * 16;
            }
        }
    } else {
        startAngle = 0;
        spanAngle = 360 * 16;
    }

    painter.drawPie(pacmanRect, startAngle, spanAngle);
    painter.setBrush(Qt::red); //set colour of ghost
    for (int i = 0; i < nog; ++i) {
        painter.drawPie(GhostRects[i],startAngle, spanAngle);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        if (dx < 0) dx =-1* incf * pac_speed;
        else dx = -1 * pac_speed;
        dy = 0;
        break;
    case Qt::Key_Right:
        if (dx > 0) dx = incf * pac_speed;
        else dx = pac_speed;
        dy = 0;
        break;
    case Qt::Key_Up:
        dx = 0;
        if (dy < 0) dy = -1*incf * pac_speed;
        else dy = -1 * pac_speed;
        break;
    case Qt::Key_Down:
        dx = 0;
        if (dy > 0) dy = incf * pac_speed;
        else dy = pac_speed;
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::toggleMouth()
{
    open = !open;  // Toggle the mouth state (open/closed)
}

bool MainWindow::checkCollision(int newX, int newY)
{
    // Convert Pacman's position to the grid coordinates
    int topLeftX = newX / cellSize;
    int topLeftY = newY / cellSize;
    int topRightX = (newX + squareSize - 1) / cellSize;
    int bottomRightY = (newY + squareSize - 1) / cellSize;

    // Ensure that Pacman fits within the grid and check collisions for all corners
    if (currentMaze[topLeftY][topLeftX] == 1 ||  // Top-left corner
        currentMaze[topLeftY][topRightX] == 1 || // Top-right corner
        currentMaze[bottomRightY][topLeftX] == 1 || // Bottom-left corner
        currentMaze[bottomRightY][topRightX] == 1)  // Bottom-right corner
    {
        return true;  // Collision detected
    }

    return false;  // No collision
}
bool MainWindow::checkCollisionX(int newX, int currentY)
{
    // Convert Pacman's new X position and current Y position to grid coordinates
    int topLeftX = newX / cellSize;
    int topRightX = (newX + squareSize - 1) / cellSize;
    int topLeftY = currentY / cellSize;
    int bottomRightY = (currentY + squareSize - 1) / cellSize;

    // Check collisions for top-left and bottom-left corners (X movement only)
    if (currentMaze[topLeftY][topLeftX] == 1 || // Top-left corner
        currentMaze[bottomRightY][topLeftX] == 1 || // Bottom-left corner
        currentMaze[topLeftY][topRightX] == 1 || // Top-right corner
        currentMaze[bottomRightY][topRightX] == 1) // Bottom-right corner
    {
        return true;  // Collision detected in the X direction
    }

    return false;  // No collision in the X direction
}
bool MainWindow::checkCollisionY(int currentX, int newY)
{
    // Convert Pacman's current X position and new Y position to grid coordinates
    int topLeftX = currentX / cellSize;
    int topLeftY = newY / cellSize;
    int bottomRightY = (newY + squareSize - 1) / cellSize;
    int topRightX = (currentX + squareSize - 1) / cellSize;

    // Check collisions for top-left and top-right corners (Y movement only)
    if (currentMaze[topLeftY][topLeftX] == 1 ||  // Top-left corner
        currentMaze[topLeftY][topRightX] == 1 ||  // Top-right corner
        currentMaze[bottomRightY][topLeftX] == 1 || // Bottom-left corner
        currentMaze[bottomRightY][topRightX] == 1) // Bottom-right corner
    {
        return true;  // Collision detected in the Y direction
    }

    return false;  // No collision in the Y direction
}

bool MainWindow::checkGhostCollision(int pacmanX, int pacmanY) {
    int range=cellSize/15;
    int xmin=max(0,pacmanX-range);
    int xmax=min(mazeHeight*cellSize,pacmanX+range);
    int ymin=max(0,pacmanY-range);
    int ymax=min(mazeWidth*cellSize,pacmanY+range);
    for (int i = 0; i < nog; i++) {
        if ((ghostX[i]>=xmin&&ghostX[i]<=xmax) && (ghostY[i]>=ymin&&ghostY[i]<=ymax)) {
            return true;  // Collision detected, game over
        }
    }
    return false;  // No collision, continue the game
}
void MainWindow::caught() {
    lives--;
    if (lives > 0) {
        // Reset Pacman and ghost positions, allowing the game to continue
        x = 0;  // Reset Pacman's position
        y = 0;
        placeGhostsRandomly();  // Reset ghost positions
        foodmanager.initializeFoodGrid();  // Reset the food grid if needed
        update();  // Trigger repaint
    } else {
        // If no lives are left, game over
        gameOver();
    }
}


void MainWindow::movePacman()
{
    int newX = x + dx;
    int newY = y + dy;

    if (!checkCollision(newX, newY)) {
        x = newX;
        y = newY;
    }

    x = qMax(0, qMin(x, mazeHeight * cellSize - squareSize));
    y = qMax(0, qMin(y, mazeWidth * cellSize - squareSize));

    if (checkGhostCollision(x, y)) {
        caught();
        return;
    }

    int px = x / cellSize;
    int py = y / cellSize;
    foodmanager.checkForFood(py, px);

    if (foodmanager.getScore() >= scoreThreshold) {
        advanceLevel();
    }
}

void MainWindow::advanceLevel()
{
    level++;
    scoreThreshold += 10; // Increase threshold for the next level

    if (level == 2) {
        currentMaze = maze2; // Switch to the new maze
    } else {
        // Additional levels can be added here
        QMessageBox::information(this, "Congratulations", "You completed the game!");
        exit(0);
    }

    // Reinitialize food and ghosts
    foodmanager.initializeFoodGrid();
    placeGhostsRandomly();

    // Reset Pacman's position
    x = y = 0;
}
void MainWindow::gameOver() {
    // Show a Game Over dialog with the option to restart or quit
    int ret = QMessageBox::question(this, "Game Over", "Pacman was caught by a ghost! Restart?",
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        resetGame();  // Reset game state and restart
    } else {
        close();  // Close the game window
    }
}


void MainWindow::resetGame() {
    lives = 3;  // Reset lives
    level = 1;  // Reset level
    scoreThreshold = 2;  // Reset score threshold
    x = y = 0;  // Reset Pacman position
    placeGhostsRandomly();  // Reset ghost positions
    foodmanager = FoodManager();  // Reset food manager (optional)
    foodmanager.initializeFoodGrid();  // Reinitialize food grid if needed
    update();  // Trigger repaint to show reset state
}



#include <cstdlib>  // For rand()

void MainWindow::moveGhost() {
    for (int i = 0; i < nog; ++i) {
        // Random movement until the ghost is close to Pacman
        int detectionRadius = 40 * cellSize;  // Range within which the ghost starts chasing Pacman
        bool chasingPacman = (abs(ghostX[i] - x) < detectionRadius) && (abs(ghostY[i] - y) < detectionRadius);

        if (chasingPacman) {
            // Ghost chases Pacman
            if (ghostX[i] < x)
                ghostDx[i] = ghost_speed;  // Ghost moves right
            else if (ghostX[i] > x)
                ghostDx[i] = -ghost_speed/2;  // Ghost moves left
            else
                ghostDx[i] = 0;

            if (ghostY[i] < y)
                ghostDy[i] = ghost_speed/2;  // Ghost moves down
            else if (ghostY[i] > y)
                ghostDy[i] = -ghost_speed/2;  // Ghost moves up
            else
                ghostDy[i] = 0;
        } else {
            if (ghostCooldown[i] == 0) {
                // If cooldown is over, choose a random direction
                ghostDx[i] = (rand() % 3 - 1) * ghost_speed;  // Move randomly in X direction (-1, 0, or 1)
                ghostDy[i] = (rand() % 3 - 1) * ghost_speed;  // Move randomly in Y direction (-1, 0, or 1)

                // Set a new cooldown before the ghost can change direction again
                ghostCooldown[i] = rand() % 600 + 100;  // Random cooldown between 10 and 30 moves
            } else {
                // Decrease cooldown on each frame
                ghostCooldown[i]--;
            }
        }

        // Check for collisions and move ghosts accordingly
        int newX = ghostX[i] + ghostDx[i];
        int newY = ghostY[i] + ghostDy[i];

        if (!checkCollisionX(newX, ghostY[i])) {
            ghostX[i] = newX;
        }
        if (!checkCollisionY(ghostX[i], newY)) {
            ghostY[i] = newY;
        }

        // Ensure ghosts stay within the window boundaries
        ghostX[i] = qMax(0, qMin(ghostX[i], mazeHeight*cellSize - squareSize));
        ghostY[i] = qMax(0, qMin(ghostY[i], mazeWidth*cellSize - squareSize));
    }
}



void MainWindow::move(){
    movePacman();
    moveGhost();
    update();
}



