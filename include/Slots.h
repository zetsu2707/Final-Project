// Header file for Slots minigame/Slots game class.
// Related Files: Slots.cpp, CasinoGame.h, SlotsUI.h
// Date Created: 3/29/2026
// Last Edited: 4/24/2026

#pragma once
#include "CasinoGame.h"
#include <string>
#include <vector>

class Slots : public CasinoGame {
public:
    struct Symbol {
        std::string display;
        int         weight;      // higher = more common
        double      multiplier;  // payout multiplier on three-of-a-kind match
    };

    Slots();

    std::string getName() const override { return "Slots"; }
    bool didPlayerWin() const override { return m_lastSpinWon; }

    // Console-mode entry point (kept for compatibility).
    void play(Player& player);

    // UI-friendly interface
    std::vector<Symbol> spinReels();
    double evaluate(const std::vector<Symbol>& result) const;
    const std::vector<Symbol>& getSymbols() const;

private:
    std::vector<Symbol> m_symbols;
    bool m_lastSpinWon = false;

    void   initializeSymbols();
    void   showBanner()  const;
    void   showReels(const std::vector<Symbol>& result) const;
    double getBet(const Player& player) const;

    template <typename T>
    T getValidatedInput(T min, T max) const;
};
