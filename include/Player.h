// Header file for Player class.
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
    // Constructor
    Player(const std::string_view name, double startingBalance = 200.0);
    

    // Saving Functionailty
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);

    // Core Gameplay
    bool placeBet(double amount);     // deducts from balance, returns false if insufficient
    void addWinnings(double amount);  // adds to balance
    bool hasSufficientFunds(double amount) const;

    // Setter / Getters
    std::string getName() const;
    double getBalance() const;
    double getTotalBetted() const;
    double getTotalWon() const;
    int getGamesPlayed() const;
    int getGamesWon() const;
    double getNetProfit() const;


private:
    std::string m_name;
    double m_balance;
    double m_totalBetted;
    double m_totalWon;
    int m_gamesPlayed;
    int m_gamesWon;
};



