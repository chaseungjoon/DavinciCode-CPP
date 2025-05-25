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
    bool shown;
    int seq;  // seq -> sequence of card in deck

    explicit Card(int seq);
    void reveal();
};

#endif
