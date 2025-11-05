#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QGroupBox>
#include <QPainter>
#include <QPen>

// BoardWidget Implementation
BoardWidget::BoardWidget(QWidget* parent) : QWidget(parent), boardButtons(nullptr), sosLines(nullptr) {}

void BoardWidget::setBoardButtons(std::vector<std::vector<QPushButton*>>* buttons) {
    boardButtons = buttons;
}

void BoardWidget::setSOSLines(std::vector<std::tuple<int, int, int, int, QColor>>* lines) {
    sosLines = lines;
}

void BoardWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);

    if (!sosLines || !boardButtons || boardButtons->empty()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw all SOS lines
    for (const auto& line : *sosLines) {
        int r1 = std::get<0>(line);
        int c1 = std::get<1>(line);
        int r2 = std::get<2>(line);
        int c2 = std::get<3>(line);
        QColor color = std::get<4>(line);

        if (r1 >= boardButtons->size() || c1 >= (*boardButtons)[0].size() ||
            r2 >= boardButtons->size() || c2 >= (*boardButtons)[0].size()) {
            continue;
        }

        QPushButton* startBtn = (*boardButtons)[r1][c1];
        QPushButton* endBtn = (*boardButtons)[r2][c2];

        QPoint start = startBtn->mapTo(this, startBtn->rect().center());
        QPoint end = endBtn->mapTo(this, endBtn->rect().center());

        QPen pen(color);
        pen.setWidth(4);
        painter.setPen(pen);
        painter.drawLine(start, end);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    // Create the game
    game = new Game(8, GameMode::SIMPLE);

    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    setCentralWidget(mainWidget);

    // Top Section
    QHBoxLayout* setupRow = new QHBoxLayout();

    QLabel* sizeLabel = new QLabel("Board Size:");
    sizeBox = new QSpinBox();
    sizeBox->setRange(3, 10);
    sizeBox->setValue(8);
    setupRow->addWidget(sizeLabel);
    setupRow->addWidget(sizeBox);

    QLabel* modeLabel = new QLabel("Mode:");
    simpleButton = new QRadioButton("Simple");
    generalButton = new QRadioButton("General");
    simpleButton->setChecked(true);
    setupRow->addWidget(modeLabel);
    setupRow->addWidget(simpleButton);
    setupRow->addWidget(generalButton);

    newGameButton = new QPushButton("New Game");
    connect(newGameButton, &QPushButton::clicked, this, &MainWindow::startNewGame);
    setupRow->addWidget(newGameButton);

    mainLayout->addLayout(setupRow);

    // Turn info
    turnLabel = new QLabel("Player 1 (Blue)'s turn");
    turnLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    mainLayout->addWidget(turnLabel);

    // Letter selection
    QHBoxLayout* letterRow = new QHBoxLayout();
    QLabel* letterLabel = new QLabel("Choose letter:");
    sButton = new QRadioButton("S");
    oButton = new QRadioButton("O");
    sButton->setChecked(true);
    letterRow->addWidget(letterLabel);
    letterRow->addWidget(sButton);
    letterRow->addWidget(oButton);
    letterRow->addStretch();
    mainLayout->addLayout(letterRow);

    // Board widget
    boardWidget = new BoardWidget();
    boardWidget->setBoardButtons(&boardButtons);
    boardWidget->setSOSLines(&sosLines);

    boardLayout = new QGridLayout();
    boardWidget->setLayout(boardLayout);
    mainLayout->addWidget(boardWidget);

    createBoard();

    setWindowTitle("SOS Game");
    resize(600, 700);
}

MainWindow::~MainWindow() {
    delete game;
}

void MainWindow::createBoard() {
    // Clear old board
    while (QLayoutItem* item = boardLayout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
    boardButtons.clear();
    sosLines.clear();

    // Get board size
    int size = game->getBoard().getSize();
    boardButtons.resize(size, std::vector<QPushButton*>(size));

    // Create buttons
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            QPushButton* button = new QPushButton();
            button->setFixedSize(60, 60);
            button->setStyleSheet("font-size: 24px; font-weight: bold; background-color: white;");

            button->setProperty("row", row);
            button->setProperty("col", col);

            connect(button, &QPushButton::clicked, this, &MainWindow::cellClicked);

            boardLayout->addWidget(button, row, col);
            boardButtons[row][col] = button;
        }
    }

    boardWidget->update();
}

bool MainWindow::checkSOSLine(int r1, int c1, int r2, int c2, int r3, int c3) {
    const Board& board = game->getBoard();
    int size = board.getSize();

    if (r1 < 0 || r1 >= size || c1 < 0 || c1 >= size ||
        r2 < 0 || r2 >= size || c2 < 0 || c2 >= size ||
        r3 < 0 || r3 >= size || c3 < 0 || c3 >= size) {
        return false;
    }

    return (board.getCell(r1, c1) == CellState::S &&
            board.getCell(r2, c2) == CellState::O &&
            board.getCell(r3, c3) == CellState::S);
}

void MainWindow::checkAndDrawSOS(int row, int col, Player* scorer) {
    const Board& board = game->getBoard();
    CellState placed = board.getCell(row, col);

    QColor lineColor = (scorer == game->getPlayer1()) ?
                           QColor(0, 100, 255) : QColor(255, 50, 50); // Blue for P1, Red for P2

    // 8 directions
    int directions[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
        {0, 1}, {1, -1}, {1, 0}, {1, 1}
    };

    if (placed == CellState::S) {
        // Check if S is start of SOS
        for (auto& dir : directions) {
            int r2 = row + dir[0], c2 = col + dir[1];
            int r3 = row + 2*dir[0], c3 = col + 2*dir[1];
            if (checkSOSLine(row, col, r2, c2, r3, c3)) {
                sosLines.push_back(std::make_tuple(row, col, r3, c3, lineColor));
            }
        }
        // Check if S is end of SOS
        for (auto& dir : directions) {
            int r2 = row - dir[0], c2 = col - dir[1];
            int r3 = row - 2*dir[0], c3 = col - 2*dir[1];
            if (checkSOSLine(r3, c3, r2, c2, row, col)) {
                sosLines.push_back(std::make_tuple(r3, c3, row, col, lineColor));
            }
        }
    } else if (placed == CellState::O) {
        // Check if O is middle of SOS
        for (auto& dir : directions) {
            int r1 = row - dir[0], c1 = col - dir[1];
            int r3 = row + dir[0], c3 = col + dir[1];
            if (checkSOSLine(r1, c1, row, col, r3, c3)) {
                sosLines.push_back(std::make_tuple(r1, c1, r3, c3, lineColor));
            }
        }
    }

    boardWidget->update();
}

void MainWindow::updateBoard() {
    const Board& board = game->getBoard();
    int size = board.getSize();

    // Update each button
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            CellState cell = board.getCell(row, col);
            QPushButton* button = boardButtons[row][col];

            if (cell == CellState::S) {
                button->setText("S");
                button->setStyleSheet("font-size: 24px; font-weight: bold; "
                                      "background-color: #333; color: white;");
                button->setEnabled(false);
            } else if (cell == CellState::O) {
                button->setText("O");
                button->setStyleSheet("font-size: 24px; font-weight: bold; "
                                      "background-color: #333; color: white;");
                button->setEnabled(false);
            } else {
                button->setText("");
                button->setStyleSheet("font-size: 24px; font-weight: bold; "
                                      "background-color: white;");
                button->setEnabled(game->getState() == GameState::ONGOING);
            }
        }
    }

    // Update status
    Player* p1 = game->getPlayer1();
    Player* p2 = game->getPlayer2();

    QString statusText;
    if (game->getState() == GameState::ONGOING) {
        QString currentName = (game->getCurrentPlayer() == p1) ?
                                  "Player 1 (Blue)" : "Player 2 (Red)";
        statusText = currentName + "'s turn | ";
        statusText += "Blue: " + QString::number(p1->getScore()) + " | ";
        statusText += "Red: " + QString::number(p2->getScore());
        turnLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    } else if (game->getState() == GameState::PLAYER1_WIN) {
        statusText = "PLAYER 1 (BLUE) WINS! | Final: " +
                     QString::number(p1->getScore()) + " - " + QString::number(p2->getScore());
        turnLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: blue;");
    } else if (game->getState() == GameState::PLAYER2_WIN) {
        statusText = "PLAYER 2 (RED) WINS! | Final: " +
                     QString::number(p1->getScore()) + " - " + QString::number(p2->getScore());
        turnLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: red;");
    } else if (game->getState() == GameState::DRAW) {
        statusText = "DRAW! | Final: " + QString::number(p1->getScore()) + " - " + QString::number(p2->getScore());
        turnLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: orange;");
    }

    turnLabel->setText(statusText);
    boardWidget->update();
}

void MainWindow::cellClicked() {
    // Get which button was clicked
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    // Get row and col from button
    int row = button->property("row").toInt();
    int col = button->property("col").toInt();

    // Get selected letter
    char letter = sButton->isChecked() ? 'S' : 'O';
    game->getCurrentPlayer()->setCurrentLetter(letter);

    // Make the move
    Player* movingPlayer = game->getCurrentPlayer();

    if (game->makeMove(row, col)) {
        checkAndDrawSOS(row, col, movingPlayer);
        updateBoard();
    }
}

void MainWindow::startNewGame() {
    // Get settings
    int size = sizeBox->value();

    // Validate size
    if (size < 3) {
        QMessageBox::warning(this, "Invalid Size", "Board size must be at least 3!");
        return;
    }

    GameMode mode = simpleButton->isChecked() ? GameMode::SIMPLE : GameMode::GENERAL;

    // Start new game
    game->newGame(size, mode);

    sButton->setChecked(true);
    oButton->setChecked(false);

    createBoard();
    updateBoard();
}
