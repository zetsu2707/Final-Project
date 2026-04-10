// Header file for Horse Racing minigame/Horse Racing game class.
// Related Files: HorseRace.cpp, Player.h
// Date Created: 3/29/2026
// Last Edited: 4/10/2026

#pragma once
#include "Player.h"

#include <array>
#include <random>
#include <string>
#include <vector>

class HorseRacing {
public:
    HorseRacing();

    static constexpr int HorseCount = 8;
    static constexpr int RacePhaseCount = 5;

    struct Horse {
        std::string color{};
        double payoutMultiplier = 0.0;
        double strengthWeight = 0.0;
        double finishScore = 0.0;
        std::array<double, RacePhaseCount> phaseScores{};
        double currentProgress = 0.0;
    };

    struct RaceState {
        bool started = false;
        bool finished = false;
        int currentPhase = 0;
    };

    const std::array<Horse, HorseCount>& getHorses() const;
    const RaceState& getRaceState() const;

    void beginRace();
    bool advanceRacePhase();
    void runRace();

    std::vector<Horse> getRaceResults() const;

    double getHorseProgress(int horseIndex) const;
    double getLeaderProgress() const;

    bool isRaceStarted() const;
    bool isRaceFinished() const;

    bool isValidHorseChoice(int horseChoice) const;
    bool canPlaceBet(const Player& player, double betAmount) const;

    bool resolveBet(Player& player, double betAmount, int horseChoice);

    bool didPlayerWinLastBet() const;
    double getLastPayout() const;

private:
    std::array<Horse, HorseCount> m_horses;
    std::mt19937 m_rng;

    RaceState m_raceState;
    bool m_lastBetWon = false;
    double m_lastPayout = 0.0;

    void initializeHorses();
    void resetRaceData();
};