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
#include <QColor>
#include <QTimer>
#include <QButtonGroup>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QTextStream>
#include <vector>
#include <tuple>
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
    QPushButton* recordButton;
    QPushButton* loadRecordingButton;

    // Button groups
    QButtonGroup* gameModeGroup;
    QButtonGroup* letterGroup;
    QButtonGroup* player1TypeGroup;
    QButtonGroup* player2TypeGroup;

    // Player type selection
    QRadioButton* player1HumanButton;
    QRadioButton* player1ComputerButton;
    QRadioButton* player2HumanButton;
    QRadioButton* player2ComputerButton;

    QTimer* computerMoveTimer;
    QTimer* replayTimer;

    bool inReplayMode;
    std::vector<Game::MoveRecord> replayMoves;
    int replayIndex;
    QString replayMetadata;

    void createBoard();
    void updateBoard();
    void checkAndDrawSOS(int row, int col, Player* scorer);
    bool checkSOSLine(int r1, int c1, int r2, int c2, int r3, int c3);
    void handleComputerTurn();
    void saveRecordingToFile();
    void loadRecordingFromFile();
    void playNextReplayMove();

private slots:
    void cellClicked();
    void startNewGame();
    void makeComputerMove();
    void toggleRecording();
    void loadRecording();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};

#endif
