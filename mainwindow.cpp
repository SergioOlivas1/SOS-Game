#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    // Create the game
    game = new Game(8, GameMode::SIMPLE);

    // Main widget and layout
    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    setCentralWidget(mainWidget);

    // Top section, game setup
    QHBoxLayout* setupRow = new QHBoxLayout();

    // Board size selector
    QLabel* sizeLabel = new QLabel("Board Size:");
    sizeBox = new QSpinBox();
    sizeBox->setRange(3, 10);
    sizeBox->setValue(8);
    setupRow->addWidget(sizeLabel);
    setupRow->addWidget(sizeBox);

    // Game mode selector
    QLabel* modeLabel = new QLabel("Mode:");
    simpleButton = new QRadioButton("Simple");
    generalButton = new QRadioButton("General");
    simpleButton->setChecked(true);
    setupRow->addWidget(modeLabel);
    setupRow->addWidget(simpleButton);
    setupRow->addWidget(generalButton);

    // New game button
    newGameButton = new QPushButton("New Game");
    connect(newGameButton, &QPushButton::clicked, this, &MainWindow::startNewGame);
    setupRow->addWidget(newGameButton);

    mainLayout->addLayout(setupRow);

    // Middle section with turn info
    turnLabel = new QLabel("Player 1's turn");
    turnLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    mainLayout->addWidget(turnLabel);

    // Leter section
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

    // Board
    boardLayout = new QGridLayout();
    mainLayout->addLayout(boardLayout);

    // Create the initial board
    createBoard();

    setWindowTitle("SOS Game");
    resize(500, 600);
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

    // Get board size
    int size = game->getBoard().getSize();
    boardButtons.resize(size, std::vector<QPushButton*>(size));

    // Create buttons
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            QPushButton* button = new QPushButton();
            button->setFixedSize(50, 50);
            button->setStyleSheet("font-size: 20px; font-weight: bold;");

            // Store row and col in the button
            button->setProperty("row", row);
            button->setProperty("col", col);

            // Connect button click
            connect(button, &QPushButton::clicked, this, &MainWindow::cellClicked);

            // Add to layout and store
            boardLayout->addWidget(button, row, col);
            boardButtons[row][col] = button;
        }
    }
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
                button->setEnabled(false);
            } else if (cell == CellState::O) {
                button->setText("O");
                button->setEnabled(false);
            } else {
                button->setText("");
                button->setEnabled(true);
            }
        }
    }

    // Update turn label
    Player* current = game->getCurrentPlayer();
    turnLabel->setText(QString::fromStdString(current->getName()) + "'s turn");
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
    if (game->makeMove(row, col)) {
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
    createBoard();
    updateBoard();
}
