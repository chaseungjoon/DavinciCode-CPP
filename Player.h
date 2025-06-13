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
    std::vector<int> blackValVec;
    std::vector<int> whiteValVec;

    bool debugMode = true;

public:
    explicit Player();
    std::vector<Card>& getHand();
    void drawCard(const Card& card);
    static bool contains(const std::vector<int>& vec, int target);
    int cardAmount();
    int shownCards();
    void initializeProb(const std::vector<Card>& hand);
    void updateProb(std::vector<Card>& hand);
    void adjustProb(const std::vector<Card>& hand);
    void deleteFromProb(int probIdx, int target);
    std::tuple<int, int> guessingAlgorithm(const std::vector<Card>& hand, bool additional);
    void debugScreen(const std::vector<Card>& hand);
};

#endif
