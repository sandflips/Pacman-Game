#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include "FoodManager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void movePacman();  // Slot to handle the timer
    void toggleMouth();
    bool checkCollision(int newX, int newY);
    void moveGhost();
    void move();
    bool checkCollisionX(int newX, int currentY);
    bool checkCollisionY(int newX, int currentY);
    bool isValidGhostPosition(int x, int y);
    void placeGhostsRandomly();
    bool checkGhostCollision(int X, int Y);
    void gameOver();
    void resetGame();
    void advanceLevel();
    void caught();

private:
    int x, y;           // Position of the square
    int squareSize,cellSize;     // Size of the square
    float dx, dy,pac_speed,ghost_speed;        // Direction of movement (dx, dy represent velocity in x and y axes)
    bool open;
    float incf;         //Speed increament factor(on pressing the same direction key multiple times
    int nog;            //this is number of ghosts
    QTimer *timer;      // Timer to move pacman continuously
    QTimer *timer1;     //Timer to open and close pacman's
    QTimer *timer2;     //Timer to move ghost
    int* ghostX;          // Ghost positions (X)
    int* ghostY;          // Ghost positions (Y)
    float* ghostDx;         // Ghost movement directions (X)
    float* ghostDy;
    int* ghostCooldown;
    FoodManager foodmanager;
    int scoreThreshold;
    int level,lives;
};

#endif // MAINWINDOW_H
