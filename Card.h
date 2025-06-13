//
// Created by 차승준 on 25. 5. 25.
//
#ifndef DAVINCICODECPP_CARD_H
#define DAVINCICODECPP_CARD_H
#include <iostream>

class Card {
public:
    std::string color;
    int number;
    bool shown = false;
    int seq;
    bool newlyDrawn = false;

    explicit Card(int seq);
    void reveal();
    void resetDrawn();
};

#endif
