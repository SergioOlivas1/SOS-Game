#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include <vector>
#include "game.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    Game* game;

    // Board buttons
    QGridLayout* boardLayout;
    std::vector<std::vector<QPushButton*>> boardButtons;

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

private slots:
    void cellClicked();
    void startNewGame();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};

#endif
