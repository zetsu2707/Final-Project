// Header file for Roulette minigame/Roulette game class.
// Related Files: Roulette.cpp, Player.h
// Date Created: 3/29/2026
// Last Edited: 04/05/2026

#pragma once
#include "Player.h"
#include <vector>
#include <string>
#include <random>

class Roulette {
public:
    Roulette();

    void play(Player& player);

private:
    struct Bet {
        std::string type;
        int number;
        double amount;
    };

    int spinWheel();
    bool isRed(int number) const;
    void showBanner() const;
    double getBetAmount(const Player& player) const;
    int getValidatedNumber(int min, int max) const;

    std::mt19937 m_rng;
};
