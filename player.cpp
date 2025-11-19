#include "player.h"
#include <cstdlib>
#include <ctime>

Player::Player(const std::string& playerName, PlayerType playerType)
    : name(playerName), type(playerType), score(0), currentLetter('S') {
    // seed random number generator
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned int> (time(nullptr)));
        seeded = true;
    }
}

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

char Player::chooseRandomLetter(){
    return (rand() % 2 == 0) ? 'S' : 'O';
}
