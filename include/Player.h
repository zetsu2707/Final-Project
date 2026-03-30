// Header file for Horse Racing minigame/Horse Racing game class.
// Related Files: HorseRacing.cpp, Player.h
// Date Created: 3/29/2026
// Last Edited: 3/29/2026

#pragma once
#include <string>
#include <string_view> 

/**
 * @class Player
 * Represents the player in the casino game.
 * Tracks player identity, balance, debt,
 * and statistics across all casino games.
 */

class Player {
public:
    Player(const std::string_view name, double startingBalance = 200.0);

    bool saveToFile(const std::string& filepath) const;

private:
    std::string m_name;
    double m_balance;
    double m_totalBetted;
    double m_totalWon;
};



