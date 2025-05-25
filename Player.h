//
// Created by 차승준 on 25. 5. 25.
//
#ifndef DAVINCICODECPP_PLAYER_H
#define DAVINCICODECPP_PLAYER_H
#include "Card.h"

class Player {
private:
    std::vector<Card> hand;
    struct probData{
        std::string color;
        std::vector<int> values;
    };
    std::vector<probData> prob;
    std::vector<int> seqVec;
public:
    explicit Player();
    std::vector<Card>& getHand();
    void drawCard(const Card& card);
    int cardAmount();
    int shownCards();
    void initializeProb(const std::vector<Card>& hand);
    void updateProb(const std::vector<Card>& hand);
    void propagation();
    std::tuple<int, int> guessingAlgorithm(bool additional);
};

#endif
