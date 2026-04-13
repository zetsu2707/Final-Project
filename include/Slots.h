// Header file for Slots minigame/Slots game class.
// Related Files: Slots.cpp, Player.h, SlotsUI.h
// Date Created: 3/29/2026
// Last Edited: 4/13/2026

#pragma once
#include "Player.h"
#include <string>
#include <vector>
#include <random>

class Slots {
public:
    // Public so SlotsUI (and other callers) can work with symbol data directly.
    struct Symbol {
        std::string display;
        int         weight;      // higher = more common
        double      multiplier;  // payout multiplier on three-of-a-kind match
    };

    Slots();

    // Console-mode entry point (kept for compatibility).
    void play(Player& player);

    // UI-friendly interface ---------------------------------------------------
    // Spin the three reels and return the resulting symbols.
    std::vector<Symbol> spinReels();

    // Given a spin result, return the payout multiplier
    // (0.0 = no win, 0.5 = two-of-a-kind, >0.5 = three-of-a-kind multiplier).
    double evaluate(const std::vector<Symbol>& result) const;

    // Read-only access to the full symbol table (used to draw payout tables).
    const std::vector<Symbol>& getSymbols() const;

private:
    std::vector<Symbol> m_symbols;
    std::mt19937        m_rng;

    void                initializeSymbols();
    void                showBanner()  const;
    void                showReels(const std::vector<Symbol>& result) const;

    // Internal helpers used by play()
    double              getBet(const Player& player) const;

    template <typename T>
    T getValidatedInput(T min, T max) const;
};
