#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include <QWidget>
#include <QPainter>
#include <vector>
#include "game.h"

// Custom widget to draw the board with SOS lines
class BoardWidget : public QWidget {
    Q_OBJECT

private:
    std::vector<std::vector<QPushButton*>>* boardButtons;
    std::vector<std::tuple<int, int, int, int, QColor>>* sosLines;

protected:
    void paintEvent(QPaintEvent* event) override;

public:
    BoardWidget(QWidget* parent = nullptr);
    void setBoardButtons(std::vector<std::vector<QPushButton*>>* buttons);
    void setSOSLines(std::vector<std::tuple<int, int, int, int, QColor>>* lines);
};

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    Game* game;

    // Board buttons
    BoardWidget* boardWidget;
    QGridLayout* boardLayout;
    std::vector<std::vector<QPushButton*>> boardButtons;
    std::vector<std::tuple<int, int, int, int, QColor>> sosLines;

    // Controls
    QLabel* turnLabel;
    QRadioButton* sButton;
    QRadioButton* oButton;
    QSpinBox* sizeBox;
    QRadioButton* simpleButton;
    QRadioButton* generalButton;
    QPushButton* newGameButton;

    void createBoard();
    void updateBoard();
    void checkAndDrawSOS(int row, int col, Player* scorer);
    bool checkSOSLine(int r1, int c1, int r2, int c2, int r3, int c3);

private slots:
    void cellClicked();
    void startNewGame();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};

#endif
