// Header file for CasinoGame menu/controller class.
// Related Files: CasinoGame.cpp, Player.h, Blackjack.h, HorseRace.h, Roulette.h, Slots.h, Poker.h
// Date Created: 3/29/2026
// Last Edited: 4/10/2026

#pragma once

#include "Player.h"

class CasinoGame {
public:
    CasinoGame() = default;
    void run(Player& player);
};