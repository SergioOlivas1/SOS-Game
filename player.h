#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "enums.h"

class Player {
private:
    std::string name;
    PlayerType type;
    int score;
    char currentLetter; // 'S' or 'O'

public:
    Player(const std::string& playerName, PlayerType playerType);

    std::string getName() const;
    PlayerType getType() const;
    int getScore() const;
    char getCurrentLetter() const;

    void setCurrentLetter(char letter);
    void addScore(int points);
    void resetScore();
};

#endif
