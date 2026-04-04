// Header file for Horse Racing minigame/Horse Racing game class.
// Related Files: HorseRace.cpp, Player.h
// Date Created: 3/29/2026
// Last Edited: 4/4/2026

#pragma once
#include "CasinoGame.h"
#include "Player.h"

#include <array>
#include <random>
#include <string>
#include <vector>

class HorseRacing {
public:
    HorseRacing();

    // Main entry point for this minigame
    void play(Player& player);

private:
    struct Horse {
        std::string color;
        double payoutMultiplier; // capped so payout never exceeds 2x bet
        double strengthWeight;   // higher = more likely to place well
        double finishScore;      // generated each race
    };

    std::array<Horse, 8> m_horses;
    std::mt19937 m_rng;

    void initializeHorses();

    void showLoadedBanner() const;
    void showMenu() const;
    void showHorseList() const;

    int getMainMenuChoice() const;
    double getBetAmount(const Player& player) const;
    int getHorseChoice() const;

    void runRace();
    std::vector<Horse> getRaceResults() const;
    void displayTopThree(const std::vector<Horse>& results) const;

    static void clearInput();
};