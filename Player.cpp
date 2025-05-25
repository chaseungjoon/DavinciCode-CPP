//
// Created by 차승준 on 25. 5. 25.
//
#include "Player.h"
#include <random>

Player::Player() {
    this->hand.clear();
    this->prob.clear();
}

std::vector<Card>& Player::getHand() {
    return hand;
}

void Player::drawCard(const Card& card) {
    hand.push_back(card);
    std::sort(hand.begin(), hand.end(), [](const Card& a, const Card& b) {
        return a.seq < b.seq;
    });
}

int Player::cardAmount() {
    return hand.size();
}

int Player::shownCards() {
    int count = 0;
    for (auto & i : hand) {
        if (i.shown) count++;
    }
    return count;
}

void Player::initializeProb(const std::vector<Card>& humanHand){

    // seqVec -> Vector of cards(seq) that the computer does not know about
    seqVec.clear();
    seqVec.resize(24);
    std::iota(seqVec.begin(), seqVec.end(), 0);

    // Exclude cards that the computer holds from seqVec
    for (const auto& card : hand) {
        seqVec.erase(std::remove(seqVec.begin(), seqVec.end(), card.seq), seqVec.end());
    }

    // initialize prob vector

}

void Player::updateProb(const std::vector<Card>& humanHand){

    // Update seqVec - exclude human's shown cards from seqVec
    for (const auto& card : humanHand) {
        if (card.shown) seqVec.erase(std::remove(seqVec.begin(), seqVec.end(), card.seq), seqVec.end());
    }

    // Update seqVec - exclude new cards that the computer owns
    for (const auto& card : hand) {
        seqVec.erase(std::remove(seqVec.begin(), seqVec.end(), card.seq), seqVec.end());
    }

    // update prob vector


}

void Player::propagation(){
    // add propagation
}

std::tuple<int, int> Player::guessingAlgorithm(bool additional) {
    int minSize = prob[0].values.size();
    int targetIdx = 0;

    for (int i = 1; i < prob.size(); ++i) {
        if (!hand[i].shown && prob[i].values.size() < minSize) {
            minSize = prob[i].values.size();
            targetIdx = i;
        }
    }

    if (additional){
        if (minSize > 2) return {-1, -1};
        else if (minSize == 1) return {targetIdx, prob[targetIdx].values[0]};
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, prob[targetIdx].values.size() - 1);
    int guessNum = prob[targetIdx].values[dist(gen)];

    return {targetIdx, guessNum};
}
