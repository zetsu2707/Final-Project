// Header file for CasinoGame class.
// Related Files:
// Date Created: 3/29/2026
// Last Edited: 3/29/2026

#pragma once
#include "Player.h"
#include <string>
#include <random>

class CasinoGame {
public:
    explicit CasinoGame(std::string name, double minimumBet = 1.0);
    virtual ~CasinoGame() = default;

    // Every subclass must implement this
    virtual void play(Player& player) = 0;

    const std::string& getName() const;
    double getMinimumBet() const;

protected:
    // Shared helpers available to all subclasses
    double getBetAmount(const Player& player) const;
    void displayBalance(const Player& player) const;
    static void clearInput();

    std::string m_name;
    double m_minimumBet;
    std::mt19937 m_rng;
};