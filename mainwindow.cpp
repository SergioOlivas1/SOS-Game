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
    : QMainWindow(parent), inReplayMode(false), replayIndex(0){

    // Create the game
    game = new Game(8, GameMode::SIMPLE);
    computerMoveTimer = new QTimer(this);
    replayTimer = new QTimer(this);
    connect(computerMoveTimer, &QTimer::timeout, this, &MainWindow::makeComputerMove);
    connect(replayTimer, &QTimer::timeout, this, &MainWindow::playNextReplayMove);

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

    gameModeGroup = new QButtonGroup(this);
    gameModeGroup->addButton(simpleButton);
    gameModeGroup->addButton(generalButton);

    setupRow->addWidget(modeLabel);
    setupRow->addWidget(simpleButton);
    setupRow->addWidget(generalButton);

    newGameButton = new QPushButton("New Game");
    connect(newGameButton, &QPushButton::clicked, this, &MainWindow::startNewGame);
    setupRow->addWidget(newGameButton);

    mainLayout->addLayout(setupRow);

    //recording controls
    QHBoxLayout* recordingRow = new QHBoxLayout();

    recordButton = new QPushButton("Start Recording");
    connect(recordButton, &QPushButton::clicked, this, &MainWindow::toggleRecording);
    recordingRow->addWidget(recordButton);

    loadRecordingButton = new QPushButton("Load Recording");
    connect(loadRecordingButton, &QPushButton::clicked, this, &MainWindow::loadRecording);
    recordingRow->addWidget(loadRecordingButton);

    recordingRow->addStretch();
    mainLayout->addLayout(recordingRow);

    // Player selection type
    QHBoxLayout* playerTypeRow = new QHBoxLayout();

    // Player 1
    QLabel* p1Label = new QLabel("Player 1 (Blue):");
    player1HumanButton = new QRadioButton("Human");
    player1ComputerButton = new QRadioButton("Computer");
    player1HumanButton->setChecked(true);

    player1TypeGroup = new QButtonGroup(this);
    player1TypeGroup->addButton(player1HumanButton);
    player1TypeGroup->addButton(player1ComputerButton);

    playerTypeRow->addWidget(p1Label);
    playerTypeRow->addWidget(player1HumanButton);
    playerTypeRow->addWidget(player1ComputerButton);

    playerTypeRow->addSpacing(20);

    //Player 2
    QLabel* p2Label = new QLabel("Player 2 (Red):");
    player2HumanButton = new QRadioButton("Human");
    player2ComputerButton = new QRadioButton("Computer");
    player2HumanButton->setChecked(true);

    player2TypeGroup = new QButtonGroup(this);
    player2TypeGroup->addButton(player2HumanButton);
    player2TypeGroup->addButton(player2ComputerButton);

    playerTypeRow->addWidget(p2Label);
    playerTypeRow->addWidget(player2HumanButton);
    playerTypeRow->addWidget(player2ComputerButton);

    playerTypeRow->addStretch();
    mainLayout->addLayout(playerTypeRow);

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
    resize(600, 800);
}

MainWindow::~MainWindow() {
    delete game;
    delete computerMoveTimer;
    delete replayTimer;
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

    // Only allow human players to click
    Player* current = game->getCurrentPlayer();
    if (current->getType() == PlayerType::AI){
        return; // Ignore clicks during computer turn
    }

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

        //After human move, check if next player is computer
        handleComputerTurn();
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

    if (inReplayMode) {
        inReplayMode = false;
        replayTimer->stop();
    }

    GameMode mode = simpleButton->isChecked() ? GameMode::SIMPLE : GameMode::GENERAL;

    //Set players based on button selections
    PlayerType p1Type = player1HumanButton->isChecked() ? PlayerType::HUMAN : PlayerType::AI;
    PlayerType p2Type = player2HumanButton->isChecked() ? PlayerType::HUMAN : PlayerType::AI;

    // Start new game
    game->newGame(size, mode);
    game->setupPlayers("Player 1", p1Type, "Player 2", p2Type);

    recordButton->setText("Start Recording");
    recordButton->setEnabled(true);

    sButton->setChecked(true);
    oButton->setChecked(false);

    createBoard();
    updateBoard();

    // If player 1 is computer, statrt computer move
    handleComputerTurn();
}

void MainWindow::handleComputerTurn(){
    if (game->getState() != GameState::ONGOING) {
        return;
    }

    Player* current = game->getCurrentPlayer();
    if (current && current->getType() == PlayerType::AI) {
        // Disable board for computer turn
        for (int i = 0; i < boardButtons.size(); i++) {
            for (int j = 0; j < boardButtons[i].size(); j++) {
                boardButtons[i][j]->setEnabled(false);
            }
        }

        // Start timer for computer move (1 second delay)
        computerMoveTimer->start(1000);
    } else {
        // Human turn, enable board
        updateBoard();
    }
}

void MainWindow::makeComputerMove(){
    computerMoveTimer->stop();

    if (game->getState() != GameState::ONGOING){
        return;
    }

    Player* movingPlayer = game->getCurrentPlayer();
    int row, col;

    if (game->makeComputerMove(row, col)){
        checkAndDrawSOS(row, col, movingPlayer);
        updateBoard();

        handleComputerTurn();
    }
}

void MainWindow::toggleRecording() {
    if (inReplayMode) {
        QMessageBox::warning(this, "Replay Mode", "Cannot record during replay!");
        return;
    }

    if (game->isRecording()) {
        // Stop recording and save
        game->stopRecording();
        recordButton->setText("Start Recording");
        saveRecordingToFile();
    } else {
        // Start recording
        game->startRecording();
        recordButton->setText("Stop Recording");
    }
}

void MainWindow::saveRecordingToFile() {
    std::vector<Game::MoveRecord> moves = game->getRecordedMoves();

    if (moves.empty()) {
        QMessageBox::information(this, "No Moves", "No moves to save!");
        return;
    }

    // Generate default filename with timestamp
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
    QString defaultName = "SOS_Game_" + timestamp + ".txt";

    QString filename = QFileDialog::getSaveFileName(this, "Save Recording",
                                                    defaultName, "Text Files (*.txt)");

    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not save file!");
        return;
    }

    QTextStream out(&file);

    // Write metadata
    out << "# SOS Game Recording\n";
    out << "# Date: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
    out << "# Board Size: " << game->getBoardSize() << "\n";
    out << "# Game Mode: " << (game->getMode() == GameMode::SIMPLE ? "Simple" : "General") << "\n";
    out << "# Player 1: " << QString::fromStdString(game->getPlayer1()->getName())
        << " (" << (game->getPlayer1()->getType() == PlayerType::HUMAN ? "Human" : "Computer") << ")\n";
    out << "# Player 2: " << QString::fromStdString(game->getPlayer2()->getName())
        << " (" << (game->getPlayer2()->getType() == PlayerType::HUMAN ? "Human" : "Computer") << ")\n";
    out << "#\n";

    // Write moves
    for (const auto& move : moves) {
        out << "MOVE:" << move.moveNumber << ":"
            << QString::fromStdString(move.playerName) << ":"
            << move.row << ":" << move.col << ":" << move.letter << "\n";
    }

    // Write result
    GameState state = game->getState();
    if (state == GameState::PLAYER1_WIN) {
        out << "RESULT:Player 1 Wins\n";
    } else if (state == GameState::PLAYER2_WIN) {
        out << "RESULT:Player 2 Wins\n";
    } else if (state == GameState::DRAW) {
        out << "RESULT:Draw\n";
    }

    out << "FINAL_SCORE:" << game->getPlayer1()->getScore() << ":"
        << game->getPlayer2()->getScore() << "\n";

    file.close();

    QMessageBox::information(this, "Success", "Recording saved successfully!");
}

void MainWindow::loadRecording() {
    QString filename = QFileDialog::getOpenFileName(this, "Load Recording",
                                                    "", "Text Files (*.txt)");

    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open file!");
        return;
    }

    QTextStream in(&file);

    // Parse file
    int boardSize = 8;
    GameMode mode = GameMode::SIMPLE;
    replayMoves.clear();
    replayMetadata = "";

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (line.startsWith("#")) {
            // Metadata
            replayMetadata += line + "\n";

            if (line.contains("Board Size:")) {
                boardSize = line.split(":").last().trimmed().toInt();
            } else if (line.contains("Game Mode:")) {
                mode = line.contains("Simple") ? GameMode::SIMPLE : GameMode::GENERAL;
            }
        } else if (line.startsWith("MOVE:")) {
            // Parse move: MOVE:1:Player 1:0:0:S
            QStringList parts = line.split(":");
            if (parts.size() == 6) {
                Game::MoveRecord move;
                move.moveNumber = parts[1].toInt();
                move.playerName = parts[2].toStdString();
                move.row = parts[3].toInt();
                move.col = parts[4].toInt();
                move.letter = parts[5].at(0).toLatin1();
                replayMoves.push_back(move);
            }
        }
    }

    file.close();

    if (replayMoves.empty()) {
        QMessageBox::warning(this, "Invalid File", "No valid moves found in file!");
        return;
    }

    // Start replay
    inReplayMode = true;
    replayIndex = 0;

    // Setup game for replay
    game->newGame(boardSize, mode);
    game->setupPlayers("Player 1", PlayerType::HUMAN, "Player 2", PlayerType::HUMAN);

    createBoard();
    updateBoard();

    // Disable controls during replay
    recordButton->setEnabled(false);
    newGameButton->setEnabled(false);

    turnLabel->setText("Replaying game... Move 0 of " + QString::number(replayMoves.size()));

    // Start replay timer
    replayTimer->start(1000);  // 1 second delay between moves
}

void MainWindow::playNextReplayMove() {
    if (replayIndex >= replayMoves.size()) {
        // Replay finished
        replayTimer->stop();
        turnLabel->setText("Replay complete!");
        recordButton->setEnabled(true);
        newGameButton->setEnabled(true);
        QMessageBox::information(this, "Replay Complete", "Game replay finished!");
        return;
    }

    const Game::MoveRecord& move = replayMoves[replayIndex];

    // Set the appropriate player and letter
    Player* player = (move.playerName == "Player 1") ? game->getPlayer1() : game->getPlayer2();
    player->setCurrentLetter(move.letter);

    // Make sure it's the right player's turn
    while (game->getCurrentPlayer() != player) {
        game->switchPlayer();
    }

    Player* movingPlayer = game->getCurrentPlayer();

    // Make the move
    if (game->makeMove(move.row, move.col)) {
        checkAndDrawSOS(move.row, move.col, movingPlayer);
        updateBoard();
    }

    replayIndex++;
    turnLabel->setText("Replaying game... Move " + QString::number(replayIndex) +
                       " of " + QString::number(replayMoves.size()));
}
