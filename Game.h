//
// Created by 차승준 on 25. 5. 25.
//
#ifndef DAVINCICODECPP_GAME_H
#define DAVINCICODECPP_GAME_H
#include "Card.h"
#include "Player.h"

class Game {
private:
    std::vector<Card> deck;
    Player human;
    Player computer;
public:
    Game();
    void initialize();
    void runGameLoop();
    void printVisual();
    void playerTurn();
    void computerTurn();
    bool gameOverCheck();

};

#endif
