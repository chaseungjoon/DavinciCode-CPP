//
// Created by 차승준 on 25. 5. 25.
//
#include "Player.h"
#include <random>

Player::Player() {
    this->hand.clear();
    this->prob.clear();
}

void Player::setAllShown() {
    for (auto & i : hand) {
        i.shown = true;
    }
}

bool Player::contains(const std::vector<int>& vec, int target){
    return std::find(vec.begin(), vec.end(), target) != vec.end();
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

    // seqVec -> Vector of cards(seq) that the computer know about
    seqVec.clear();
    for (const auto& card: hand){
        seqVec.push_back(card.seq);
    }

    std::sort(seqVec.begin(), seqVec.end());

    blackValVec.clear();
    whiteValVec.clear();

    for (int seq : seqVec){
        if (seq%2==0) blackValVec.push_back(seq/2);
        else whiteValVec.push_back(seq/2);
    }

    // count and index black/white cards in humanHand
    int blackCount = 0;
    int blackIdx = 0;
    int whiteCount = 0;
    int whiteIdx = 0;
    for (const auto& card : humanHand) {
        if (card.color == "black") blackCount++;
        else whiteCount++;
    }

    // initialize prob vector - color, values (black, white)
    prob.clear();
    for (const auto & i : humanHand) {
        probData newProb;
        newProb.color = i.color;

        if (newProb.color == "black") {
            for (int j = blackIdx; j < 13 - (blackCount-blackIdx); ++j) {
                if (!contains(blackValVec,j)) newProb.values.push_back(j);
            }
            blackIdx++;
        } else {
            for (int j = whiteIdx; j < 13 - (whiteCount-whiteIdx); ++j) {
                if (!contains(whiteValVec,j)) newProb.values.push_back(j);
            }
            whiteIdx++;
        }
        prob.push_back(newProb);
    }
    adjustProb(humanHand);
}

void Player::adjustProb(const std::vector<Card> &humanHand) {

    // count and index black/white cards in humanHand
    std::vector<int> whiteIdxes;
    std::vector<int> blackIdxes;
    for (int i = 0; i < humanHand.size(); i++) {
        if (humanHand[i].color == "black") blackIdxes.push_back(i);
        else whiteIdxes.push_back(i);
    }

    /// BLACK + BLACK / WHITE + WHITE CARD RELATIONSHIPS
    int lowerBound;
    int upperBound;

    // Black card relationship
    for (int i = 0; i < blackIdxes.size(); i++){
        if (humanHand[blackIdxes[i]].shown) continue;

        if (i==0) lowerBound = -1;
        else lowerBound = *std::min_element(
                    prob[blackIdxes[i - 1]].values.begin(),
                    prob[blackIdxes[i - 1]].values.end()
            );

        if (i==blackIdxes.size()-1) upperBound = 12;
        else upperBound = *std::max_element(
                    prob[blackIdxes[i + 1]].values.begin(),
                    prob[blackIdxes[i + 1]].values.end()
            );

        // remove values leq/geq than lowerBound/upperBound
        auto& vals = prob[blackIdxes[i]].values;

        vals.erase(std::remove_if(vals.begin(), vals.end(), [&](int val) {
            return val <= lowerBound;
        }), vals.end());

        vals.erase(std::remove_if(vals.begin(), vals.end(), [&](int val) {
            return val >= upperBound;
        }), vals.end());
    }

    // White card relationship - same with black
    for (int i = 0; i < whiteIdxes.size(); i++){
        if (humanHand[whiteIdxes[i]].shown) continue;

        if (i==0) lowerBound = -1;
        else lowerBound = *std::min_element(
                    prob[whiteIdxes[i - 1]].values.begin(),
                    prob[whiteIdxes[i - 1]].values.end()
            );

        if (i==whiteIdxes.size()-1) upperBound = 12;
        else upperBound = *std::max_element(
                    prob[whiteIdxes[i + 1]].values.begin(),
                    prob[whiteIdxes[i + 1]].values.end()
            );

        // remove values leq/geq than lowerBound/upperBound
        auto& vals = prob[whiteIdxes[i]].values;

        vals.erase(std::remove_if(vals.begin(), vals.end(), [&](int val) {
            return val <= lowerBound;
        }), vals.end());

        vals.erase(std::remove_if(vals.begin(), vals.end(), [&](int val) {
            return val >= upperBound;
        }), vals.end());
    }


    /// BLACK + WHITE CARD RELATIONSHIPS

    // when there are at least one white card
    if (!whiteIdxes.empty()){
        int left_closest_black_idx = -1;
        int right_closest_black_idx = -1;

        // get the right/left closest black card and fix the value vector
        for (int whiteIdx : whiteIdxes){
            if (humanHand[whiteIdx].shown) continue;

            left_closest_black_idx = -1;
            right_closest_black_idx = -1;

            for (int j = 0; j < whiteIdx; j++){
                if (humanHand[j].color == "black") left_closest_black_idx = j;
            }
            for (int j = humanHand.size()-1; j > whiteIdx; j--){
                if (humanHand[j].color == "black") right_closest_black_idx = j;
            }

            // Delete values that are smaller(larger) than min(max) lcb(rcb)
            if (left_closest_black_idx != -1) {
                lowerBound = *std::min_element(
                        prob[left_closest_black_idx].values.begin(),
                        prob[left_closest_black_idx].values.end()
                        );
                auto& vals = prob[whiteIdx].values;

                vals.erase(std::remove_if(vals.begin(), vals.end(), [&](int val) {
                    return val < lowerBound;
                }), vals.end());
            }

            if (right_closest_black_idx != -1) {
                upperBound = *std::max_element(
                        prob[right_closest_black_idx].values.begin(),
                        prob[right_closest_black_idx].values.end()
                        );
                auto& vals = prob[whiteIdx].values;

                vals.erase(std::remove_if(vals.begin(), vals.end(), [&](int val) {
                    return val >= upperBound;
                }), vals.end());
            }
        }
    }

    // when there are at least one black card
    if (!blackIdxes.empty()){
        int left_closest_white_idx = -1;
        int right_closest_white_idx = -1;

        // get the right/left closest white card and fix the value vector
        for (int blackIdx : blackIdxes){
            if (humanHand[blackIdx].shown) continue;

            left_closest_white_idx = -1;
            right_closest_white_idx = -1;

            for (int j = 0; j < blackIdx; j++){
                if (humanHand[j].color == "white") left_closest_white_idx = j;
            }
            for (int j = humanHand.size()-1; j > blackIdx; j--){
                if (humanHand[j].color == "white") right_closest_white_idx = j;
            }

            // Delete values that are smaller(larger) than min(max) lcw(rcw)
            if (left_closest_white_idx != -1) {
                lowerBound = *std::min_element(
                        prob[left_closest_white_idx].values.begin(),
                        prob[left_closest_white_idx].values.end()
                        );
                auto& vals = prob[blackIdx].values;

                vals.erase(std::remove_if(vals.begin(), vals.end(), [&](int val) {
                    return val <= lowerBound;
                }), vals.end());
            }

            if (right_closest_white_idx != -1) {
                upperBound = *std::max_element(
                        prob[right_closest_white_idx].values.begin(),
                        prob[right_closest_white_idx].values.end()
                        );
                auto& vals = prob[blackIdx].values;

                vals.erase(std::remove_if(vals.begin(), vals.end(), [&](int val) {
                    return val > upperBound;
                }), vals.end());
            }
        }
    }

    // FOR DEBUGGING
    if (debugMode) debugScreen(humanHand);
}

void Player::updateProb(std::vector<Card>& humanHand){

    // Update seqVec - include human's shown cards from seqVec
    for (const auto& card : humanHand){
        if (card.shown && !contains(seqVec, card.seq)) seqVec.push_back(card.seq);
    }

    // Update seqVec - include new cards that the computer owns
    for (const auto& card : hand){
        if (!contains(seqVec, card.seq)) seqVec.push_back(card.seq);
    }

    std::sort(seqVec.begin(), seqVec.end());

    blackValVec.clear();
    whiteValVec.clear();

    for (int seq : seqVec){
        if (seq%2==0) blackValVec.push_back(seq/2);
        else whiteValVec.push_back(seq/2);
    }

    // Add newly added card to prob
    while (prob.size() < humanHand.size()) {
        probData newData;
        int newlyDrawnIdx;
        for (int i = 0; i < humanHand.size(); i++){
            if (humanHand[i].newlyDrawn) {
                newData.color = humanHand[i].color;
                newlyDrawnIdx = i;
                break;
            }
        }

        // add values that are not in ValVec
        if (newData.color=="black"){
            for (int i = 0; i < 12; i++){
                if (!contains(blackValVec,i)) newData.values.push_back(i);
            }
        }
        else {
            for (int i = 0; i < 12; i++){
                if (!contains(whiteValVec,i)) newData.values.push_back(i);
            }
        }
        prob.insert(prob.begin() + newlyDrawnIdx, newData);
    }

    // If shown, collapse into known value
    for (int i = 0; i < humanHand.size(); i++) {
        if (humanHand[i].shown) prob[i].values = {humanHand[i].number};
    }

    // For hidden cards, get rid of numbers within probs that is in ValVec
    for (int i = 0; i < prob.size(); i++) {
        if (humanHand[i].shown) continue;
        if (prob[i].color=="black"){
            prob[i].values.erase(
                    std::remove_if(prob[i].values.begin(), prob[i].values.end(),
                                   [&](int x) {
                        return std::find(blackValVec.begin(), blackValVec.end(), x) != blackValVec.end();
                    }),
                    prob[i].values.end());
        }
        else{
            prob[i].values.erase(
                    std::remove_if(prob[i].values.begin(), prob[i].values.end(),
                                   [&](int x) {
                                       return std::find(whiteValVec.begin(), whiteValVec.end(), x) != whiteValVec.end();
                                   }),
                    prob[i].values.end());
        }
    }
    // adjust neighboring cards values
    adjustProb(humanHand);
}

void Player::deleteFromProb(int probIdx, int target) {
    auto& vec = prob[probIdx].values;
    vec.erase(std::remove(vec.begin(), vec.end(), target), vec.end());
}


std::tuple<int, int> Player::guessingAlgorithm(const std::vector<Card>& humanHand, bool additional) {
    int minSize = 12;
    int targetIdx = -1;

    // find the card that has the smallest value vector
    for (int i = 0; i < prob.size(); i++) {
        if (!humanHand[i].shown && prob[i].values.size() < minSize) {
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

void Player::debugScreen(const std::vector<Card>& humanHand) {
    std::cout << "\n **COMPUTER CARDS** \n";
    for (auto & i : hand){
        std::cout << i.color << i.number << "  ";
    }
    std::cout << std::endl;

    std::cout << "\n **seqVec** \n";
    for (int i : seqVec){
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "\n **blackValVec** \n";
    for (int i : blackValVec){
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "\n **whiteValVec** \n";
    for (int i : whiteValVec){
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "\n **PROB VECTOR VALUES** \n";
    for (int i = 0; i<prob.size(); i++){
        auto& vec = prob[i].values;
        if (humanHand[i].shown) std::cout << prob[i].color << "(SHOWN) -> ";
        else std::cout << prob[i].color << ": ";

        for (int j : vec){
            std::cout << j << ' ';
        }
        std::cout << std::endl;
    }
}