//
// Created by 차승준 on 25. 5. 25.
//
#include "Card.h"

Card::Card(int seq) {
    if (seq%2==0) this->color = "black";
    else this->color = "white";

    this->number = seq/2;
    this->shown = false;
    this->seq = seq;
}

void Card::reveal() {shown = true;}

