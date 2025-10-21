#include "player.h"

Player::Player(const std::string& playerName, PlayerType playerType)
    : name(playerName), type(playerType), score(0), currentLetter('S') {}

std::string Player::getName() const {
    return name;
}

PlayerType Player::getType() const {
    return type;
}

int Player::getScore() const {
    return score;
}

char Player::getCurrentLetter() const {
    return currentLetter;
}

void Player::setCurrentLetter(char letter) {
    currentLetter = letter;
}

void Player::addScore(int points) {
    score += points;
}

void Player::resetScore() {
    score = 0;
}
