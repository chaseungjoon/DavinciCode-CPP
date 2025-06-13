//
// Created by 차승준 on 25. 5. 25.
//
#include "Game.h"
#include <random>
#include <algorithm>
#include <thread>

void Game::runGameLoop() {
    initialize();

    while (true) {
        printVisual();

        playerTurn();
        if (gameOverCheck()) break;

        computerTurn();
        if (gameOverCheck()) break;

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    human.setAllShown();
    computer.setAllShown();
    printVisual();
}

void Game::playerTurn() {
    for (Card& card : human.getHand()) {
        if (card.newlyDrawn) {
            card.resetDrawn();
        }
    }

    // Draw card
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, deck.size() - 1);

    int index = dist(gen);
    Card selected = deck[index];
    selected.newlyDrawn = true;

    human.drawCard(selected);
    deck.erase(deck.begin() + index);

    // Guess
    int guessPos;
    int guessNum;

    while (true){
        std::cout << "Guess (position, number) >> ";
        std::cin >> guessPos >> guessNum;

        if (computer.getHand()[guessPos].shown) std::cout << "\nYou can't guess revealed cards!\n\n";
        else break;
    }

    // Correct
    if (computer.getHand()[guessPos].number == guessNum) {

        std::cout << "\nCorrect!\n";
        computer.getHand()[guessPos].reveal();

        //Additional Guess
        while (!gameOverCheck()){
            std::cout << "Additional Guess (position, number)(-1 if not guessing) >> ";
            std::cin >> guessPos;
            if (guessPos == -1) return;
            std::cin >> guessNum;

            // Correct
            if (computer.getHand()[guessPos].number == guessNum) {
                std::cout << "\nCorrect!\n";
                computer.getHand()[guessPos].reveal();
            }
            // Not correct
            else {
                std::cout << "\nWrong!\n";
                for (Card& card : human.getHand()) {
                    if (card.seq == selected.seq) {
                        card.reveal();
                        break;
                    }
                }
                break;
            }
        }
    }
    // Not correct
    else {
        std::cout << "\nWrong!\n";
        for (Card& card : human.getHand()) {
            if (card.seq == selected.seq) {
                card.reveal();
                break;
            }
        }
    }
}

void Game::computerTurn() {
    for (Card& card : computer.getHand()) {
        if (card.newlyDrawn) {
            card.resetDrawn();
        }
    }

    // Draw card
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, deck.size() - 1);

    int index = dist(gen);
    Card selected = deck[index];
    selected.newlyDrawn = true;

    computer.drawCard(selected);
    deck.erase(deck.begin() + index);
    computer.updateProb(human.getHand());

    // Guess
    auto [guessPos, guessNum] = computer.guessingAlgorithm(human.getHand(), false);
    std::cout << "\nComputer Guess : " << guessPos << "th is " << guessNum << "\n";

    // Correct
    if (human.getHand()[guessPos].number == guessNum) {

        std::cout << "\nComputer Correct!\n";
        human.getHand()[guessPos].reveal();

        // Update prob
        computer.updateProb(human.getHand());

        // Computer Additional Guess
        while (true){
            auto [guessPosAdditional, guessNumAdditional] = computer.guessingAlgorithm(human.getHand(), true);
            if (guessPosAdditional == -1) return;
            std::cout << "\nComputer "
                         " : " << guessPosAdditional << "th is " << guessNumAdditional << "\n";

            // Correct
            if (human.getHand()[guessPosAdditional].number == guessNumAdditional) {
                std::cout << "\nComputer Correct!\n";
                human.getHand()[guessPosAdditional].reveal();

                // Update prob
                computer.updateProb(human.getHand());
            }
            // Not correct
            else {
                std::cout << "\nComputer Wrong!\n";
                // get rid of incorrect guess from prob vector
                computer.deleteFromProb(guessPosAdditional, guessNumAdditional);

                // reveal most recently drawn card
                for (Card& card : computer.getHand()) {
                    if (card.seq == selected.seq) {
                        card.reveal();
                        break;
                    }
                }
                break;
            }
        }
    }
    // Not correct
    else {
        std::cout << "\nComputer Wrong!\n";

        // get rid of incorrect guess from prob vector
        computer.deleteFromProb(guessPos, guessNum);

        // reveal most recently drawn card
        for (Card& card : computer.getHand()) {
            if (card.seq == selected.seq) {
                card.reveal();
                break;
            }
        }
    }
}

Game::Game() : human(), computer() {}

void Game::initialize() {
    for (int i = 0; i < 24; i++){
        deck.push_back(Card(i));
    }

    // Draw 8 random cards
    std::vector<int> nums(24);
    std::iota(nums.begin(), nums.end(), 0); // 0 ~ 23
    std::shuffle(nums.begin(), nums.end(), std::mt19937(std::random_device{}()));
    nums.resize(8);

    // Give the first 4 cards to human
    for (int i = 0; i < 4; ++i) {
        human.drawCard(deck[nums[i]]);
    }

    // Give the next 4 cards to computer
    for (int i = 4; i < 8; ++i) {
        computer.drawCard(deck[nums[i]]);
    }

    // Remove drawn cards from deck
    std::sort(nums.begin(), nums.begin() + 8, std::greater<>());
    for (int i = 0; i < 8; ++i) {
        deck.erase(deck.begin() + nums[i]);
    }

    // Initialize prob
    computer.initializeProb(human.getHand());
}

void Game::printVisual() {
    const std::string line_3_l_w = "┃  ";
    const std::string line_3_r_w = "  ┃";
    const std::string line_3_l_b = "┃■■";
    const std::string line_3_r_b = "■■┃";
    const std::string line_white_1 = "┏━━━━━━┓";
    const std::string line_white_2 = "┃      ┃";
    const std::string line_white_3 = "┗━━━━━━┛";
    const std::string line_black_1 = "┃■■■■■■┃";
    std::string temp_num = "??";

    const auto& oppHand = computer.getHand();
    const auto& myHand = human.getHand();

    std::string Line_0, Line_1, Line_2, Line_3, Line_4, Line_5, Line_7;

    // OPPONENT CARDS
    for (int i = 0; i < oppHand.size(); i++) {
        const Card& card = oppHand[i];

        Line_7 += "    " + std::to_string(i) + "     ";
        if (card.newlyDrawn) Line_0 += "  NEW";
        else Line_0 += "          ";

        if (!card.shown) {
            Line_1 += line_white_1 + "  ";
            Line_2 += (card.color == "white" ? line_white_2 : line_black_1) + "  ";
            Line_4 += (card.color == "white" ? line_white_2 : line_black_1) + "  ";
            Line_5 += line_white_3 + "  ";
        } else {
            Line_1 += "          ";
            Line_2 += "          ";
            Line_3 += (card.color == "white" ? "━━━━━━━   " : "■■■■■■■   ");
            Line_5 += (card.color == "white" ? " ━━━━━━━  " : " ■■■■■■■  ");
        }

        if (card.shown) {
            temp_num = card.number < 10 ? ' ' + std::to_string(card.number) : std::to_string(card.number);
            Line_4 += (card.color == "black" ? "\\■■" + temp_num + "■■\\  " : "\\  " + temp_num + "  \\  ");
        } else {
            Line_3 += (card.color == "black" ? line_3_l_b + temp_num + line_3_r_b : line_3_l_w + temp_num + line_3_r_w) + "  ";
        }
        temp_num = "??";
    }

    std::cout << "\n<Opponent Cards>\n\n";
    std::cout << Line_0 << '\n' << Line_1 << '\n' << Line_2 << '\n' << Line_3 << '\n' << Line_4 << '\n' << Line_5 << "\n\n" << Line_7 << "\n\n";


    // RESET FOR PLAYER
    Line_0 = Line_1 = Line_2 = Line_3 = Line_4 = Line_5 = Line_7 = "";

    // PLAYER CARDS
    for (int i = 0; i < myHand.size(); i++) {
        const Card& card = myHand[i];

        Line_7 += "    " + std::to_string(i) + "     ";
        if (card.newlyDrawn) Line_0 += "  NEW";
        else Line_0 += "          ";

        if (!card.shown) {
            Line_1 += line_white_1 + "  ";
            Line_2 += (card.color == "white" ? line_white_2 : line_black_1) + "  ";
            Line_4 += (card.color == "white" ? line_white_2 : line_black_1) + "  ";
            Line_5 += line_white_3 + "  ";
        } else {
            Line_1 += "          ";
            Line_2 += "          ";
            Line_3 += (card.color == "white" ? " ━━━━━━━  " : " ■■■■■■■  ");
            Line_5 += (card.color == "white" ? "━━━━━━━   " : "■■■■■■■   ");
        }

        temp_num = card.number < 10 ? ' ' + std::to_string(card.number) : std::to_string(card.number);
        if (!card.shown) {
            Line_3 += (card.color == "black" ? line_3_l_b + temp_num + line_3_r_b : line_3_l_w + temp_num + line_3_r_w) + "  ";
        } else {
            Line_4 += (card.color == "black" ? "/■■" + temp_num + "■■/  " : "/  " + temp_num + "  /  ");
        }
    }

    std::cout << "<My Cards>\n\n";
    std::cout << Line_0 << '\n' << Line_1 << '\n' << Line_2 << '\n' << Line_3 << '\n' << Line_4 << '\n' << Line_5 << "\n\n" << Line_7 << "\n";
}

bool Game::gameOverCheck() {
    if (computer.shownCards() == computer.cardAmount()) {
        std::cout << "\nYou Win!\n";
        return true;
    }
    else if (human.shownCards() == human.cardAmount()) {
        std::cout << "\nYou Lose!\n";
        return true;
    }
    else return false;
}