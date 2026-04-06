// Header file for CasinoGame menu/controller class.
// Related Files: CasinoGame.cpp, Player.h, Blackjack.h, HorseRace.h, Roulette.h, Slots.h, Poker.h
// Date Created: 3/29/2026
// Last Edited: 4/5/2026

#pragma once

#include "Player.h"
#include <string>

class CasinoGame {
public:
    CasinoGame() = default;
    void run(Player& player);

private:
    void displayMenu(const Player& player) const;
    void displayPlayerStats(const Player& player) const;
    void saveGame(Player& player) const;

    int getValidatedInt(const std::string& prompt, int minValue, int maxValue) const;
};