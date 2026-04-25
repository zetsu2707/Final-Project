// Header file for CasinoGame abstract base class for all casino games.
// Related Files: CasinoGame.cpp, Player.h, CardGame.h
// Date Created: 3/29/2026
// Last Edited: 4/24/2026

#pragma once

#include "Player.h"
#include <random>
#include <string>

class CasinoGame {
public:
    CasinoGame();
    virtual ~CasinoGame() = default;

    // Every game must provide its display name.
    virtual std::string getName() const = 0;

    // Optional: games can override this to report whether the player
    // won the last round/hand/race. Defaults to false.
    virtual bool didPlayerWin() const { return false; }

protected:
    // Shared RNG available to all derived game classes.
    // Seeded once in the CasinoGame constructor via std::random_device.
    std::mt19937 m_rng;
};
